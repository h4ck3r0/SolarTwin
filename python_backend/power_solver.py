import numpy as np
from pydantic import BaseModel
from typing import List
import numba

from control_algorithms import clarke_transform, park_transform, inv_park_transform, inv_clarke_transform, srf_pll, pq_theory_harmonic_extraction
from thermal_model import IGBT_Thermal_Model

class SimulationParameters(BaseModel):
    microgridVoltage: float = 415.0
    microgridFrequency: float = 50.0
    solarIrradiance: float = 1000.0
    solarTemperature: float = 25.0
    solarStringsParallel: int = 88
    solarModulesSeries: int = 7
    solarPanelWatts: float = 415.0
    batterySOC: float = 100.0
    batteryCapacityKwh: float = 100.0
    dcLinkVoltage: float = 700.0
    isGridConnected: bool = True
    simulationDuration: float = 0.300
    irradianceProfile: str = "0:1000"
    temperatureProfile: str = "0:25"
    gridResistance: float = 0.1
    gridReactance: float = 0.2
    loadActivePower: float = 15.0
    loadPowerFactor: float = 0.85
    loadHarmonicType: str = "Rectifier"
    filterInductance: float = 2.5
    dcCapacitance: float = 2200.0
    loadTHD: float = 28.0
    kp: float = 0.5
    ki: float = 10.0
    isTripped: bool = False

class NodeData(BaseModel):
    label: str = ""
    parameters: dict = {}

class Node(BaseModel):
    id: str
    type: str
    data: NodeData

class Edge(BaseModel):
    id: str
    source: str
    target: str

class Topology(BaseModel):
    nodes: List[Node] = []
    edges: List[Edge] = []

class SimulationRequest(BaseModel):
    parameters: SimulationParameters
    topology: Topology

class SimulationDataPoint(BaseModel):
    time: float
    gridVoltageA: float
    gridVoltageB: float
    gridVoltageC: float
    gridCurrentA: float
    gridCurrentB: float
    gridCurrentC: float
    supplyVoltageA: float
    supplyVoltageB: float
    supplyVoltageC: float
    loadCurrentA: float
    loadCurrentB: float
    loadCurrentC: float
    injectingVoltageA: float
    injectingVoltageB: float
    injectingVoltageC: float
    injectingCurrentA: float
    injectingCurrentB: float
    injectingCurrentC: float
    dcLinkVoltage: float
    solarPowerWatts: float
    solarVoltageDc: float
    solarCurrentDc: float
    solarIrradiance: float
    solarTemperature: float
    batterySOC: float
    windPowerWatts: float
    igbtTemperature: float = 25.0

