export interface SimulationParameters {
  // AC Microgrid
  microgridVoltage: number;
  microgridFrequency: number;

  // Solar PV
  solarIrradiance: number;
  solarTemperature?: number;
  solarStringsParallel?: number;
  solarModulesSeries?: number;
  solarPanelWatts?: number;
  solarVmpp?: number;          // MPP voltage per module at STC (V)

  // Battery
  batterySOC: number;
  batteryCapacityKwh?: number;

  // DC Link & UPQC
  dcLinkVoltage: number;
  filterInductance?: number;
  dcCapacitance?: number;
  kp?: number;
  ki?: number;

  // Grid
  isGridConnected?: boolean;
  gridResistance?: number;
  gridReactance?: number;

  // Load
  loadActivePower?: number;
  loadPowerFactor?: number;
  loadHarmonicType?: string;
  loadTHD?: number;

  // Wind
  windSpeed?: number;
  windCutIn?: number;
  windCutOut?: number;
  windNominalPower?: number;

  // Simulation control
  simulationDuration?: number;
  irradianceProfile?: string;
  temperatureProfile?: string;
  isTripped?: boolean;
}

export interface SimulationDataPoint {
  time: number;
  gridVoltageA: number; gridVoltageB: number; gridVoltageC: number;
  gridCurrentA: number; gridCurrentB: number; gridCurrentC: number;
  supplyVoltageA: number; supplyVoltageB: number; supplyVoltageC: number;
  loadCurrentA: number; loadCurrentB: number; loadCurrentC: number;
  injectingVoltageA: number; injectingVoltageB: number; injectingVoltageC: number;
  injectingCurrentA: number; injectingCurrentB: number; injectingCurrentC: number;
  dcLinkVoltage: number;
  solarPowerWatts: number;
  solarVoltageDc: number;
  solarCurrentDc: number;
  solarIrradiance: number;
  solarTemperature: number;
  batterySOC?: number;
  windPowerWatts?: number;
  igbtTemperature?: number;
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
