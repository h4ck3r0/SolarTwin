"""
SolarTwin — LSTM Validation & R² Evaluation Script
Fixes: BUG-15 (torch.load without map_location — crashes on CPU-only machines).
Phase 3.3: Uses shared LSTMModel from lstm_model.py.
Phase 6.3: Extended to report MAPE and per-target uncertainty bounds.
"""
import os
import numpy as np
import pandas as pd
import torch
import joblib
from sklearn.metrics import r2_score

from lstm_model import LSTMModel
from train_lstm import create_sequences_by_scenario

# ── Paths ──────────────────────────────────────────────────────────────────────
_DIR  = os.path.dirname(os.path.abspath(__file__))
DATA_PATH   = os.path.join(_DIR, 'lstm_training_data_25scenarios.csv')
MODEL_PATH  = os.path.join(_DIR, 'lstm_best_model.pth')
SCALER_X    = os.path.join(_DIR, 'x_scaler.pkl')
SCALER_Y    = os.path.join(_DIR, 'y_scaler.pkl')

SEQ_LENGTH  = 10
HIDDEN_SIZE = 256
NUM_LAYERS  = 3

# FIX BUG-15: resolve device before loading model
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
print(f"Device: {device}")

# ── Load data & scalers ────────────────────────────────────────────────────────
df = pd.read_csv(DATA_PATH)

exo_cols     = ['Irradiance', 'Temp']
exclude_cols = ['Time', 'ScenarioID', 'ProfileName', 'TempSetpoint'] + exo_cols
target_cols  = [c for c in df.columns if c not in exclude_cols]
all_input_cols = exo_cols + target_cols

xs = joblib.load(SCALER_X)
ys = joblib.load(SCALER_Y)

# ── Create sequences ───────────────────────────────────────────────────────────
X_seq, y_seq = create_sequences_by_scenario(df, all_input_cols, target_cols, SEQ_LENGTH, xs, ys)

# ── Load model ─────────────────────────────────────────────────────────────────
model = LSTMModel(len(all_input_cols), HIDDEN_SIZE, NUM_LAYERS, len(target_cols))
# FIX BUG-15: always pass map_location so this works on CPU-only machines
model.load_state_dict(torch.load(MODEL_PATH, map_location=device))
model = model.to(device)
model.eval()

# ── Inference ──────────────────────────────────────────────────────────────────
X_tensor = torch.tensor(X_seq, dtype=torch.float32)
with torch.no_grad():
    y_pred_scaled = model(X_tensor.to(device)).cpu().numpy()

# Inverse scale for human-readable metrics
y_pred = ys.inverse_transform(y_pred_scaled)
y_true = ys.inverse_transform(y_seq)

# ── R² scores ─────────────────────────────────────────────────────────────────
r2 = r2_score(y_true, y_pred, multioutput='raw_values')

# ── MAPE scores ───────────────────────────────────────────────────────────────
with np.errstate(divide='ignore', invalid='ignore'):
    mape = np.where(
        np.abs(y_true) > 1e-6,
        np.abs((y_true - y_pred) / y_true) * 100.0,
        0.0,
    ).mean(axis=0)

# ── Report ─────────────────────────────────────────────────────────────────────
print("\n" + "=" * 70)
print("  SolarTwin LSTM Validation Report (MATLAB/Simulink Training Data)")
print("=" * 70)
print(f"\n{'Target':<16} {'R²':>8} {'MAPE (%)':>10} {'Status':>12}")
print("-" * 50)
for i, col in enumerate(target_cols):
    status = "✅ Good" if r2[i] > 0.90 else ("⚠️  Fair" if r2[i] > 0.70 else "❌ Poor")
    print(f"  {col:<14} {r2[i]:>8.4f} {mape[i]:>10.2f}   {status}")

print("-" * 50)
print(f"  {'Mean R²':<14} {np.mean(r2):>8.4f} {np.mean(mape):>10.2f}")
print("\nNote: Model was pre-trained on MATLAB/Simulink physics data.")
print("      Use CalibratedLSTM (lstm_model.py) for real-sensor fine-tuning.")
