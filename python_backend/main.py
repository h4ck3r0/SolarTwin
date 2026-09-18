"""
SolarTwin — Pandapower EMT Solver API
Fixes: BUG-17 (model_dump vs dict), Phase 4.5 (non-blocking async executor).
"""
import asyncio
from concurrent.futures import ThreadPoolExecutor

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
import uvicorn

from power_solver import SimulationRequest, run_simulation

app = FastAPI(title="SolarTwin EMT Solver API")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# FIX Phase 4.5: thread pool so the CPU-bound solver doesn't block FastAPI's event loop
_executor = ThreadPoolExecutor(max_workers=4)


@app.post("/simulate")
async def simulate(req: SimulationRequest):
    try:
        loop = asyncio.get_event_loop()
        # run_simulation is pure-CPU NumPy — offload to thread pool
        data_points = await loop.run_in_executor(_executor, run_simulation, req)
        return {
            "success": True,
            "message": "Simulation completed successfully.",
            # FIX BUG-17: use model_dump() — Pydantic v2 API (consistent with power_solver)
            "dataPoints": [dp.model_dump() for dp in data_points],
        }
    except Exception as e:
        import traceback
        traceback.print_exc()
        return {
            "success": False,
            "message": f"Simulation failed: {str(e)}",
            "dataPoints": [],
        }


if __name__ == "__main__":
    uvicorn.run("main:app", host="127.0.0.1", port=8001, reload=True)
