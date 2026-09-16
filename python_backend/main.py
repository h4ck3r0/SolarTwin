from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
import uvicorn
from power_solver import SimulationRequest, run_simulation

app = FastAPI(title="SolarTwin Pandapower Solver API")

# Allow CORS for Next.js frontend
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.post("/simulate")
async def simulate(req: SimulationRequest):
    try:
        data_points = run_simulation(req)
        return {
            "success": True,
            "message": "Pandapower simulation completed successfully.",
            "dataPoints": [dp.model_dump() for dp in data_points]
        }
    except Exception as e:
        return {
            "success": False,
            "message": f"Pandapower simulation failed: {str(e)}",
            "dataPoints": []
        }

if __name__ == "__main__":
    uvicorn.run("main:app", host="127.0.0.1", port=8001, reload=False)
