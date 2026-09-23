"""
SolarTwin — LSTM Live Prediction Server
Fixes applied: BUG-07 (duplicate joblib), BUG-08 (O(n) pop), BUG-09 (irradiance random walk),
               BUG-16 (relative paths), Phase 4 (site coords configurable, deque, abs paths).
"""
import os
import asyncio
from collections import deque

import httpx
import joblib
import math
import numpy as np
import torch
import torch.nn as nn
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

# ── Resolve paths relative to this file so the server works regardless of CWD ──
# FIX BUG-16: use __file__ for all asset paths
_DIR  = os.path.dirname(os.path.abspath(__file__))
_ROOT = os.path.dirname(_DIR)   # one level up to repo root

# ── Site location (configurable via env vars — no longer hardcoded) ──────────
# FIX Phase 4.8: make lat/lon configurable
SITE_LAT = float(os.getenv("SITE_LAT", "12.9007"))  # default: RNSIT College, Bangalore
SITE_LON = float(os.getenv("SITE_LON", "77.5174"))

# ── Model / sequence configuration ───────────────────────────────────────────
SEQ_LENGTH  = 10
HIDDEN_SIZE = 256
NUM_LAYERS  = 3

EXO_COLS    = ['Irradiance', 'Temp']
TARGET_COLS = [
    'Vrms_1', 'Vrms_2', 'Vrms_3',
    'Irms_1', 'Irms_2', 'Irms_3',
    'Pac', 'Q',
    'THDv_1', 'THDv_2', 'THDv_3',
    'THDi_1', 'THDi_2', 'THDi_3',
    'Vdc', 'Ipv', 'Vpv', 'Pdc',
]
ALL_INPUT_COLS = EXO_COLS + TARGET_COLS

MAX_HISTORY = 3600   # keep 1 hour (or 3600 ticks) of history


# ── LSTM model definition ─────────────────────────────────────────────────────

