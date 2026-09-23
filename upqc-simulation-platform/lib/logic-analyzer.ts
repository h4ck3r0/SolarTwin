import { SimulationDataPoint, SimulationParameters } from './simulation-types';

export interface LogicState {
  solar: {
    state: 'Generating' | 'Idle' | 'Offline';
    powerKw: number;
    description: string;
  };
  grid: {
    state: 'Supplying Load' | 'Absorbing Excess' | 'Disconnected' | 'Balanced';
    powerKw: number;
    description: string;
  };
  battery: {
    state: 'Charging' | 'Discharging' | 'Idle' | 'Disconnected';
    soc: number;
    description: string;
  };
  upqcSeries: {
    state: 'Injecting Voltage (Sag)' | 'Bucking Voltage (Swell)' | 'Idle';
    description: string;
  };
  upqcShunt: {
    state: 'Active Filtering' | 'Idle';
    description: string;
  };
  load: {
    powerKw: number;
    description: string;
  };
}

// Calculate RMS value from an array of instantaneous values
function computeRms(waveform: number[]): number {
  if (waveform.length === 0) return 0;
  const sumSq = waveform.reduce((acc, val) => acc + val * val, 0);
  return Math.sqrt(sumSq / waveform.length);
}

export function analyzeLogic(
  dataPoints: SimulationDataPoint[],
  params: SimulationParameters,
  connectedNodeIds?: Set<string>
): LogicState {
  if (!dataPoints || dataPoints.length === 0) {
    return {
      solar: { state: 'Offline', powerKw: 0, description: 'No data' },
      grid: { state: 'Disconnected', powerKw: 0, description: 'No data' },
      battery: { state: 'Disconnected', soc: 0, description: 'No data' },
      upqcSeries: { state: 'Idle', description: 'No data' },
      upqcShunt: { state: 'Idle', description: 'No data' },
      load: { powerKw: 0, description: 'No data' }
    };
  }

  // Analyze the steady-state (last 10% of the simulation)
  const tailLength = Math.max(10, Math.floor(dataPoints.length * 0.1));
  const tail = dataPoints.slice(-tailLength);

  // Averages in the tail
  const avgSolarW = tail.reduce((sum, dp) => sum + dp.solarPowerWatts, 0) / tailLength;
  const avgSolarKw = avgSolarW / 1000;
  
  // FIX BUG-F04: batterySOC is now Optional — null means disconnected
  const finalSoc = tail[tailLength - 1].batterySOC ?? null;

  // Grid RMS voltage (Phase A)
  const gridVoltsA = tail.map(dp => dp.gridVoltageA);
  const vGridRms = computeRms(gridVoltsA);
  
  // Load voltage (Phase A)
  const loadVoltsA = tail.map(dp => dp.supplyVoltageA);
  const vLoadRms = computeRms(loadVoltsA);

  // Injecting voltage (Phase A)
  const injVoltsA = tail.map(dp => dp.injectingVoltageA);
  const vInjRms = computeRms(injVoltsA);

  // Shunt injection current (Phase A)
  const injCurrentA = tail.map(dp => dp.injectingCurrentA);
  const iShuntRms = computeRms(injCurrentA);

  // Grid Power (Approximate using V_rms * I_rms * 3, assuming balanced for simplicity here,
  // or use the difference between load and solar if we don't have exact instantaneous power calc).
  // Simplest for CS insight: P_grid = P_load - P_solar - P_battery
  const loadKw = params.loadActivePower || 0;
  const netKw = avgSolarKw - loadKw;

  // Evaluate Solar State
  let solarState: 'Generating' | 'Idle' | 'Offline' = 'Offline';
  let solarDesc = 'Solar array is disconnected or tripped.';
  if (avgSolarKw > 0.1) {
    solarState = 'Generating';
    solarDesc = `Solar array is actively generating ${avgSolarKw.toFixed(1)} kW from available irradiance.`;
  } else if (avgSolarKw <= 0.1 && (params.solarIrradiance || 0) > 0) {
    solarState = 'Idle';
    solarDesc = 'Solar array is connected but generating negligible power (possibly restricted).';
  }

  // Evaluate Battery State
  // If connectedNodeIds is provided, use it as the ground truth for whether battery is wired in.
  const batteryWired = connectedNodeIds
    ? [...connectedNodeIds].some(id => id.includes('battery') || id.includes('bess'))
    : finalSoc != null && finalSoc >= 0;

  let batteryState: 'Charging' | 'Discharging' | 'Idle' | 'Disconnected' = 'Disconnected';
  let batteryDesc = 'Battery BESS is not wired into the DC Link — it is offline.';
  const displaySoc = batteryWired ? (finalSoc ?? 0) : 0;

  if (batteryWired && finalSoc != null && finalSoc >= 0) {
    // Determine charging/discharging based on net power and SOC limits
    if (netKw > 0.5 && finalSoc < 100) {
      batteryState = 'Charging';
      batteryDesc = `Excess solar power is charging the battery (SOC: ${finalSoc.toFixed(1)}%).`;
    } else if (netKw < -0.5 && finalSoc > 0) {
      batteryState = 'Discharging';
      batteryDesc = `Battery is discharging to help meet load demand (SOC: ${finalSoc.toFixed(1)}%).`;
    } else {
      batteryState = 'Idle';
      batteryDesc = `Battery is in standby mode (SOC: ${finalSoc.toFixed(1)}%).`;
    }
  }

  // Evaluate Grid State
  let gridState: 'Supplying Load' | 'Absorbing Excess' | 'Disconnected' | 'Balanced' = 'Disconnected';
  let gridDesc = 'Main grid is disconnected (Islanded mode).';
  let gridPowerKw = 0;
  
  // FIX BUG-F04: Use `?? true` so old localStorage params (missing this key) default to grid-connected
  if ((params.isGridConnected ?? true) !== false) {
    // If battery absorbs all excess, grid might be balanced. Otherwise, grid takes the rest.
    if (netKw > 1.0) {
       gridState = 'Absorbing Excess';
       gridPowerKw = netKw; // roughly
       gridDesc = `After feeding the load (and battery), ${gridPowerKw.toFixed(1)} kW of excess solar is exported back to the main grid.`;
    } else if (netKw < -1.0) {
       gridState = 'Supplying Load';
       gridPowerKw = Math.abs(netKw);
       gridDesc = `Local generation is insufficient. The main grid is supplying ${gridPowerKw.toFixed(1)} kW to meet the load.`;
    } else {
       gridState = 'Balanced';
       gridDesc = 'Local generation exactly matches the load. Minimal power exchange with the grid.';
    }
  }

  // Evaluate UPQC Series
  let upqcSeriesState: 'Injecting Voltage (Sag)' | 'Bucking Voltage (Swell)' | 'Idle' = 'Idle';
  let seriesDesc = 'Grid voltage is normal; series compensator is idle.';
  const expectedVrms = (params.microgridVoltage || 415) / Math.sqrt(3);
  
  if (vInjRms > 5) { // more than 5V RMS injection
    if (vGridRms < expectedVrms - 5) {
      upqcSeriesState = 'Injecting Voltage (Sag)';
      seriesDesc = `Grid voltage dipped to ${(vGridRms * Math.sqrt(3)).toFixed(0)}V. The UPQC series transformer is injecting +${(vInjRms * Math.sqrt(3)).toFixed(0)}V to maintain a clean load voltage.`;
    } else if (vGridRms > expectedVrms + 5) {
      upqcSeriesState = 'Bucking Voltage (Swell)';
      seriesDesc = `Grid voltage swelled to ${(vGridRms * Math.sqrt(3)).toFixed(0)}V. The UPQC series transformer is bucking -${(vInjRms * Math.sqrt(3)).toFixed(0)}V to protect the load.`;
    }
  }

  // Evaluate UPQC Shunt
  let upqcShuntState: 'Active Filtering' | 'Idle' = 'Idle';
  let shuntDesc = 'Load current is clean; shunt compensator is idle.';
  if (iShuntRms > 1.0) {
    upqcShuntState = 'Active Filtering';
    shuntDesc = `The load is drawing distorted current (THD). The UPQC shunt inverter is injecting ${iShuntRms.toFixed(1)}A RMS of 'anti-noise' current to cancel out the distortion and keep the grid clean.`;
  }

  return {
    solar: { state: solarState, powerKw: avgSolarKw, description: solarDesc },
    grid: { state: gridState, powerKw: gridPowerKw, description: gridDesc },
    battery: { state: batteryState, soc: displaySoc, description: batteryDesc },
    upqcSeries: { state: upqcSeriesState, description: seriesDesc },
    upqcShunt: { state: upqcShuntState, description: shuntDesc },
    load: { powerKw: loadKw, description: `Load requires ${loadKw.toFixed(1)} kW of power.` }
  };
}
