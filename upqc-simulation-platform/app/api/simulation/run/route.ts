import { NextRequest, NextResponse } from 'next/server';
import { MockSimulationService } from '@/lib/mock-simulation';
import { SimulationParameters } from '@/lib/simulation-types';

export async function POST(req: NextRequest) {
  try {
    const body = await req.json();

    // Map parameters from request body with defaults if not provided
    const parameters: SimulationParameters = {
      gridVoltage: Number(body.gridVoltage ?? 415),
      gridFrequency: Number(body.gridFrequency ?? 50),
      gridRs: Number(body.gridRs ?? 0.1),
      gridLs: Number(body.gridLs ?? 0.001),
      solarIrradiance: Number(body.solarIrradiance ?? 1000),
      solarPanelCount: Number(body.solarPanelCount ?? 5),
      solarPanelWatts: Number(body.solarPanelWatts ?? 305),
      windSpeed: Number(body.windSpeed ?? 12),
      batterySOC: Number(body.batterySOC ?? 80),
      dcLinkVoltage: Number(body.dcLinkVoltage ?? 700),
      kpSeries: Number(body.kpSeries ?? 1.5),
      kiSeries: Number(body.kiSeries ?? 120),
      refVoltSeries: Number(body.refVoltSeries ?? 415),
      kpShunt: Number(body.kpShunt ?? 1.0),
      kiShunt: Number(body.kiShunt ?? 85),
      refVoltShunt: Number(body.refVoltShunt ?? 700),
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
