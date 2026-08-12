import torch
import pandas as pd
import numpy as np
import joblib
from sklearn.metrics import r2_score
import sys
import os
from train_lstm import create_sequences_by_scenario
import torch.nn as nn

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

df = pd.read_csv('lstm_training_data_25scenarios.csv')

exo_cols = ['Irradiance', 'Temp']
exclude_cols = ['Time', 'ScenarioID', 'ProfileName', 'TempSetpoint'] + exo_cols
target_cols = [c for c in df.columns if c not in exclude_cols]
all_input_cols = exo_cols + target_cols

xs = joblib.load('x_scaler.pkl')
ys = joblib.load('y_scaler.pkl')

SEQ_LENGTH = 10
X_seq, y_seq = create_sequences_by_scenario(df, all_input_cols, target_cols, SEQ_LENGTH, xs, ys)

model = LSTMModel(len(all_input_cols), 256, 3, len(target_cols))
model.load_state_dict(torch.load('lstm_best_model.pth'))
model.eval()

with torch.no_grad():
    y_pred = model(torch.tensor(X_seq, dtype=torch.float32)).numpy()

r2 = r2_score(y_seq, y_pred, multioutput='raw_values')
print("R2 Scores per target (Autoregressive + Scenario Fixed):")
for i, col in enumerate(target_cols):
    print(f"{col}: {r2[i]:.4f}")
print(f"Mean R2: {np.mean(r2):.4f}")
