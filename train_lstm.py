"""
SolarTwin — LSTM Training Script
Fixes: BUG-14 (scenario-level train/val split to prevent look-back leakage).
Phase 3.3: Uses shared LSTMModel from lstm_model.py.

Training data source: MATLAB/Simulink 25-scenario sweep (run_25_scenario_sweep.m).
The Simulink model provides physics-consistent, noise-free ground truth — ideal for
pre-training a physics-informed LSTM prior.
"""
import os
import sys
import time

# train_lstm.py lives in the repo root; lstm_model.py is in python_backend/
_DIR  = os.path.dirname(os.path.abspath(__file__))   # = repo root
sys.path.insert(0, os.path.join(_DIR, 'python_backend'))

import joblib
import numpy as np
import pandas as pd
import torch
import torch.nn as nn
from sklearn.preprocessing import StandardScaler
from torch.utils.data import DataLoader, TensorDataset

from lstm_model import LSTMModel

# ── Paths ──────────────────────────────────────────────────────────────────────
DATA_PATH = os.path.join(_DIR, "lstm_training_data_25scenarios.csv")

# ── Hyperparameters ────────────────────────────────────────────────────────────
SEQ_LENGTH    = 10      # Reduced: each scenario ≈ 73 steps
BATCH_SIZE    = 64
HIDDEN_SIZE   = 256
NUM_LAYERS    = 3
LEARNING_RATE = 0.001
EPOCHS        = 300
PATIENCE      = 30      # Early stopping patience


def create_sequences_by_scenario(df, input_cols, target_cols, seq_length, x_scaler, y_scaler):
    """
    Create LSTM input sequences without crossing scenario boundaries.
    Avoids temporal leakage between different simulation scenarios.
    """
    xs, ys = [], []
    for scenario_id, group in df.groupby('ScenarioID'):
        if len(group) <= seq_length:
            continue
        X_raw = group[input_cols].values
        y_raw = group[target_cols].values
        X_scaled = x_scaler.transform(X_raw)
        y_scaled = y_scaler.transform(y_raw)
        for i in range(len(X_scaled) - seq_length):
            xs.append(X_scaled[i:(i + seq_length)])
            ys.append(y_scaled[i + seq_length])
    return np.array(xs), np.array(ys)


