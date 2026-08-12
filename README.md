# ☀️ SolarTwin: Solar PV & UPQC Digital Twin Platform

> ⚠️ **Project Status: Under Active Development** 🚧  
> *This project is actively being developed, extended, and updated. Features, models, and UI components are continuously evolving.*

---

## 📌 Overview

**SolarTwin** is a high-fidelity Digital Twin platform combining physical MATLAB/Simulink simulation models of a Photovoltaic (PV) Array and Unified Power Quality Conditioner (UPQC) with deep learning surrogate models (PyTorch LSTM) and a modern real-time interactive web dashboard built with Next.js.

The platform enables real-time dynamic analysis, telemetry monitoring, parameter sweeps, and instant AI-driven dynamic surrogate inference for solar microgrids under various irradiance dips, ramps, and environmental operational scenarios.

---

## 🏗️ System Architecture

```
                                  +---------------------------------------+
                                  |     MATLAB / Simulink Physical Model  |
                                  |     (PV_Array_Model.slx / UPQC)       |
                                  +-------------------+-------------------+
                                                      |
                                                      v  (Scenario Sweeps & Datasets)
                                  +-------------------+-------------------+
                                  |  PyTorch LSTM Surrogate Model         |
                                  |  (train_lstm.py & Scalers)            |
                                  +-------------------+-------------------+
                                                      |
                                                      v  (Inference & Real-time Telemetry)
                                  +-------------------+-------------------+
                                  |  Next.js Interactive Dashboard        |
                                  |  (upqc-simulation-platform)           |
                                  +---------------------------------------+
```

---

## ✨ Key Features

- **⚡ Physical Simulink Engine**: High-fidelity MATLAB/Simulink model (`PV_Array_Model.slx`) running in Rapid Accelerator mode for fast simulation of solar arrays, power converters, and power quality parameters.
- **🤖 Deep Learning Surrogate (LSTM)**: 3-layer PyTorch LSTM model trained on 25 operational scenarios predicting 14+ power quality metrics (3-phase $V_{\text{rms}}$, $I_{\text{rms}}$, $THD_v$, $THD_i$, $P_{\text{ac}}$, $Q$, $V_{\text{dc}}$, $P_{\text{dc}}$, etc.).
- **📊 Next.js Web Dashboard**: Modern, responsive dashboard featuring:
  - **Live Telemetry & Gauges**: Real-time visualization of voltage, current, power, and harmonic distortion.
  - **Parameter Panel**: Interactive control for Irradiance profiles (`DeepDip_Recover`, `RampDown`, `RampUp`, `ShallowCloud`, `DoubleDip`) and temperature setpoints.
  - **Statistical Analytics & Charts**: Historical trends, scenario comparisons, and performance analytics.
  - **Model Explorer**: Inspect digital twin block structures and electrical signal pathways.
- **🔄 Scenario Generation Pipeline**: Automated scripts (`run_25_scenario_sweep.m`, `run_scenario_deployed.m`) generating structured CSV datasets for AI training and evaluation.

---

## 📁 Repository Structure

```
SolarTwin/
├── PV_Array_Model.slx              # Primary Simulink PV Array & Power Converter model
├── run_scenario_deployed.m         # MATLAB deployment script for rapid execution
├── run_25_scenario_sweep.m         # Automated scenario generation sweep script
├── train_lstm.py                   # PyTorch LSTM surrogate model training pipeline
├── x_scaler.pkl / y_scaler.pkl     # Feature scaling artifacts for AI inference
├── lstm_solar_model.h5             # Pre-trained deep learning surrogate model weights
├── scenario_csvs/                  # Generated operational dataset CSVs (25 scenarios)
└── upqc-simulation-platform/      # Next.js 15 Web Application Dashboard
    ├── app/                        # Next.js App Router pages (Dashboard, Live, Stats)
    ├── components/                 # React UI components (Telemetry Cards, Charts, Control Panels)
    └── lib/                        # Simulation types and API logic
```

---

## 🛠️ Technology Stack

- **Physics Simulation**: MATLAB, Simulink, Simulink Compiler, Rapid Accelerator
- **Machine Learning**: Python 3.x, PyTorch, Scikit-Learn, Pandas, Joblib
- **Frontend / Dashboard**: Next.js 15, React 19, TypeScript, Tailwind CSS, Recharts, Lucide Icons

---

## 🚀 Getting Started

### 1. Prerequisites

- **Python**: Version 3.9+
- **Node.js**: Version 18+ (npm included)
- **MATLAB**: R2022b or newer with Simulink and Simulink Compiler (optional for running raw Simulink sweeps)

### 2. Machine Learning Setup

```bash
# Clone the repository
git clone https://github.com/h4ck3r0/SolarTwin.git
cd SolarTwin

# Create and activate a Python virtual environment
python -m venv .venv
source .venv/bin/activate  # On Windows: .venv\Scripts\activate

# Install required dependencies
pip install torch pandas numpy scikit-learn joblib
```

To train or evaluate the LSTM surrogate model:
```bash
python train_lstm.py
```

### 3. Web Dashboard Setup

```bash
# Navigate to the frontend directory
cd upqc-simulation-platform

# Install dependencies
npm install

# Start the development server
npm run dev
```

Open [http://localhost:3000](http://localhost:3000) in your browser to view the interactive Digital Twin dashboard.

---

## 🔄 Ongoing Development & Roadmap

This project is **actively under development**. Current focus areas and upcoming enhancements include:

- [ ] **Real-Time WebSockets Integration**: Connecting Python/MATLAB live simulation runtime directly to the Next.js frontend via WebSocket stream.
- [ ] **Expanded Grid Distortion Profiles**: Adding sag, swell, voltage unbalance, and non-linear load disturbance profiles.
- [ ] **Reinforcement Learning Control**: Implementing RL-based control algorithms for active power filter optimization.
- [ ] **Docker Containerization**: Full Docker Compose setup for seamless deployment of physical engine, ML inference server, and web frontend.

---

## 📄 License

Distributed under the MIT License. See `LICENSE` for more details.
