# UPQC Power Quality Simulation Platform

This is a production-style, high-fidelity engineering web workspace for hosting, configuring, and analyzing three-phase power quality simulations. Specifically, it simulates a **Unified Power Quality Conditioner (UPQC)** connected to a microgrid containing solar, wind, and battery assets, mitigating grid voltage disturbances (sags/swells) and non-linear load current harmonics.

The dashboard UI represents a compiled MATLAB/Simulink workspace. Calculations are abstracted via a `SimulationService` backend interface, currently running a high-fidelity physical simulation mock that is ready to be swapped with a live `MatlabSimulationService` calling MATLAB Runtime.

## Key Features

- **Industrial Engineering Workspace**: Recreates the aesthetics of specialized tools (MATLAB Simulink, ETAP, PSCAD, etc.).
- **Interactive React Flow Diagram**: A full-fledged electrical schematic showing nodes (Grid, Transformer, Series APF, Shunt APF, DC Link, Microgrid, Diode Rectifier, Critical Load, Scopes).
  - Highlight-to-select support.
  - Interactive panning and zooming, plus zoom sync from the tree view and toolbar.
  - Active signal flow animations indicating dynamic electrical current/voltage flow during solver runs.
- **Model Explorer**: A collapsible hierarchical tree of the UPQC model blocks. Selecting elements focuses the canvas camera and highlights the block.
- **Dynamic Parameter Panel**: Form inputs (Solar Irradiance, Wind Speed, Battery SOC, DC Link Voltage, Grid Voltage, Kp/Ki gains) that change based on the active block.
- **Advanced Scope Results Drawer**: Tabbed Recharts plotting Phase A, Phase B, and Phase C signals (or DC values) with:
  - Time-series tooltips.
  - CSV export options.
  - Maximized full-screen modal zoom for high-resolution analysis.
- **Floating Solver Status Console**: Real-time progress tracker outputting compiler and solver status.

---

## Technical Stack

- **Core**: Next.js (App Router), React, TypeScript
- **Styling**: Tailwind CSS
- **Interactive Diagram**: React Flow
- **Plotting**: Recharts
- **Icons**: Lucide React
- **Deployment**: Docker & Docker Compose

---

## Project Structure

```text
upqc-simulation-platform/
├── app/
│   ├── api/
│   │   └── simulation/
│   │       └── run/
│   │           └── route.ts       # Solver POST endpoint
│   ├── globals.css                # Global CSS / tailwind configuration
│   ├── layout.tsx                 # Root layout configuration
│   └── page.tsx                   # Central workspace view orchestrator
├── components/
│   ├── ModelExplorer.tsx          # Collapsible tree navigator
│   ├── ParameterPanel.tsx         # Dynamic input form for active blocks
│   ├── SimulationCanvas.tsx       # React Flow electrical schematic
│   ├── SimulationResults.tsx      # Recharts scope plotter & exporter
│   ├── SimulationStatus.tsx       # Floating progress console
│   └── SimulationToolbar.tsx      # Global toolbar controls
├── lib/
│   ├── mock-simulation.ts         # High-fidelity electrical waveform generator
│   ├── model-definition.ts        # React Flow schema node layout
│   └── simulation-types.ts        # TypeScript interface types
├── Dockerfile                     # Two-stage Node deployment dockerfile
├── docker-compose.yml             # Container orchestration config
└── README.md                      # Documentation
```

---

## Setup & Running Instructions

Ensure you have [Docker](https://www.docker.com/) and [Docker Compose](https://docs.docker.com/compose/) installed.

### Option 1: Docker (Recommended)

To build and run the application container:

```bash
docker compose up --build
```

Once started, the application will be accessible at:
[http://localhost:3000](http://localhost:3000)

### Option 2: Local Development

To run locally in development mode:

1. Navigate to the project folder:
   ```bash
   cd upqc-simulation-platform
   ```
2. Install npm dependencies:
   ```bash
   npm install
   ```
3. Run the development server:
   ```bash
   npm run dev
   ```
4. Access the app at [http://localhost:3000](http://localhost:3000).
5. Compile/build validation:
   ```bash
   npm run build
   ```