def main():
    print("=" * 60)
    print("  SolarTwin LSTM Training (MATLAB/Simulink Dataset)")
    print("=" * 60)
    print(f"Loading data from: {DATA_PATH}")
    df = pd.read_csv(DATA_PATH)

    exo_cols     = ['Irradiance', 'Temp']
    exclude_cols = ['Time', 'ScenarioID', 'ProfileName', 'TempSetpoint'] + exo_cols
    target_cols  = [c for c in df.columns if c not in exclude_cols]
    all_input_cols = exo_cols + target_cols

    print(f"\nExogenous inputs:  {exo_cols}")
    print(f"Targets ({len(target_cols)}):    {target_cols}")
    print(f"Total LSTM inputs: {len(all_input_cols)}")

    # ── Fit scalers on full dataset ──────────────────────────────────────────
    print("\nFitting scalers on full dataset...")
    X_scaler = StandardScaler()
    y_scaler = StandardScaler()
    X_scaler.fit(df[all_input_cols].values)
    y_scaler.fit(df[target_cols].values)

    scaler_x_path = os.path.join(_DIR, 'x_scaler.pkl')
    scaler_y_path = os.path.join(_DIR, 'y_scaler.pkl')
    joblib.dump(X_scaler, scaler_x_path)
    joblib.dump(y_scaler, scaler_y_path)
    print(f"Scalers saved → {scaler_x_path}, {scaler_y_path}")

    # ── FIX BUG-14: Scenario-level train/val split ───────────────────────────
    # Split at the SCENARIO level, not the sequence level.
    # This prevents any look-back window from the validation set referencing
    # training scenario data, which would inflate R² scores artificially.
    scenario_ids = df['ScenarioID'].unique()
    np.random.seed(42)
    np.random.shuffle(scenario_ids)
    n_train = int(0.8 * len(scenario_ids))
    train_scenario_ids = set(scenario_ids[:n_train])
    val_scenario_ids   = set(scenario_ids[n_train:])

    print(f"\nScenario-level split:")
    print(f"  Train scenarios: {sorted(train_scenario_ids)}")
    print(f"  Val   scenarios: {sorted(val_scenario_ids)}")

    df_train = df[df['ScenarioID'].isin(train_scenario_ids)]
    df_val   = df[df['ScenarioID'].isin(val_scenario_ids)]

    print(f"\nCreating sequences (seq_length={SEQ_LENGTH})...")
    X_train, y_train = create_sequences_by_scenario(
        df_train, all_input_cols, target_cols, SEQ_LENGTH, X_scaler, y_scaler
    )
    X_val, y_val = create_sequences_by_scenario(
        df_val, all_input_cols, target_cols, SEQ_LENGTH, X_scaler, y_scaler
    )
    print(f"  Train sequences: {len(X_train):,}  |  Val sequences: {len(X_val):,}")

    # ── PyTorch Datasets ─────────────────────────────────────────────────────
    X_train_t = torch.tensor(X_train, dtype=torch.float32)
    y_train_t = torch.tensor(y_train, dtype=torch.float32)
    X_val_t   = torch.tensor(X_val,   dtype=torch.float32)
    y_val_t   = torch.tensor(y_val,   dtype=torch.float32)

    train_loader = DataLoader(TensorDataset(X_train_t, y_train_t),
                              batch_size=BATCH_SIZE, shuffle=True)
    val_loader   = DataLoader(TensorDataset(X_val_t, y_val_t),
                              batch_size=BATCH_SIZE, shuffle=False)

    # ── Model, optimizer, scheduler ─────────────────────────────────────────
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    print(f"\nDevice: {device}")

    model = LSTMModel(
        input_size=len(all_input_cols),
        hidden_size=HIDDEN_SIZE,
        num_layers=NUM_LAYERS,
        output_size=len(target_cols),
    ).to(device)

    criterion = nn.MSELoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=LEARNING_RATE, weight_decay=1e-5)
    scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(
        optimizer, mode='min', factor=0.5, patience=10
    )

    # ── Training loop ─────────────────────────────────────────────────────────
    print("\nStarting training...")
    best_val_loss = float('inf')
    patience_counter = 0
    start_time = time.time()
    model_path = os.path.join(_DIR, 'lstm_best_model.pth')

    for epoch in range(EPOCHS):
        model.train()
        train_loss = 0.0
        for batch_X, batch_y in train_loader:
            batch_X, batch_y = batch_X.to(device), batch_y.to(device)
            optimizer.zero_grad()
            loss = criterion(model(batch_X), batch_y)
            loss.backward()
            optimizer.step()
            train_loss += loss.item() * batch_X.size(0)
        train_loss /= len(train_loader.dataset)

        model.eval()
        val_loss = 0.0
        with torch.no_grad():
            for batch_X, batch_y in val_loader:
                batch_X, batch_y = batch_X.to(device), batch_y.to(device)
                val_loss += criterion(model(batch_X), batch_y).item() * batch_X.size(0)
        val_loss /= len(val_loader.dataset)

        scheduler.step(val_loss)

        if (epoch + 1) % 10 == 0 or epoch == 0:
            print(f"  Epoch [{epoch+1:3d}/{EPOCHS}] "
                  f"Train: {train_loss:.6f}  Val: {val_loss:.6f}  "
                  f"LR: {optimizer.param_groups[0]['lr']:.2e}")

        if val_loss < best_val_loss:
            best_val_loss = val_loss
            patience_counter = 0
            torch.save(model.state_dict(), model_path)
        else:
            patience_counter += 1

        if patience_counter >= PATIENCE:
            print(f"\n  Early stopping at epoch {epoch+1}")
            break

    elapsed = time.time() - start_time
    print(f"\nTraining complete in {elapsed:.1f}s  |  Best val loss: {best_val_loss:.6f}")
    print(f"Model saved → {model_path}")

    # ── Quick sanity check ────────────────────────────────────────────────────
    print("\nQuick validation sample (first 5 val sequences):")
    model.load_state_dict(torch.load(model_path, map_location=device))
    model.eval()
    with torch.no_grad():
        sample_X  = X_val_t[:5].to(device)
        sample_y_true = y_val_t[:5].cpu().numpy()
        sample_y_pred = model(sample_X).cpu().numpy()
        true_inv = y_scaler.inverse_transform(sample_y_true)
        pred_inv = y_scaler.inverse_transform(sample_y_pred)

    print(f"\n{'Target':<14} {'True':>12} {'Pred':>12} {'Error':>10}")
    print("-" * 52)
    for t, p in zip(true_inv[:, 0], pred_inv[:, 0]):
        print(f"  {target_cols[0]:<12} {t:>12.2f} {p:>12.2f} {abs(t-p):>10.2f}")


if __name__ == '__main__':
    main()
