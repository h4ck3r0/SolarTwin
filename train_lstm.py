import pandas as pd
import numpy as np
import torch
import torch.nn as nn
from torch.utils.data import TensorDataset, DataLoader
from sklearn.preprocessing import StandardScaler
import joblib
import time
import os

# --- Configurations ---
DATA_PATH = "lstm_training_data_25scenarios.csv"
SEQ_LENGTH = 10  # Reduced because each scenario is short (approx 73 steps)
BATCH_SIZE = 64
HIDDEN_SIZE = 256  # Increased for higher accuracy
NUM_LAYERS = 3     # Increased for higher accuracy
LEARNING_RATE = 0.001
EPOCHS = 300
PATIENCE = 30      # Early stopping patience

def create_sequences_by_scenario(df, input_cols, target_cols, seq_length, x_scaler, y_scaler):
    xs = []
    ys = []
    
    # Group by ScenarioID to avoid sequence boundary issues
    grouped = df.groupby('ScenarioID')
    
    for scenario_id, group in grouped:
        if len(group) <= seq_length:
            continue
            
        # Extract numpy arrays
        X_raw = group[input_cols].values
        y_raw = group[target_cols].values
        
        # Scale
        X_scaled = x_scaler.transform(X_raw)
        y_scaled = y_scaler.transform(y_raw)
        
        for i in range(len(X_scaled) - seq_length):
            xs.append(X_scaled[i:(i + seq_length)])
            ys.append(y_scaled[i + seq_length])
            
    return np.array(xs), np.array(ys)