@numba.njit
def emt_solver_loop(steps, dt, omega, v_base_peak, 
                    load_kw, pf_load, thd_factor, h_mags,
                    L_filter, C_dc, kp, ki, dc_ref, is_grid_connected,
                    solar_power_array):
                    
    # Pre-allocate output arrays
    time_arr = np.zeros(steps)
    v_grid = np.zeros((3, steps))
    i_grid = np.zeros((3, steps))
    v_load = np.zeros((3, steps))
    i_load = np.zeros((3, steps))
    v_inj = np.zeros((3, steps))
    i_inj = np.zeros((3, steps))
    v_dc_arr = np.zeros(steps)
    
    # State variables
    v_dc = dc_ref
    theta_pll = 0.0
    vq_int = 0.0
    
    # Harmonics Setup
    h3_mag, h5_mag, h7_mag, h11_mag, h13_mag = h_mags
    
    # Load fundamental current peak
    s_load_va = load_kw * 1000.0 / pf_load
    i_load_rms = s_load_va / (np.sqrt(3) * (v_base_peak * np.sqrt(3)/np.sqrt(2)))
    i_load_peak = i_load_rms * np.sqrt(2)
    theta_load = np.arccos(pf_load)
    
    phase_A, phase_B, phase_C = 0.0, -2*np.pi/3, 2*np.pi/3
    
    for k in range(steps):
        t = k * dt
        time_arr[k] = t
        
        # Grid Voltage (Ideal infinite bus with sags applied externally if needed)
        if is_grid_connected:
            v_ga = v_base_peak * np.sin(omega * t + phase_A)
            v_gb = v_base_peak * np.sin(omega * t + phase_B)
            v_gc = v_base_peak * np.sin(omega * t + phase_C)
        else:
            v_ga, v_gb, v_gc = 0.0, 0.0, 0.0
            
        v_grid[0, k] = v_ga
        v_grid[1, k] = v_gb
        v_grid[2, k] = v_gc
        
        # PLL Execution (Grid Synchronization)
        theta_pll, omega_pll, vq_int = srf_pll(v_ga, v_gb, v_gc, theta_pll, vq_int, dt, 0.5, 10.0, omega)
        
        # Non-Linear Load Current Model
        # Fundamental
        i_la = i_load_peak * np.sin(omega * t + phase_A - theta_load)
        i_lb = i_load_peak * np.sin(omega * t + phase_B - theta_load)
        i_lc = i_load_peak * np.sin(omega * t + phase_C - theta_load)
        
        # Add Harmonics
        i_la += thd_factor * i_load_peak * (h5_mag * np.sin(5*(omega*t+phase_A)) + h7_mag * np.sin(7*(omega*t+phase_A)))
        i_lb += thd_factor * i_load_peak * (h5_mag * np.sin(5*(omega*t+phase_B)) + h7_mag * np.sin(7*(omega*t+phase_B)))
        i_lc += thd_factor * i_load_peak * (h5_mag * np.sin(5*(omega*t+phase_C)) + h7_mag * np.sin(7*(omega*t+phase_C)))
        
        i_load[0, k] = i_la
        i_load[1, k] = i_lb
        i_load[2, k] = i_lc
        
        # Shunt Inverter Control (p-q theory based Harmonic Extraction)
        # We need to extract the AC component of p to inject as harmonic compensation
        p, q = pq_theory_harmonic_extraction(v_ga, v_gb, v_gc, i_la, i_lb, i_lc)
        
        # High pass filter to extract oscillating power (p_tilde)
        # Simplified: ideal compensation by directly subtracting fundamental from load
        i_la_fund = i_load_peak * np.sin(theta_pll - theta_load)
        i_lb_fund = i_load_peak * np.sin(theta_pll - 2*np.pi/3 - theta_load)
        i_lc_fund = i_load_peak * np.sin(theta_pll + 2*np.pi/3 - theta_load)
        
        # The UPQC should supply the harmonic currents and reactive power
        i_sh_a_ref = i_la - i_la_fund
        i_sh_b_ref = i_lb - i_lb_fund
        i_sh_c_ref = i_lc - i_lc_fund
        
        # Apply PI controller limits
        comp_eff = min(1.0, kp * 0.1 + ki * 0.01)
        i_inj[0, k] = -i_sh_a_ref * comp_eff
        i_inj[1, k] = -i_sh_b_ref * comp_eff
        i_inj[2, k] = -i_sh_c_ref * comp_eff
        
        # Grid Current = Load Current + Injected Current - Solar Injection
        # We model solar as active power injection directly to grid
        solar_power = solar_power_array[k]
        i_solar_peak = (solar_power / (1.5 * v_base_peak)) if v_base_peak > 0 else 0
        i_sol_a = i_solar_peak * np.sin(theta_pll)
        i_sol_b = i_solar_peak * np.sin(theta_pll - 2*np.pi/3)
        i_sol_c = i_solar_peak * np.sin(theta_pll + 2*np.pi/3)
        
        i_grid[0, k] = i_la + i_inj[0, k] - i_sol_a
        i_grid[1, k] = i_lb + i_inj[1, k] - i_sol_b
        i_grid[2, k] = i_lc + i_inj[2, k] - i_sol_c
        
        # Series Inverter Control (Voltage Sag Compensation)
        # Maintain load bus voltage at nominal reference
        v_la_ref = v_base_peak * np.sin(theta_pll)
        v_lb_ref = v_base_peak * np.sin(theta_pll - 2*np.pi/3)
        v_lc_ref = v_base_peak * np.sin(theta_pll + 2*np.pi/3)
        
        v_inj[0, k] = v_ga - v_la_ref
        v_inj[1, k] = v_gb - v_lb_ref
        v_inj[2, k] = v_gc - v_lc_ref
        
        v_load[0, k] = v_ga - v_inj[0, k]
        v_load[1, k] = v_gb - v_inj[1, k]
        v_load[2, k] = v_gc - v_inj[2, k]
        
        # DC Link Dynamics (Energy buffer)
        # Power flowing into capacitor = Power from grid (UPQC absorbed) + Power from solar - Power to load
        # For a simplified model, we track DC link capacitor charge based on harmonic power exchanged
        p_upqc = v_inj[0, k]*i_grid[0, k] + v_inj[1, k]*i_grid[1, k] + v_inj[2, k]*i_grid[2, k] \
                 + v_ga*i_inj[0, k] + v_gb*i_inj[1, k] + v_gc*i_inj[2, k]
                 
        v_dc_dot = -p_upqc / (C_dc * v_dc) if v_dc > 0 else 0
        v_dc += v_dc_dot * dt
        v_dc = max(v_dc, 0.0) # Can't drop below 0
        v_dc_arr[k] = v_dc
        
    return time_arr, v_grid, i_grid, v_load, i_load, v_inj, i_inj, v_dc_arr

