export interface SimulationParameters {
  // Microgrid parameters
  microgridVoltage: number; // V
  microgridFrequency: number; // Hz
  solarIrradiance: number; // W/m^2
  solarTemperature?: number; // deg C
  solarStringsParallel?: number; // default: 88
  solarModulesSeries?: number; // default: 7
  solarPanelWatts?: number; // default: 415 W // m/s
  batterySOC: number; // %
  batteryCapacityKwh?: number; // kWh
  dcLinkVoltage: number; // V (target DC voltage)
  isGridConnected?: boolean;
  simulationDuration?: number;
  irradianceProfile?: string;
  temperatureProfile?: string;
  isTripped?: boolean; // Universal disconnect switch
  
  // Wind Turbine Parameters
  windSpeed?: number; // m/s
  windCutIn?: number; // m/s
  windCutOut?: number; // m/s
  windNominalPower?: number; // kW
  
  // MPPT Controller Parameters
  mpptAlgorithm?: string; // 'PO', 'INC', 'CV'
  mpptStepSize?: number; // V
  mpptUpdateFreq?: number; // ms

  // Grid & EMT parameters
  gridResistance?: number; // Ohms
  gridReactance?: number; // Ohms
  loadActivePower?: number; // kW
  loadPowerFactor?: number; // 0.0 to 1.0
  loadHarmonicType?: string; // VFD, Rectifier, ArcFurnace, Clean
  
  // Advanced Physics Parameters
  filterInductance?: number; // mH
  dcCapacitance?: number; // uF
  loadTHD?: number; // %
  kp?: number; // PI Proportional Gain
  ki?: number; // PI Integral Gain
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

  // Three-phase injecting voltages (V)
  injectingVoltageA: number;
  injectingVoltageB: number;
  injectingVoltageC: number;

  // Three-phase injecting currents (A)
  injectingCurrentA: number;
  injectingCurrentB: number;
  injectingCurrentC: number;


  // DC link voltage (V)
  dcLinkVoltage: number;

  // Solar PV output telemetry
  solarPowerWatts: number;
  solarVoltageDc: number;
  solarCurrentDc: number;
  solarIrradiance: number;
  solarTemperature: number;
  
  // Storage & Wind Telemetry
  batterySOC?: number;
  windPowerWatts?: number;
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
