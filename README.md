# ☀️ SolarTwin: Interactive Microgrid & UPQC Digital Twin

> ⚠️ **Project Status: Under Active Development** 🚧  
> *This project is actively being developed, extended, and updated. Features, models, and UI components are continuously evolving.*

---

## 📌 Overview

**SolarTwin** is a high-fidelity Digital Twin platform for modeling, analyzing, and optimizing hybrid renewable energy microgrids. It features an interactive drag-and-drop web interface, an ultra-fast custom Python electromagnetic transient (EMT) solver, and deep learning surrogate models.

The platform enables real-time dynamic analysis, telemetry monitoring, interactive parameter sweeps, and instant physics-informed visualization for microgrids integrating **Solar PV**, **Wind Turbines**, **Battery Energy Storage Systems (BESS)**, and a **Unified Power Quality Conditioner (UPQC)**.

---

## 🏗️ System Architecture

```
                                  +---------------------------------------+
                                  |     Next.js Interactive Dashboard     |
                                  |     (React Flow Canvas & UI)          |
                                  +-------------------+-------------------+
                                                      | (REST / JSON)
                                                      v 
                                  +-------------------+-------------------+
                                  |  Python FastAPI Backend Server        |
                                  |  (run_all.py Orchestration)           |
                                  +---------+-------------------+---------+
                                            |                   |
                                            v                   v
                     +----------------------+------+  +---------+-----------+
                     | Custom EMT Physics Solver   |  | PyTorch LSTM Model  |
                     | (power_solver.py - RK4)     |  | (lstm_model.py)     |
                     +-----------------------------+  +---------------------+
```

---

## ✨ Key Features

- **🌐 Interactive Microgrid Canvas**: Drag-and-drop React Flow interface to build topologies on the fly. Connect Solar, Wind, Battery, Grid, and UPQC nodes.
- **⚡ Custom EMT Physics Engine**: High-fidelity Python solver (`power_solver.py`) utilizing **Runge-Kutta (RK4)** numerical integration for sub-millisecond electrical transients, MPPT tracking, and dynamic PI-controller loops.
- **🔋 Battery & Energy Storage**: Real-time integration of battery capacity and State of Charge (SOC) physics. 
- **🤖 Deep Learning Surrogate (LSTM)**: 3-layer PyTorch LSTM model trained on high-fidelity simulation sweeps (MATLAB/Simulink ground truth) predicting system stability and power quality metrics.
- **🌤️ Live Weather Sync**: Integrates with live meteorological APIs to automatically populate real-world irradiance, temperature, and wind speed data into the digital twin.
- **📊 Diagnostic Dashboard**: Recharts-powered oscilloscope-style charts rendering high-resolution telemetry of 3-phase voltages, currents, harmonics, and DC-link stability, complete with full-screen expandable views.

---

## 📁 Repository Structure

```
SolarTwin/
├── python_backend/
│   ├── main.py                     # FastAPI server handling simulation requests
│   ├── power_solver.py             # Core EMT physics engine (RK4 integration)
│   └── lstm_model.py               # PyTorch model definitions
├── upqc-simulation-platform/       # Next.js 15 Web Application Dashboard
│   ├── app/                        # App Router (Workspace, Live, Statistics)
│   ├── components/                 # React UI components (Canvas, Parameter Panel, Charts)
│   └── lib/                        # API types and utility functions
├── run_all.py                      # Orchestration script (starts UI + Backend)
├── train_lstm.py                   # PyTorch LSTM surrogate training script
├── lstm_training_data_25scenarios.csv # Training data
└── MATLAB/                         # Original Simulink ground-truth sweeps
```

---

## 🛠️ Technology Stack

- **Physics Simulation**: Custom Python EMT Solver (Numpy, SciPy), Runge-Kutta (RK4)
- **Backend**: Python 3.10+, FastAPI, Uvicorn
- **Machine Learning**: PyTorch, Scikit-Learn, Pandas, Joblib
- **Frontend / Dashboard**: Next.js 15, React 19, TypeScript, Tailwind CSS, React Flow, Recharts, Lucide Icons

---

## 🚀 Getting Started

### 1. Prerequisites

- **Python**: Version 3.9+
- **Node.js**: Version 18+ (npm included)

### 2. Setup & Installation

```bash
# Clone the repository
git clone https://github.com/h4ck3r0/SolarTwin.git
cd SolarTwin

# Install Python backend dependencies
pip install fastapi uvicorn torch pandas numpy scikit-learn joblib

# Install Frontend dependencies
cd upqc-simulation-platform
npm install
cd ..
```

### 3. Running the Platform

To launch both the Python backend simulation server and the Next.js frontend simultaneously, run the orchestration script from the root directory:

```bash
python run_all.py
```

The script will automatically start:
1. The **FastAPI Backend** on `http://localhost:8000`
2. The **Next.js Dashboard** on `http://localhost:3000`

Open [http://localhost:3000](http://localhost:3000) in your browser to begin building your microgrid on the canvas.

---

## 📄 License

Distributed under the MIT License. See `LICENSE` for more details.
