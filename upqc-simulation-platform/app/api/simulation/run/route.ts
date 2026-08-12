import { NextRequest, NextResponse } from 'next/server';
import { MockSimulationService } from '@/lib/mock-simulation';
import { SimulationParameters } from '@/lib/simulation-types';

export async function POST(req: NextRequest) {
  try {
    const body = await req.json();

    // Map parameters from request body with defaults if not provided
    const parameters: SimulationParameters = {
      microgridVoltage: Number(body.microgridVoltage ?? 415),
      microgridFrequency: Number(body.microgridFrequency ?? 60),
      solarIrradiance: Number(body.solarIrradiance ?? 1000),
      solarStringsParallel: Number(body.solarStringsParallel ?? 88),
      solarModulesSeries: Number(body.solarModulesSeries ?? 7),
      solarPanelWatts: Number(body.solarPanelWatts ?? 415),
      windSpeed: Number(body.windSpeed ?? 12),
      batterySOC: Number(body.batterySOC ?? 80),
      dcLinkVoltage: Number(body.dcLinkVoltage ?? 700),
    };

    const simulationService = new MockSimulationService();
    const result = await simulationService.runSimulation(parameters);

    return NextResponse.json(result);
  } catch (error: any) {
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
