import { NextRequest, NextResponse } from 'next/server';
import { SimulationParameters } from '@/lib/simulation-types';

export async function POST(req: NextRequest) {
  try {
    const body = await req.json();
    const p = body.parameters ?? {};

    // BUG-C01 FIX: All fallbacks now match DEFAULT_PARAMETERS in page.tsx
    // so a fresh-browser user never gets the old crash-inducing values.
    const parameters: SimulationParameters = {
      // AC Bus
      microgridVoltage:    Number(p.microgridVoltage    ?? 415),
      microgridFrequency:  Number(p.microgridFrequency  ?? 50),

      // Solar PV Array — matched to DEFAULT_PARAMETERS (10 strings, not 88)
      solarIrradiance:       Number(p.solarIrradiance       ?? 1000),
      solarTemperature:      Number(p.solarTemperature       ?? 25),
      solarStringsParallel:  Number(p.solarStringsParallel   ?? 10),
      solarModulesSeries:    Number(p.solarModulesSeries     ?? 7),
      solarPanelWatts:       Number(p.solarPanelWatts        ?? 415),
      solarVmpp:             Number(p.solarVmpp              ?? 34.1),

      // Battery — matched to DEFAULT_PARAMETERS (500 kWh, not 100)
      batterySOC:          Number(p.batterySOC          ?? 80),
      batteryCapacityKwh:  Number(p.batteryCapacityKwh  ?? 500),

      // DC Link & UPQC — matched to DEFAULT_PARAMETERS (healthy PI gains)
      dcLinkVoltage:    Number(p.dcLinkVoltage    ?? 700),
      filterInductance: Number(p.filterInductance ?? 5.0),
      dcCapacitance:    Number(p.dcCapacitance    ?? 50000),
      kp:               Number(p.kp               ?? 5.0),
      ki:               Number(p.ki               ?? 50),

      // Grid — matched to DEFAULT_PARAMETERS (low impedance)
      isGridConnected:  p.isGridConnected  !== undefined ? Boolean(p.isGridConnected) : true,
      gridResistance:   Number(p.gridResistance   ?? 0.01),
      gridReactance:    Number(p.gridReactance    ?? 0.05),

      // Load — matched to DEFAULT_PARAMETERS
      loadActivePower:  Number(p.loadActivePower  ?? 10.0),
      loadPowerFactor:  Number(p.loadPowerFactor  ?? 0.85),
      loadHarmonicType: String(p.loadHarmonicType ?? 'Rectifier'),
      loadTHD:          Number(p.loadTHD          ?? 28),
      loadPowerProfile: String(p.loadPowerProfile ?? ''),

      // Wind
      windSpeed:        Number(p.windSpeed        ?? 8.0),
      windCutIn:        Number(p.windCutIn        ?? 3.0),
      windCutOut:       Number(p.windCutOut       ?? 25.0),
      windNominalPower: Number(p.windNominalPower ?? 50.0),

      simulationDuration: Number(p.simulationDuration ?? 0.05),
      irradianceProfile:  String(p.irradianceProfile  ?? '0:1000'),
      temperatureProfile: String(p.temperatureProfile ?? '0:25'),
      isTripped:          p.isTripped !== undefined ? Boolean(p.isTripped) : false,
      simulationMode:     (String(p.simulationMode ?? 'EMT')) as 'EMT' | 'EMS',
    };

    const topology = body.topology || { nodes: [], edges: [] };

    const response = await fetch('http://127.0.0.1:8001/simulate', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
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
      { success: false, message: error?.message || 'Internal Server Error.', dataPoints: [] },
      { status: 500 }
    );
  }
}
