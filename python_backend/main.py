"""
SolarTwin — Pandapower EMT Solver API
Fixes: BUG-17 (model_dump vs dict), Phase 4.5 (non-blocking async executor).
"""
import asyncio
from concurrent.futures import ThreadPoolExecutor
import os
import google.generativeai as genai
from dotenv import load_dotenv
from pydantic import BaseModel

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
import uvicorn

from power_solver import SimulationRequest, run_simulation

load_dotenv()
api_key = os.getenv("GEMINI_API_KEY")
if api_key:
    genai.configure(api_key=api_key)

app = FastAPI(title="SolarTwin EMT Solver API")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# FIX Phase 4.2: Shared solver state — allows /status endpoint to report real progress
_solver_state: dict = {
    "running": False,
    "progress": 0.0,   # 0.0 – 1.0
    "stage": "idle",
    "last_duration_s": None,
}

# FIX Phase 4.5: thread pool so the CPU-bound solver doesn't block FastAPI's event loop
_executor = ThreadPoolExecutor(max_workers=4)


@app.post("/simulate")
async def simulate(req: SimulationRequest):
    import time
    _solver_state["running"] = True
    _solver_state["progress"] = 0.05
    _solver_state["stage"] = "Initialising solver…"
    t0 = time.monotonic()
    try:
        loop = asyncio.get_running_loop()
        _solver_state["progress"] = 0.15
        _solver_state["stage"] = "Building topology…"
        # run_simulation is pure-CPU NumPy — offload to thread pool
        data_points = await loop.run_in_executor(_executor, run_simulation, req)
        elapsed = time.monotonic() - t0
        _solver_state["running"] = False
        _solver_state["progress"] = 1.0
        _solver_state["stage"] = "complete"
        _solver_state["last_duration_s"] = round(elapsed, 3)
        return {
            "success": True,
            "message": "Simulation completed successfully.",
            # FIX BUG-17: use model_dump() — Pydantic v2 API (consistent with power_solver)
            "dataPoints": [dp.model_dump() for dp in data_points],
        }
    except Exception as e:
        import traceback
        traceback.print_exc()
        _solver_state["running"] = False
        _solver_state["progress"] = 0.0
        _solver_state["stage"] = "error"
        return {
            "success": False,
            "message": f"Simulation failed: {str(e)}",
            "dataPoints": [],
        }


@app.get("/status")
async def status():
    """FIX Phase 4.2: Return real solver progress so frontend can replace fake timer."""
    return _solver_state


@app.get("/health")
async def health():
    """Liveness probe for Docker / k8s."""
    return {"status": "ok"}


class AgentRequest(BaseModel):
    state: dict

@app.post("/api/agent/analyze")
async def analyze_state(req: AgentRequest):
    try:
        model = genai.GenerativeModel("gemini-1.5-flash")
        prompt = f"You are an expert microgrid operator. Analyze the following microgrid state and provide a short, actionable recommendation (max 3 sentences). Here is the telemetry state: {req.state}"
        response = model.generate_content(prompt)
        return {"success": True, "advice": response.text}
    except Exception as e:
        return {"success": False, "advice": f"Error communicating with AI: {str(e)}"}


if __name__ == "__main__":
    uvicorn.run("main:app", host="127.0.0.1", port=8001, reload=True)
