"""
SolarTwin — Shared LSTM Model Definition
Phase 3.3: Extracted from train_lstm.py, live_server.py, eval_r2.py to a single source of truth.
Phase 3.4: CalibratedLSTM — Physics-Informed Transfer Learning adapter for real sensor data.
"""
import torch
import torch.nn as nn


class LSTMModel(nn.Module):
    """
    Multi-layer LSTM regressor for predicting microgrid electrical quantities.
    Pre-trained on MATLAB/Simulink-generated 25-scenario dataset.
    """
    def __init__(self, input_size: int, hidden_size: int, num_layers: int, output_size: int):
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

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        out, _ = self.lstm(x)
        out = out[:, -1, :]          # take last time-step output
        out = self.fc1(out)
        out = self.relu(out)
        out = self.fc2(out)
        return out

    def get_features(self, x: torch.Tensor) -> torch.Tensor:
        """Extract penultimate-layer features (for calibration adapter input)."""
        out, _ = self.lstm(x)
        out = out[:, -1, :]
        out = self.fc1(out)
        out = self.relu(out)
        return out   # shape: (batch, hidden_size // 2)


class CalibratedLSTM(nn.Module):
    """
    Physics-Informed Transfer Learning adapter.

    Strategy:
    - Backbone: the MATLAB/Simulink pre-trained LSTMModel (FROZEN weights).
      The MATLAB training data is physics-consistent, noise-free, and validated —
      it provides an excellent prior for the microgrid dynamics.
    - Calibration head: a trainable MLP that adapts to real sensor measurements
      (noise, drift, sensor offsets) without disrupting the physics prior.

    Usage:
        base_model = LSTMModel(...)
        base_model.load_state_dict(torch.load('lstm_best_model.pth'))
        model = CalibratedLSTM(base_model, output_size=len(TARGET_COLS))
        # Fine-tune on ≥100 real sensor readings:
        optimizer = torch.optim.Adam(model.calibration_head.parameters(), lr=1e-4)
    """
    def __init__(self, base_model: LSTMModel, output_size: int):
        super(CalibratedLSTM, self).__init__()
        self.backbone = base_model

        # Freeze the MATLAB-trained backbone weights
        for param in self.backbone.parameters():
            param.requires_grad = False

        # Lightweight calibration head — only this trains on real data
        feature_size = base_model.hidden_size // 2
        self.calibration_head = nn.Sequential(
            nn.Linear(feature_size, feature_size),
            nn.ReLU(),
            nn.Dropout(0.1),
            nn.Linear(feature_size, output_size),
        )

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        with torch.no_grad():
            features = self.backbone.get_features(x)
        return self.calibration_head(features)

    def unfreeze_backbone(self, lr_scale: float = 0.01):
        """
        Optional: gradually unfreeze backbone for full fine-tuning.
        Use a very small lr_scale to preserve the physics prior.
        """
        for param in self.backbone.parameters():
            param.requires_grad = True
        return [
            {'params': self.backbone.parameters(), 'lr': lr_scale},
            {'params': self.calibration_head.parameters(), 'lr': 1.0},
        ]