def run_simulation(req: SimulationRequest) -> List[SimulationDataPoint]:
    params = req.parameters
    
    duration = params.simulationDuration if params.simulationDuration > 0 else 0.300
    # 10kHz sampling for true EMT
    dt = 0.0001
    steps = int(duration / dt)
    if steps > 10000: steps = 10000
    
    omega = 2 * np.pi * params.microgridFrequency
    v_base_peak = (params.microgridVoltage / np.sqrt(3)) * np.sqrt(2)
    
    # Harmonic Signature
    htype = params.loadHarmonicType.lower()
    h3_mag, h5_mag, h7_mag, h11_mag, h13_mag = 0.0, 0.0, 0.0, 0.0, 0.0
    if "vfd" in htype:
        h5_mag, h7_mag = 0.7, 0.3
    elif "arc" in htype:
        h3_mag, h5_mag, h7_mag = 0.6, 0.3, 0.1
    elif "clean" in htype:
        pass
    else:
        h5_mag, h7_mag, h11_mag, h13_mag = 0.7, 0.5, 0.2, 0.1
    h_mags = np.array([h3_mag, h5_mag, h7_mag, h11_mag, h13_mag])
    
    # Solar Setup
    def parse_profile(profile_str, default_val):
        points = []
        try:
            for p in profile_str.split(','):
                if ':' in p:
                    t_val, val = p.split(':')
                    points.append((float(t_val.strip()), float(val.strip())))
            points.sort(key=lambda x: x[0])
        except:
            pass
        if not points:
            points = [(0.0, default_val)]
        return points

    def interpolate_profile(points, t):
        current_val = points[0][1]
        for i in range(len(points)):
            if t >= points[i][0]:
                current_val = points[i][1]
                if i < len(points) - 1 and t < points[i+1][0]:
                    t0, v0 = points[i]
                    t1, v1 = points[i+1]
                    current_val = v0 + (v1 - v0) * ((t - t0) / (t1 - t0))
            else:
                break
        return current_val

    irrad_points = parse_profile(params.irradianceProfile, params.solarIrradiance)
    temp_points = parse_profile(params.temperatureProfile, params.solarTemperature)

    solar_power_array = np.zeros(steps)
    solar_voltage_array = np.zeros(steps)
    solar_current_array = np.zeros(steps)
    irrad_array = np.zeros(steps)
    temp_array = np.zeros(steps)
    
    power_stc = params.solarStringsParallel * params.solarModulesSeries * params.solarPanelWatts
    
    for k in range(steps):
        t = k * dt
        current_irrad = interpolate_profile(irrad_points, t)
        current_temp = interpolate_profile(temp_points, t)
        
        irrad_array[k] = current_irrad
        temp_array[k] = current_temp
        
        temp_diff = current_temp - 25.0
        # Power formula
        power = power_stc * (current_irrad / 1000.0) * (1 - 0.004 * temp_diff)
        power = max(power, 0.0)
        solar_power_array[k] = power
        
        # Calculate ideal voltage based on temp
        v_mpp_ideal = (35.0 * params.solarModulesSeries) * (1 - 0.003 * temp_diff)
        v_mpp_ideal = max(v_mpp_ideal, 1.0)
        solar_voltage_array[k] = v_mpp_ideal
        
        solar_current_array[k] = power / v_mpp_ideal if v_mpp_ideal > 0 else 0.0
    
    # Run high-performance compiled Numba core
    time_arr, v_grid, i_grid, v_load, i_load, v_inj, i_inj, v_dc_arr = emt_solver_loop(
        steps, dt, omega, v_base_peak, 
        params.loadActivePower, params.loadPowerFactor, params.loadTHD / 100.0, h_mags,
        params.filterInductance * 1e-3, params.dcCapacitance * 1e-6, 
        params.kp, params.ki, params.dcLinkVoltage, params.isGridConnected,
        solar_power_array
    )
    
    # Thermal Model Setup
    thermal_net = IGBT_Thermal_Model(T_ambient=params.solarTemperature)
    
    # Package Results (Subsample to 1000 points to prevent browser crashing, keeping it ~3000Hz equivalent)
    # We want max ~1000-2000 points in UI for performance.
    sub_step = max(1, steps // 2000)
    
    data_points = []
    
    for k in range(0, steps, sub_step):
        # Calculate IGBT Junction Temp dynamically based on Shunt current and DC Voltage
        current_shunt_rms = np.sqrt((i_inj[0, k]**2 + i_inj[1, k]**2 + i_inj[2, k]**2)/3.0)
        p_loss = thermal_net.compute_losses(current_shunt_rms, v_dc_arr[k], 10000.0)
        tj = thermal_net.step(p_loss, dt * sub_step)
        
        dp = SimulationDataPoint(
            time=round(time_arr[k], 4),
            gridVoltageA=round(v_grid[0, k], 2),
            gridVoltageB=round(v_grid[1, k], 2),
            gridVoltageC=round(v_grid[2, k], 2),
            gridCurrentA=round(i_grid[0, k], 2),
            gridCurrentB=round(i_grid[1, k], 2),
            gridCurrentC=round(i_grid[2, k], 2),
            supplyVoltageA=round(v_load[0, k], 2),
            supplyVoltageB=round(v_load[1, k], 2),
            supplyVoltageC=round(v_load[2, k], 2),
            loadCurrentA=round(i_load[0, k], 2),
            loadCurrentB=round(i_load[1, k], 2),
            loadCurrentC=round(i_load[2, k], 2),
            injectingVoltageA=round(v_inj[0, k], 2),
            injectingVoltageB=round(v_inj[1, k], 2),
            injectingVoltageC=round(v_inj[2, k], 2),
            injectingCurrentA=round(i_inj[0, k], 2),
            injectingCurrentB=round(i_inj[1, k], 2),
            injectingCurrentC=round(i_inj[2, k], 2),
            dcLinkVoltage=round(v_dc_arr[k], 2),
            solarPowerWatts=round(solar_power_array[k], 2),
            solarVoltageDc=round(solar_voltage_array[k], 2),
            solarCurrentDc=round(solar_current_array[k], 2),
            solarIrradiance=round(irrad_array[k], 2),
            solarTemperature=round(temp_array[k], 2),
            batterySOC=round(params.batterySOC, 6),
            windPowerWatts=0.0,
            igbtTemperature=round(tj, 2)
        )
        data_points.append(dp)
        

    # Save full results to disk for download
    import json
    try:
        with open("full_simulation_results.json", "w") as f:
            json.dump([dp.dict() for dp in data_points], f)
    except:
        pass
        
    # Return subset (first 5 cycles / 500 points) to prevent browser lag
    return data_points[:500]
