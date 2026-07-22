export interface SimulationParameters {
  // Grid parameters
  gridVoltage: number; // V (RMS, line-to-line)
  gridFrequency: number; // Hz
  gridRs: number; // Source resistance (ohms)
  gridLs: number; // Source inductance (H)

  // Microgrid parameters
  solarIrradiance: number; // W/m^2
  solarPanelCount?: number; // default: 5
  solarPanelWatts?: number; // default: 305 W
  windSpeed: number; // m/s
  batterySOC: number; // %
  dcLinkVoltage: number; // V (target DC voltage)

  // APF Controller parameters
  kpSeries: number;
  kiSeries: number;
  refVoltSeries: number; // V (target load voltage RMS)
  
  kpShunt: number;
  kiShunt: number;
  refVoltShunt: number; // V
}

export interface SimulationDataPoint {
  time: number;
  
  // Three-phase grid voltages (V)
  gridVoltageA: number;
  gridVoltageB: number;
  gridVoltageC: number;
  
  // Three-phase grid currents (A)
  gridCurrentA: number;
  gridCurrentB: number;
  gridCurrentC: number;
  
  // Three-phase supply/load voltages (V)
  supplyVoltageA: number;
  supplyVoltageB: number;
  supplyVoltageC: number;
  
  // Three-phase load currents (A)
  loadCurrentA: number;
  loadCurrentB: number;
  loadCurrentC: number;

  // Three-phase series injecting voltages (V)
  injectingVoltageA: number;
  injectingVoltageB: number;
  injectingVoltageC: number;

  // Three-phase shunt injecting currents (A)
  injectingCurrentA: number;
  injectingCurrentB: number;
  injectingCurrentC: number;
  
  // DC link voltage (V)
  dcLinkVoltage: number;

  // Solar PV output telemetry
  solarPowerWatts: number;
  solarVoltageDc: number;
  solarCurrentDc: number;
}

export interface SimulationResult {
  success: boolean;
  message: string;
  dataPoints: SimulationDataPoint[];
}

export interface SimulationService {
  runSimulation(parameters: SimulationParameters): Promise<SimulationResult>;
}

export type SimulationStatus = 'IDLE' | 'RUNNING' | 'COMPLETED' | 'FAILED';
