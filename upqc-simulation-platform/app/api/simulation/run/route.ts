import { NextRequest, NextResponse } from 'next/server';
import { SimulationParameters } from '@/lib/simulation-types';

export async function POST(req: NextRequest) {
  try {
    const body = await req.json();

    const parameters: SimulationParameters = {
      microgridVoltage: Number(body.parameters?.microgridVoltage ?? 415),
      microgridFrequency: Number(body.parameters?.microgridFrequency ?? 60),
      solarIrradiance: Number(body.parameters?.solarIrradiance ?? 1000),
      solarStringsParallel: Number(body.parameters?.solarStringsParallel ?? 88),
      solarModulesSeries: Number(body.parameters?.solarModulesSeries ?? 7),
      solarPanelWatts: Number(body.parameters?.solarPanelWatts ?? 415),
      batterySOC: Number(body.parameters?.batterySOC ?? 80),
      dcLinkVoltage: Number(body.parameters?.dcLinkVoltage ?? 700),
      gridResistance: Number(body.parameters?.gridResistance ?? 0.1),
      gridReactance: Number(body.parameters?.gridReactance ?? 0.2),
      loadActivePower: Number(body.parameters?.loadActivePower ?? 15.0),
      loadPowerFactor: Number(body.parameters?.loadPowerFactor ?? 0.85),
      filterInductance: Number(body.parameters?.filterInductance ?? 2.5),
      dcCapacitance: Number(body.parameters?.dcCapacitance ?? 2200),
      loadTHD: Number(body.parameters?.loadTHD ?? 28),
      kp: Number(body.parameters?.kp ?? 0.5),
      ki: Number(body.parameters?.ki ?? 10),
      isGridConnected: body.parameters?.isGridConnected ?? true,
      simulationDuration: Number(body.parameters?.simulationDuration ?? 0.3),
      irradianceProfile: body.parameters?.irradianceProfile ?? '0:1000',
    };
    const topology = body.topology || { nodes: [], edges: [] };

    // Call the Pandapower Python backend
    const response = await fetch('http://127.0.0.1:8001/simulate', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ parameters, topology }),
    });

    if (!response.ok) {
      throw new Error(`Python backend returned ${response.status}: ${await response.text()}`);
    }

    const result = await response.json();

    return NextResponse.json(result);
  } catch (error: any) {
    console.error('Simulation error:', error);
    return NextResponse.json(
      {
        success: false,
        message: error?.message || 'Internal Server Error during simulation run.',
        dataPoints: [],
      },
      { status: 500 }
    );
  }
}
