#!/bin/bash

# Navigate to the root directory just in case
cd "$(dirname "$0")"

echo "====================================="
echo " Starting SolarTwin Platform "
echo "====================================="

# Start the Pandapower Python Backend
echo "[1/3] Starting Pandapower Python Backend (Port 8001)..."
source .venv/bin/activate
python python_backend/main.py &
BACKEND_PID=$!

# Start the LSTM Live Server
echo "[2/3] Starting LSTM Live Server (Port 8000)..."
python python_backend/live_server.py &
LSTM_PID=$!

# Start the Next.js Frontend
echo "[3/3] Starting Next.js Frontend (Port 3000)..."
cd upqc-simulation-platform
npm run dev &
FRONTEND_PID=$!

echo "====================================="
echo " All servers are now running! "
echo " Next.js Frontend: http://localhost:3000"
echo " Live Server:      http://localhost:8000"
echo " Python Backend:   http://localhost:8001"
echo " Press Ctrl+C to stop all servers."
echo "====================================="

# Trap termination signals to kill all processes when script is stopped
trap "echo -e '\nShutting down servers...'; kill $BACKEND_PID $LSTM_PID $FRONTEND_PID 2>/dev/null; exit" SIGINT SIGTERM EXIT

# Wait indefinitely until interrupted
wait
