import asyncio
import httpx
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
import torch
import torch.nn as nn
import pandas as pd
import numpy as np
import joblib
import math

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# --- Configuration ---
BANGALORE_LAT = 12.9007 # RNSIT College, Bangalore
BANGALORE_LON = 77.5174 # RNSIT College, Bangalore
SEQ_LENGTH = 10
HIDDEN_SIZE = 256
NUM_LAYERS = 3

EXO_COLS = ['Irradiance', 'Temp']
TARGET_COLS = ['Vrms_1', 'Vrms_2', 'Vrms_3', 'Irms_1', 'Irms_2', 'Irms_3', 'Pac', 'Q', 
               'THDv_1', 'THDv_2', 'THDv_3', 'THDi_1', 'THDi_2', 'THDi_3', 'Vdc', 'Ipv', 'Vpv', 'Pdc']
ALL_INPUT_COLS = EXO_COLS + TARGET_COLS

class LSTMModel(nn.Module):
    def __init__(self, input_size, hidden_size, num_layers, output_size):
        super(LSTMModel, self).__init__()
        self.hidden_size = hidden_size
        self.num_layers = num_layers
        self.lstm = nn.LSTM(input_size, hidden_size, num_layers, batch_first=True, dropout=0.2 if num_layers > 1 else 0)
        self.fc1 = nn.Linear(hidden_size, hidden_size // 2)
        self.relu = nn.ReLU()
        self.fc2 = nn.Linear(hidden_size // 2, output_size)
        
    def forward(self, x):
        out, _ = self.lstm(x)
        out = out[:, -1, :]
        out = self.fc1(out)
        out = self.relu(out)
        out = self.fc2(out)
        return out

# --- Global State ---
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
model = LSTMModel(len(ALL_INPUT_COLS), HIDDEN_SIZE, NUM_LAYERS, len(TARGET_COLS)).to(device)
try:
    model.load_state_dict(torch.load('lstm_best_model.pth', map_location=device))
    model.eval()
    x_scaler = joblib.load('x_scaler.pkl')
    y_scaler = joblib.load('y_scaler.pkl')
except Exception as e:
    print("Warning: Model or scalers not found. Ensure training is complete.", e)

# The state buffer maintains the last 10 rows of ALL_INPUT_COLS
# Initialize with some typical average values from the dataset so it doesn't crash on start
state_buffer = np.zeros((SEQ_LENGTH, len(ALL_INPUT_COLS)))
history_data = [] # To send historical points to the frontend for charting
MAX_HISTORY = 60 # Keep 60 seconds (or ticks) of history

current_weather = {"Irradiance": 0, "Temp": 0}
client = httpx.AsyncClient()

async def fetch_weather():
    global current_weather
    try:
        # Open-Meteo Current Weather API
        url = f"https://api.open-meteo.com/v1/forecast?latitude={BANGALORE_LAT}&longitude={BANGALORE_LON}&current=temperature_2m,direct_radiation"
        response = await client.get(url)
        data = response.json()
        current = data.get("current", {})
        
        # Mapping their fields to ours
        current_weather["Temp"] = current.get("temperature_2m", 25)
        # We might use direct_radiation or shortwave_radiation. Direct radiation is a good proxy for Irradiance.
        # Add some noise to make the chart look "alive" even when weather updates slowly (every 15 min)
        noise = np.random.normal(0, 10)
        current_weather["Irradiance"] = max(0, current.get("direct_radiation", 800) + noise)
        
    except Exception as e:
        print("Error fetching weather:", e)

def predict_next_step():
    global state_buffer, history_data, current_weather
    
    # We have current_weather and we need to predict the NEXT target variables.
    # The last row of our state buffer currently has some target variables.
    # But wait, our autoregressive model needs X[t] = [exo(t), target(t)] to predict target(t+1).
    # Since we don't have true target(t+1), we use the prediction as the new target(t+1).
    
    # 1. Prepare sequence input
    seq = state_buffer.copy()
    
    # Update the very last element of the sequence with the CURRENT weather, 
    # to let the model know the latest weather conditions.
    seq[-1, 0] = current_weather["Irradiance"]
    seq[-1, 1] = current_weather["Temp"]
    
    # 2. Scale
    seq_scaled = x_scaler.transform(seq)
    seq_tensor = torch.tensor(seq_scaled, dtype=torch.float32).unsqueeze(0).to(device)
    
    # 3. Predict
    with torch.no_grad():
        pred_scaled = model(seq_tensor).cpu().numpy()
        
    # 4. Inverse Scale
    pred_raw = y_scaler.inverse_transform(pred_scaled)[0]
    
    # 5. Shift buffer and append new state
    new_state = np.zeros(len(ALL_INPUT_COLS))
    new_state[0] = current_weather["Irradiance"]
    new_state[1] = current_weather["Temp"]
    new_state[2:] = pred_raw
    
    state_buffer = np.roll(state_buffer, -1, axis=0)
    state_buffer[-1] = new_state
    
    # 6. Save to history
    history_point = {
        "Irradiance": float(current_weather["Irradiance"]),
        "Temp": float(current_weather["Temp"]),
    }
    for i, col in enumerate(TARGET_COLS):
        # Apply absolute value to THD if it goes negative due to model error
        val = float(pred_raw[i])
        if "THD" in col:
            val = max(0, val)
        history_point[col] = val
        
    history_data.append(history_point)
    if len(history_data) > MAX_HISTORY:
        history_data.pop(0)

@app.on_event("startup")
async def startup_event():
    # Pre-fill buffer with rough averages so the chart doesn't start at pure zeros and jump wildly
    for i in range(SEQ_LENGTH):
        state_buffer[i, 0] = 800 # Irr
        state_buffer[i, 1] = 25 # Temp
        # Fill targets with approx values (e.g. Vrms=14300, etc)
        for j, col in enumerate(TARGET_COLS):
            if "Vrms" in col: state_buffer[i, 2+j] = 14330
            elif "Irms" in col: state_buffer[i, 2+j] = 6.0
            elif "Pac" in col: state_buffer[i, 2+j] = 250000
            elif "Vdc" in col: state_buffer[i, 2+j] = 525
            elif "Vpv" in col: state_buffer[i, 2+j] = 525
            else: state_buffer[i, 2+j] = 0.0
            
    asyncio.create_task(background_loop())

async def background_loop():
    weather_counter = 0
    while True:
        # Weather API doesn't update every second, fetch every 10 ticks
        if weather_counter % 10 == 0:
            await fetch_weather()
        weather_counter += 1
        
        # We can add small random noise to weather just to simulate rapid micro-fluctuations
        current_weather["Irradiance"] = max(0, current_weather["Irradiance"] + np.random.normal(0, 2))
        
        predict_next_step()
        await asyncio.sleep(1) # Tick every 1 second

@app.get("/api/live_data")
async def get_live_data():
    if not history_data:
        return {"current": {}, "history": []}
    return {
        "current": history_data[-1],
        "history": history_data
    }

if __name__ == "__main__":
    import uvicorn
    uvicorn.run("live_server:app", host="0.0.0.0", port=8000, reload=True)
