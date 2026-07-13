import { SimulationParameters, SimulationResult, SimulationDataPoint, SimulationService } from './simulation-types';

export class MockSimulationService implements SimulationService {
  async runSimulation(params: SimulationParameters): Promise<SimulationResult> {
    // Simulate API processing delay of 2.5 seconds
    await new Promise((resolve) => setTimeout(resolve, 2500));

    const dataPoints: SimulationDataPoint[] = [];
    const f = params.gridFrequency || 50;
    const omega = 2 * Math.PI * f;
    
    // Nominal peak voltage line-to-neutral: V_rms_L-L * sqrt(2) / sqrt(3)
    const nominalVLL = 415; // standard base line-to-line RMS
    const nominalPeak = (nominalVLL * Math.sqrt(2)) / Math.sqrt(3);
    
    // User-defined grid voltage L-L RMS
    const gridPeak = (params.gridVoltage * Math.sqrt(2)) / Math.sqrt(3);
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

      // 3. Shunt APF Injecting Currents
      // The Shunt APF injects currents to cancel the harmonics and supply active current for DC link
      // Kp and Ki parameters adjust the settling transient of compensation
      const settlingFactor = Math.min(1.0, t / 0.015); // transient settling in first 15ms
      const controllerGain = Math.max(0.2, Math.min(1.5, params.kpShunt / 1.0)); // affect effectiveness
      
      // Compensation of 5th & 7th harmonics
      let injectingCurrentA = -(
        i5 * Math.sin(5 * omega * t + Math.PI / 4) +
        i7 * Math.sin(7 * omega * t - Math.PI / 6)
      ) * settlingFactor * controllerGain;
      
      let injectingCurrentB = -(
        i5 * Math.sin(5 * (omega * t - (2 * Math.PI) / 3) + Math.PI / 4) +
        i7 * Math.sin(7 * (omega * t - (2 * Math.PI) / 3) - Math.PI / 6)
      ) * settlingFactor * controllerGain;
      
      let injectingCurrentC = -(
        i5 * Math.sin(5 * (omega * t + (2 * Math.PI) / 3) + Math.PI / 4) +
        i7 * Math.sin(7 * (omega * t + (2 * Math.PI) / 3) - Math.PI / 6)
      ) * settlingFactor * controllerGain;
      
      // Add high frequency switching ripple (~20kHz modeled as small noise)
      const rippleAmpCurrent = 0.35;
      injectingCurrentA += rippleAmpCurrent * Math.sin(400 * omega * t);
      injectingCurrentB += rippleAmpCurrent * Math.sin(400 * omega * t + Math.PI/3);
      injectingCurrentC += rippleAmpCurrent * Math.sin(400 * omega * t - Math.PI/3);

      // 4. Grid Currents (Load Current + Shunt Injecting Current)
      // Ideally grid current is pure sinusoid
      const gridCurrentA = loadCurrentA + injectingCurrentA;
      const gridCurrentB = loadCurrentB + injectingCurrentB;
      const gridCurrentC = loadCurrentC + injectingCurrentC;

      // 5. Series APF Injecting Voltages
      // Injects voltage in series to compensate for grid sag/swell and maintain nominal load voltage
      const seriesSettling = inDisturbanceWindow ? Math.min(1.0, (t - 0.10) / 0.005) : 0; // 5ms response time
      const seriesDecay = t > 0.20 ? Math.max(0, 1 - (t - 0.20) / 0.005) : 1;
      const seriesActive = inDisturbanceWindow ? seriesSettling : seriesDecay;
      
      const compGain = Math.max(0.5, Math.min(1.2, params.kpSeries / 1.5));
      const targetInjectedPeak = nominalPeak * (1 - activeSagFactor) * compGain;
      
      let injectingVoltageA = targetInjectedPeak * Math.sin(omega * t) * seriesActive;
      let injectingVoltageB = targetInjectedPeak * Math.sin(omega * t - (2 * Math.PI) / 3) * seriesActive;
      let injectingVoltageC = targetInjectedPeak * Math.sin(omega * t + (2 * Math.PI) / 3) * seriesActive;
      
      // Add inverter switching noise
      const rippleAmpVolt = 3.5;
      injectingVoltageA += rippleAmpVolt * Math.sin(350 * omega * t);
      injectingVoltageB += rippleAmpVolt * Math.sin(350 * omega * t + Math.PI/4);
      injectingVoltageC += rippleAmpVolt * Math.sin(350 * omega * t - Math.PI/4);

      // 6. Supply (Load) Voltages (Grid Voltage + Series Injecting Voltage)
      // UPQC ensures that load voltage is stable at nominal value even during grid sag/swell
      const supplyVoltageA = gridVoltageA + injectingVoltageA;
      const supplyVoltageB = gridVoltageB + injectingVoltageB;
      const supplyVoltageC = gridVoltageC + injectingVoltageC;

      // 7. DC Link Voltage
      // DC link voltage starts with a transient and settles to the target value.
      // Influenced by microgrid generation: Solar Irradiance, Wind Speed, Battery SOC
      const vDcTarget = params.dcLinkVoltage;
      const vDcInitial = 580; // starting pre-charged level
      
      // Solar and wind power contribution reduces the time constant of charging and raises the steady-state slightly
      const solarPowerFactor = params.solarIrradiance / 1000; // 0 to 1+
      const windPowerFactor = params.windSpeed / 12; // 0 to 1.5
      const batteryFactor = params.batterySOC / 100; // 0 to 1
      
      const totalMgPower = (solarPowerFactor * 15) + (windPowerFactor * 10) + (batteryFactor * 5); // kW mock
      
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
        injectingVoltageA: Math.round(injectingVoltageA * 10) / 10,
        injectingVoltageB: Math.round(injectingVoltageB * 10) / 10,
        injectingVoltageC: Math.round(injectingVoltageC * 10) / 10,
        injectingCurrentA: Math.round(injectingCurrentA * 100) / 100,
        injectingCurrentB: Math.round(injectingCurrentB * 100) / 100,
        injectingCurrentC: Math.round(injectingCurrentC * 100) / 100,
        dcLinkVoltage: Math.round(dcLinkVoltage * 10) / 10,
      });
    }

    return {
      success: true,
      message: 'Simulation run completed successfully.',
      dataPoints,
    };
  }
}