class LSTMModel(nn.Module):
    def __init__(self, input_size, hidden_size, num_layers, output_size):
        super(LSTMModel, self).__init__()
        self.hidden_size = hidden_size
        self.num_layers  = num_layers
        self.lstm = nn.LSTM(
            input_size, hidden_size, num_layers,
            batch_first=True,
            dropout=0.2 if num_layers > 1 else 0,
        )
        self.fc1  = nn.Linear(hidden_size, hidden_size // 2)
        self.relu = nn.ReLU()
        self.fc2  = nn.Linear(hidden_size // 2, output_size)

    def forward(self, x):
        out, _ = self.lstm(x)
        out = out[:, -1, :]
        out = self.fc1(out)
        out = self.relu(out)
        out = self.fc2(out)
        return out


# ── FastAPI app ───────────────────────────────────────────────────────────────

app = FastAPI(title="SolarTwin LSTM Live Server")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# ── Global state ──────────────────────────────────────────────────────────────

device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

model = LSTMModel(
    input_size=len(ALL_INPUT_COLS),
    hidden_size=HIDDEN_SIZE,
    num_layers=NUM_LAYERS,
    output_size=len(TARGET_COLS),
).to(device)

# FIX BUG-07: load joblib import once; load scalers once in the try block
x_scaler = None
y_scaler = None

try:
    # FIX BUG-16: all paths are absolute, relative to _ROOT
    model.load_state_dict(
        torch.load(
            os.path.join(_ROOT, 'lstm_best_model.pth'),
            map_location=device,
        )
    )
    model.eval()
    x_scaler = joblib.load(os.path.join(_ROOT, 'x_scaler.pkl'))
    y_scaler = joblib.load(os.path.join(_ROOT, 'y_scaler.pkl'))
    print(f"[live_server] Model and scalers loaded from {_ROOT}")
except Exception as e:
    print(f"[live_server] Warning: could not load model/scalers: {e}")

# State buffer — last SEQ_LENGTH rows of ALL_INPUT_COLS (used for autoregressive input)
state_buffer = np.zeros((SEQ_LENGTH, len(ALL_INPUT_COLS)))

# FIX BUG-08: use deque with maxlen instead of list.pop(0) which is O(n)
history_data: deque = deque(maxlen=MAX_HISTORY)

# FIX BUG-09: store the last *fetched* base irradiance as an anchor
# Micro-noise is applied relative to this anchor, not the running value — prevents random walk
_base_irradiance: float = 800.0
current_weather: dict = {"Irradiance": 800.0, "Temp": 25.0}

client = httpx.AsyncClient()


# ── Weather fetching ──────────────────────────────────────────────────────────

async def fetch_weather():
    global current_weather, _base_irradiance
    try:
        url = (
            f"https://api.open-meteo.com/v1/forecast"
            f"?latitude={SITE_LAT}&longitude={SITE_LON}"
            f"&current=temperature_2m,direct_radiation"
        )
        response = await client.get(url, timeout=10.0)
        data = response.json()
        current = data.get("current", {})

        temp   = current.get("temperature_2m", 25.0)
        irr_raw = current.get("direct_radiation", 800.0)

        # FIX BUG-09: anchor the base irradiance to the fresh fetch;
        # micro-noise is applied relative to anchor, not accumulated
        _base_irradiance = float(irr_raw)
        current_weather["Temp"]       = float(temp)
        current_weather["Irradiance"] = max(0.0, _base_irradiance + np.random.normal(0, 5))

    except Exception as e:
        print(f"[live_server] Weather fetch error: {e}")


# ── LSTM Prediction ───────────────────────────────────────────────────────────

def predict_next_step():
    global state_buffer, current_weather

    if x_scaler is None or y_scaler is None:
        return   # model not loaded — skip silently

    # 1. Prepare sequence input — update last row with current weather
    seq = state_buffer.copy()
    seq[-1, 0] = current_weather["Irradiance"]
    seq[-1, 1] = current_weather["Temp"]

    # 2. Scale
    seq_scaled = x_scaler.transform(seq)
    seq_tensor = torch.tensor(seq_scaled, dtype=torch.float32).unsqueeze(0).to(device)

    # 3. Predict (MC Dropout for uncertainty estimation)
    model.train()  # enable dropout for MC sampling
    mc_preds = []
    with torch.no_grad():
        for _ in range(20):
            mc_preds.append(model(seq_tensor).cpu().numpy())
    model.eval()

    mc_stack = np.stack(mc_preds, axis=0)        # (20, 1, n_targets)
    pred_mean_scaled = mc_stack.mean(axis=0)     # (1, n_targets)
    pred_std_scaled  = mc_stack.std(axis=0)      # (1, n_targets)

    # 4. Inverse scale
    pred_raw = y_scaler.inverse_transform(pred_mean_scaled)[0]
    # Std in original units — approximate (std doesn't transform perfectly but useful)
    pred_std_raw = y_scaler.scale_ * pred_std_scaled[0]   # element-wise scale

    # 5. Shift state buffer and append new state
    new_state = np.zeros(len(ALL_INPUT_COLS))
    new_state[0] = current_weather["Irradiance"]
    new_state[1] = current_weather["Temp"]
    new_state[2:] = pred_raw

    state_buffer = np.roll(state_buffer, -1, axis=0)
    state_buffer[-1] = new_state

    # 6. Build history point with uncertainty bounds
    history_point = {
        "Irradiance": float(current_weather["Irradiance"]),
        "Temp":       float(current_weather["Temp"]),
    }
    for i, col in enumerate(TARGET_COLS):
        val = float(pred_raw[i])
        # Clamp physically impossible negatives (e.g. THD < 0)
        if "THD" in col:
            val = max(0.0, val)
        history_point[col] = val
        history_point[f"{col}_std"] = float(abs(pred_std_raw[i]))   # uncertainty band

    # FIX BUG-08: deque.append() is O(1) — no pop(0) needed (maxlen handles eviction)
    history_data.append(history_point)


# ── Startup & background loop ─────────────────────────────────────────────────

@app.on_event("startup")
async def startup_event():
    # Pre-fill state buffer with approximate steady-state values
    for i in range(SEQ_LENGTH):
        state_buffer[i, 0] = 800.0   # Irradiance
        state_buffer[i, 1] = 25.0    # Temp
        for j, col in enumerate(TARGET_COLS):
            if   "Vrms" in col: state_buffer[i, 2 + j] = 14330.0
            elif "Irms" in col: state_buffer[i, 2 + j] = 6.0
            elif "Pac"  in col: state_buffer[i, 2 + j] = 250000.0
            elif "Vdc"  in col: state_buffer[i, 2 + j] = 525.0
            elif "Vpv"  in col: state_buffer[i, 2 + j] = 525.0
            else:               state_buffer[i, 2 + j] = 0.0

    asyncio.create_task(background_loop())


async def background_loop():
    weather_counter = 0
    while True:
        # Fetch real weather every 10 ticks (10 seconds)
        if weather_counter % 10 == 0:
            await fetch_weather()
        else:
            # FIX BUG-09: micro-fluctuation always relative to anchored base — not a random walk
            current_weather["Irradiance"] = max(0.0, _base_irradiance + np.random.normal(0, 5))

        weather_counter += 1
        predict_next_step()
        await asyncio.sleep(1)


# ── API endpoints ─────────────────────────────────────────────────────────────

@app.get("/api/live_data")
async def get_live_data():
    if not history_data:
        return {"current": {}, "history": []}
    history_list = list(history_data)   # convert deque to list for JSON serialization
    return {
        "current": history_list[-1],
        "history": history_list,
    }


if __name__ == "__main__":
    import uvicorn
    uvicorn.run("live_server:app", host="0.0.0.0", port=8000, reload=True)
