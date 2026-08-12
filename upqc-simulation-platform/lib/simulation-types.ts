export interface SimulationParameters {
  // Microgrid parameters
  microgridVoltage: number; // V
  microgridFrequency: number; // Hz
  solarIrradiance: number; // W/m^2
  solarStringsParallel?: number; // default: 88
  solarModulesSeries?: number; // default: 7
  solarPanelWatts?: number; // default: 415 W
  windSpeed: number; // m/s
  batterySOC: number; // %
  dcLinkVoltage: number; // V (target DC voltage)

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


  // DC link voltage (V)
  dcLinkVoltage: number;

  // Solar PV output telemetry
  solarPowerWatts: number;
  solarVoltageDc: number;
  solarCurrentDc: number;
  solarIrradiance: number;
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
