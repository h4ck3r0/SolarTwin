import { SimulationParameters, SimulationResult, SimulationDataPoint, SimulationService } from './simulation-types';

export class MockSimulationService implements SimulationService {
  async runSimulation(params: SimulationParameters): Promise<SimulationResult> {
    // Simulate API processing delay of 2.5 seconds
    await new Promise((resolve) => setTimeout(resolve, 2500));

    const dataPoints: SimulationDataPoint[] = [];
    const f = params.microgridFrequency || 60;
    const omega = 2 * Math.PI * f;
    
    // Nominal peak voltage line-to-neutral: V_rms_L-L * sqrt(2) / sqrt(3)
    const nominalVLL = 415; // standard base line-to-line RMS
    const nominalPeak = (nominalVLL * Math.sqrt(2)) / Math.sqrt(3);
    
    // User-defined grid voltage L-L RMS
    const gridPeak = (params.microgridVoltage * Math.sqrt(2)) / Math.sqrt(3);
    const sagFactor = gridPeak / nominalPeak; // ratio of actual grid voltage to nominal
    
    // Phase shift angle for inductive load current
    const phi = 0.45; // ~25 degrees lag (power factor ~0.9)
    
    const steps = 300;
    const duration = 0.3; // 300 ms total simulation time matching actual Simulink model (0.3s)
    const dt = duration / steps;
    
    for (let i = 0; i <= steps; i++) {
      const t = i * dt;
      
      // 1. Grid Voltages
      // Simulating a temporary grid voltage disturbance (sag or swell) between t = 0.10s and 0.20s
      let activeSagFactor = 1.0;
      const inDisturbanceWindow = t >= 0.10 && t <= 0.20;
      if (inDisturbanceWindow) {
        activeSagFactor = sagFactor;
      }
      
      const vGridPeakActive = nominalPeak * activeSagFactor;
      
      const gridVoltageA = vGridPeakActive * Math.sin(omega * t);
      const gridVoltageB = vGridPeakActive * Math.sin(omega * t - (2 * Math.PI) / 3);
      const gridVoltageC = vGridPeakActive * Math.sin(omega * t + (2 * Math.PI) / 3);
      
      // 2. Load Currents (Non-linear diode rectifier load causes harmonics)
      // Formulated with 5th and 7th harmonics (typical for 6-pulse rectifiers)
      const baseLoadCurrentPeak = 20; // Amps
      
      // Harmonic amplitudes
      const i5 = 4.0; // 5th harmonic peak
      const i7 = 2.0; // 7th harmonic peak
      
      const loadCurrentA =
        baseLoadCurrentPeak * Math.sin(omega * t - phi) +
        i5 * Math.sin(5 * omega * t + Math.PI / 4) +
        i7 * Math.sin(7 * omega * t - Math.PI / 6);
      
      const loadCurrentB =
        baseLoadCurrentPeak * Math.sin(omega * t - phi - (2 * Math.PI) / 3) +
        i5 * Math.sin(5 * (omega * t - (2 * Math.PI) / 3) + Math.PI / 4) +
        i7 * Math.sin(7 * (omega * t - (2 * Math.PI) / 3) - Math.PI / 6);
        
      const loadCurrentC =
        baseLoadCurrentPeak * Math.sin(omega * t - phi + (2 * Math.PI) / 3) +
        i5 * Math.sin(5 * (omega * t + (2 * Math.PI) / 3) + Math.PI / 4) +
        i7 * Math.sin(7 * (omega * t + (2 * Math.PI) / 3) - Math.PI / 6);

      // 3. Grid Currents (Load Current)
      // Ideally grid current is pure sinusoid, but without APF, harmonics flow directly
      const gridCurrentA = loadCurrentA;
      const gridCurrentB = loadCurrentB;
      const gridCurrentC = loadCurrentC;

      // 4. Supply (Load) Voltages (Grid Voltage)
      // Without UPQC, load voltage drops during microgrid sag
      const supplyVoltageA = gridVoltageA;
      const supplyVoltageB = gridVoltageB;
      const supplyVoltageC = gridVoltageC;

      // 7. Solar PV Array
      const strings = params.solarStringsParallel ?? 88;
      const modulesPerString = params.solarModulesSeries ?? 7;
      const panelRatingWatts = params.solarPanelWatts ?? 415;
      const totalPvCapacityWatts = strings * modulesPerString * panelRatingWatts;
      
      // Dynamic irradiance profile: 1000 -> 600 -> 200 -> 1000 W/m²
      let dynamicIrradiance = 1000;
      if (t >= 0.075 && t < 0.15) {
        dynamicIrradiance = 600;
      } else if (t >= 0.15 && t < 0.225) {
        dynamicIrradiance = 200;
      } else if (t >= 0.225) {
        dynamicIrradiance = 1000;
      }
      
      const irradianceFactor = Math.max(0, dynamicIrradiance / 1000);
      const solarPowerWatts = totalPvCapacityWatts * irradianceFactor;
      
      // Solar string voltage
      const vPvNominal = modulesPerString * 72.9; // 72.9 Vmp per panel
      const vPvActive = solarPowerWatts > 0 ? vPvNominal * Math.min(1.0, t / 0.01) : 0;
      const solarVoltageDc = Math.round(vPvActive * 10) / 10;
      
      // I_pv = P_pv / V_pv
      const solarCurrentDc = solarVoltageDc > 0 ? Math.round((solarPowerWatts / solarVoltageDc) * 100) / 100 : 0;

      // 8. DC Link Voltage
      // DC link voltage starts with a transient and settles to the target value.
      // Influenced by microgrid generation: Solar Irradiance, Wind Speed, Battery SOC
      const vDcTarget = params.dcLinkVoltage;
      const vDcInitial = 580; // starting pre-charged level
      
      // Solar and wind power contribution reduces the time constant of charging and raises the steady-state slightly
      const solarPowerFactor = (solarPowerWatts / 1000); // in kW (1.525 kW max)
      const windPowerFactor = params.windSpeed / 12; // 0 to 1.5
      const batteryFactor = params.batterySOC / 100; // 0 to 1
      
      const totalMgPower = solarPowerFactor + (windPowerFactor * 10) + (batteryFactor * 5); // kW
      
      // Time constant (tau) represents charging speed. More microgrid power = faster charging
      const tau = 0.015 / (1 + totalMgPower * 0.02); 
      
      // Exponential rise + 6-pulse ripple (300Hz for 50Hz grid) + noise
      const baseDc = vDcInitial + (vDcTarget - vDcInitial) * (1 - Math.exp(-t / tau));
      
      // Steady state ripple depends on non-linear load and capacitor sizing (mocked)
      // Higher solar/wind decreases ripple (stabilizes DC link)
      const rippleFactor = Math.max(0.2, 2.5 - totalMgPower * 0.05);
      const dcRipple = rippleFactor * Math.sin(6 * omega * t) + 0.3 * Math.random();
      
      const dcLinkVoltage = baseDc + (t > 0.005 ? dcRipple : 0);

      dataPoints.push({
        time: parseFloat(t.toFixed(5)),
        gridVoltageA: Math.round(gridVoltageA * 10) / 10,
        gridVoltageB: Math.round(gridVoltageB * 10) / 10,
        gridVoltageC: Math.round(gridVoltageC * 10) / 10,
        gridCurrentA: Math.round(gridCurrentA * 100) / 100,
        gridCurrentB: Math.round(gridCurrentB * 100) / 100,
        gridCurrentC: Math.round(gridCurrentC * 100) / 100,
        supplyVoltageA: Math.round(supplyVoltageA * 10) / 10,
        supplyVoltageB: Math.round(supplyVoltageB * 10) / 10,
        supplyVoltageC: Math.round(supplyVoltageC * 10) / 10,
        loadCurrentA: Math.round(loadCurrentA * 100) / 100,
        loadCurrentB: Math.round(loadCurrentB * 100) / 100,
        loadCurrentC: Math.round(loadCurrentC * 100) / 100,

        dcLinkVoltage: Math.round(dcLinkVoltage * 10) / 10,
        solarPowerWatts: Math.round(solarPowerWatts * 10) / 10,
        solarVoltageDc,
        solarCurrentDc,
        solarIrradiance: dynamicIrradiance,
      });
    }

    return {
      success: true,
      message: 'Simulation run completed successfully.',
      dataPoints,
    };
  }
}