def main():
    print("Loading data...")
    df = pd.read_csv(DATA_PATH)
    
    # Input features: Now including past targets as well as exogenous inputs
    exo_cols = ['Irradiance', 'Temp']
    exclude_cols = ['Time', 'ScenarioID', 'ProfileName', 'TempSetpoint'] + exo_cols
    target_cols = [c for c in df.columns if c not in exclude_cols]
    
    # All features to be used as inputs to the LSTM
    all_input_cols = exo_cols + target_cols
    
    print(f"Exogenous Inputs: {exo_cols}")
    print(f"Targets to predict ({len(target_cols)}): {target_cols}")
    print(f"Total LSTM Input Features: {len(all_input_cols)}")
    
    # Global scaling based on all data
    print("Fitting Scalers on full data...")
    X_scaler = StandardScaler()
    y_scaler = StandardScaler()
    
    X_scaler.fit(df[all_input_cols].values)
    y_scaler.fit(df[target_cols].values)
    
    # Save scalers for later inference
    joblib.dump(X_scaler, 'x_scaler.pkl')
    joblib.dump(y_scaler, 'y_scaler.pkl')
    print("Scalers saved to x_scaler.pkl and y_scaler.pkl")
    
    # Create sequences properly separated by Scenario
    print(f"Creating sequences of length {SEQ_LENGTH} avoiding scenario boundaries...")
    X_seq, y_seq = create_sequences_by_scenario(df, all_input_cols, target_cols, SEQ_LENGTH, X_scaler, y_scaler)
    
    # Train/Val split (80/20 chronological)
    split_idx = int(0.8 * len(X_seq))
    X_train, X_val = X_seq[:split_idx], X_seq[split_idx:]
    y_train, y_val = y_seq[:split_idx], y_seq[split_idx:]
    
    print(f"Train samples: {len(X_train)}, Val samples: {len(X_val)}")
    
    # Convert to PyTorch tensors
    X_train_t = torch.tensor(X_train, dtype=torch.float32)
    y_train_t = torch.tensor(y_train, dtype=torch.float32)
    X_val_t = torch.tensor(X_val, dtype=torch.float32)
    y_val_t = torch.tensor(y_val, dtype=torch.float32)
    
    # DataLoaders
    train_dataset = TensorDataset(X_train_t, y_train_t)
    val_dataset = TensorDataset(X_val_t, y_val_t)
    
    train_loader = DataLoader(train_dataset, batch_size=BATCH_SIZE, shuffle=True)
    val_loader = DataLoader(val_dataset, batch_size=BATCH_SIZE, shuffle=False)
    
    # Define Model
    class LSTMModel(nn.Module):
        def __init__(self, input_size, hidden_size, num_layers, output_size):
            super(LSTMModel, self).__init__()
            self.hidden_size = hidden_size
            self.num_layers = num_layers
            
            # batch_first=True means inputs are (batch, seq, feature)
            self.lstm = nn.LSTM(input_size, hidden_size, num_layers, batch_first=True, dropout=0.2 if num_layers > 1 else 0)
            self.fc1 = nn.Linear(hidden_size, hidden_size // 2)
            self.relu = nn.ReLU()
            self.fc2 = nn.Linear(hidden_size // 2, output_size)
            
        def forward(self, x):
            # h0 and c0 default to zeros if not provided
            out, _ = self.lstm(x)
            # Take the output of the last time step
            out = out[:, -1, :]
            out = self.fc1(out)
            out = self.relu(out)
            out = self.fc2(out)
            return out
            
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    print(f"Using device: {device}")
    
    model = LSTMModel(input_size=len(all_input_cols), 
                      hidden_size=HIDDEN_SIZE, 
                      num_layers=NUM_LAYERS, 
                      output_size=len(target_cols)).to(device)
                      
    criterion = nn.MSELoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=LEARNING_RATE, weight_decay=1e-5)
    scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(optimizer, mode='min', factor=0.5, patience=10)
    
    # Training Loop
    print("Starting training...")
    best_val_loss = float('inf')
    patience_counter = 0
    start_time = time.time()
    
    for epoch in range(EPOCHS):
        model.train()
        train_loss = 0.0
        for batch_X, batch_y in train_loader:
            batch_X, batch_y = batch_X.to(device), batch_y.to(device)
            
            optimizer.zero_grad()
            outputs = model(batch_X)
            loss = criterion(outputs, batch_y)
            loss.backward()
            optimizer.step()
            
            train_loss += loss.item() * batch_X.size(0)
            
        train_loss /= len(train_loader.dataset)
        
        model.eval()
        val_loss = 0.0
        with torch.no_grad():
            for batch_X, batch_y in val_loader:
                batch_X, batch_y = batch_X.to(device), batch_y.to(device)
                outputs = model(batch_X)
                loss = criterion(outputs, batch_y)
                val_loss += loss.item() * batch_X.size(0)
                
        val_loss /= len(val_loader.dataset)
        
        # Scheduler step
        scheduler.step(val_loss)
        
        if (epoch+1) % 10 == 0 or epoch == 0:
            print(f"Epoch [{epoch+1}/{EPOCHS}] - Train Loss: {train_loss:.6f} - Val Loss: {val_loss:.6f}")
            
        # Early stopping and model saving
        if val_loss < best_val_loss:
            best_val_loss = val_loss
            patience_counter = 0
            torch.save(model.state_dict(), 'lstm_best_model.pth')
        else:
            patience_counter += 1
            
        if patience_counter >= PATIENCE:
            print(f"Early stopping triggered at epoch {epoch+1}")
            break
            
    total_time = time.time() - start_time
    print(f"Training completed in {total_time:.2f} seconds.")
    print(f"Best Validation Loss: {best_val_loss:.6f}")
    
    # Quick sanity check on validation set
    print("Running quick validation check...")
    model.load_state_dict(torch.load('lstm_best_model.pth'))
    model.eval()
    
    with torch.no_grad():
        sample_X = X_val_t[:5].to(device)
        sample_y_true = y_val_t[:5].cpu().numpy()
        sample_y_pred = model(sample_X).cpu().numpy()
        
        sample_y_pred_inv = y_scaler.inverse_transform(sample_y_pred)
        sample_y_true_inv = y_scaler.inverse_transform(sample_y_true)
        
        print("\nSample Target 0 (True vs Pred):")
        for t, p in zip(sample_y_true_inv[:, 0], sample_y_pred_inv[:, 0]):
            print(f"  True: {t:.2f} | Pred: {p:.2f} | Error: {abs(t-p):.2f}")

if __name__ == '__main__':
    main()
