#!/bin/bash
# SolarTwin — Full Platform Startup Script
# Starts: Python EMT solver (8001) + LSTM live server (8000) + Next.js frontend (3000)
#
# FIX Phase 4.8: SITE_LAT / SITE_LON are now environment variables (no longer hardcoded
#                in live_server.py). Override below for your installation site.

cd "$(dirname "$0")"

# ── Site Configuration (override for your location) ──────────────────────────
export SITE_LAT="${SITE_LAT:-12.9007}"   # RNSIT College, Bangalore (default)
export SITE_LON="${SITE_LON:-77.5174}"

echo "====================================================="
echo "   SolarTwin Digital Twin Platform"
echo "====================================================="
echo "   Site: lat=${SITE_LAT}, lon=${SITE_LON}"
echo "====================================================="

# Activate virtual environment
source .venv/bin/activate

# [1/3] Python EMT Solver backend (port 8001)
echo ""
echo "[1/3] Starting EMT Solver Backend (port 8001)..."
cd python_backend
python main.py &
BACKEND_PID=$!
cd ..

# [2/3] LSTM Live Server (port 8000)
echo "[2/3] Starting LSTM Live Server  (port 8000)..."
python python_backend/live_server.py &
LSTM_PID=$!

# Give Python servers a moment to start before launching frontend
sleep 2

# [3/3] Next.js Frontend (port 3000)
echo "[3/3] Starting Next.js Frontend  (port 3000)..."
cd upqc-simulation-platform
npm run dev &
FRONTEND_PID=$!
cd ..

echo ""
echo "====================================================="
echo "   All servers started!"
echo ""
echo "   Frontend   → http://localhost:3000"
echo "   Live LSTM  → http://localhost:8000/api/live_data"
echo "   EMT Solver → http://localhost:8001/docs"
echo ""
echo "   Press Ctrl+C to stop all servers."
echo "====================================================="

# Trap signals: cleanly shut down all processes on Ctrl+C / SIGTERM
trap "echo -e '\nShutting down...'; kill \$BACKEND_PID \$LSTM_PID \$FRONTEND_PID 2>/dev/null; exit 0" SIGINT SIGTERM EXIT

wait
