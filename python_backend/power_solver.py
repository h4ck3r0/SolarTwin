import pandapower as pp
import numpy as np
from pydantic import BaseModel
from typing import List

class SimulationParameters(BaseModel):
    microgridVoltage: float = 415.0
    microgridFrequency: float = 60.0
    solarIrradiance: float = 1000.0
    solarTemperature: float = 25.0
    solarStringsParallel: int = 88
    solarModulesSeries: int = 7
    solarPanelWatts: float = 415.0
    batterySOC: float
    batteryCapacityKwh: float = 100.0
    dcLinkVoltage: float
    isGridConnected: bool = True
    simulationDuration: float = 0.300
    irradianceProfile: str = "0:1000"
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

def run_simulation(req: SimulationRequest) -> List[SimulationDataPoint]:
    params = req.parameters
    topology = req.topology
    # 1. Base Load Flow for Fundamental RMS
    net = pp.create_empty_network(f_hz=params.microgridFrequency)
    vn_kv = params.microgridVoltage / 1000.0
    b_grid = pp.create_bus(net, vn_kv=vn_kv, name="Grid Bus")
    b_load = pp.create_bus(net, vn_kv=vn_kv, name="Microgrid Bus")
    pp.create_ext_grid(net, bus=b_grid, vm_pu=1.0, va_degree=0.0)
    
    # Accurate Physics Line Impedance
    pp.create_line_from_parameters(net, from_bus=b_grid, to_bus=b_load, length_km=1.0, 
                                   r_ohm_per_km=params.gridResistance, x_ohm_per_km=params.gridReactance, 
                                   c_nf_per_km=0.0, max_i_ka=10.0, name="Grid Impedance Feeder")


    # Dynamic Topology Compiler
    has_battery_node = False
    solar_arrays_power = []
    actual_solar_modules = []
    wind_power_total = 0.0
    
    # Extract connected node IDs from edges
    connected_node_ids = set()
    for edge in topology.edges:
        connected_node_ids.add(edge.source)
        connected_node_ids.add(edge.target)
    
    for node in topology.nodes:
        # Require the node to be connected via an edge, unless it's a core un-deletable node
        if node.id not in connected_node_ids and node.id != "grid-source" and node.id != "critical-load":
            continue
            
        p_params = node.data.parameters
        if p_params.get('isTripped', False):
            continue # Physics Isolation Switch
            
        n_id = node.id.lower()
        n_type = node.type.lower()
        n_label = (node.data.label or "").lower()
        
        if "battery" in n_id or "battery" in n_type or "battery" in n_label:
            has_battery_node = True
            
        elif "solar" in n_id or "solar" in n_type or "solar" in n_label:
            # Extract parameters safely, default to global if not specific
            irrad = float(p_params.get('solarIrradiance', params.solarIrradiance))
            temp = float(p_params.get('solarTemperature', params.solarTemperature))
            strings = int(p_params.get('solarStringsParallel', params.solarStringsParallel))
            modules = int(p_params.get('solarModulesSeries', params.solarModulesSeries))
            watts = float(p_params.get('solarPanelWatts', params.solarPanelWatts))
            
            power_stc = strings * modules * watts
            # Thermal Degradation Physics (-0.4% per deg C above 25)
            temp_diff = temp - 25.0
            power = power_stc * (irrad / 1000.0) * (1 - 0.004 * temp_diff)
            power = max(power, 0.0)
            
            solar_arrays_power.append(power)
            actual_solar_modules.append(modules)
            pp.create_sgen(net, bus=b_load, p_mw=power / 1e6, q_mvar=0.0, name=node.data.label)
            
        elif "mppt" in n_id or "mppt" in n_type or "mppt" in n_label:
            mppt_params = p_params
            
        elif "wind" in n_id or "wind" in n_type or "wind" in n_label:
            # DFIG Wind Physics
            v_wind = float(p_params.get('windSpeed', 0.0))
            v_cutin = float(p_params.get('windCutIn', 3.0))
            v_cutout = float(p_params.get('windCutOut', 25.0))
            p_nom = float(p_params.get('windNominalPower', 50.0)) * 1000.0 # to Watts
            
            w_power = 0.0
            if v_wind < v_cutin or v_wind > v_cutout:
                w_power = 0.0
            else:
                # Simplified Cubic Power Curve between cut-in and approx rated (12 m/s)
                v_rated = 12.0
                if v_wind >= v_rated:
                    w_power = p_nom
                else:
                    # Power proportional to v^3
                    k = p_nom / (v_rated**3 - v_cutin**3)
                    w_power = k * (v_wind**3 - v_cutin**3)
                    
            wind_power_total += w_power
            pp.create_sgen(net, bus=b_load, p_mw=w_power / 1e6, q_mvar=0.0, name=node.data.label)

    if not solar_arrays_power:
        # Fallback if no nodes drawn
        power_stc = params.solarStringsParallel * params.solarModulesSeries * params.solarPanelWatts
        temp_diff = params.solarTemperature - 25.0
        solar_power_watts = power_stc * (params.solarIrradiance / 1000.0) * (1 - 0.004 * temp_diff)
        solar_power_watts = max(solar_power_watts, 0.0)
        pp.create_sgen(net, bus=b_load, p_mw=solar_power_watts / 1e6, q_mvar=0.0, name="Solar PV")
        effective_modules = params.solarModulesSeries
    else:
        solar_power_watts = sum(solar_arrays_power)
        effective_modules = max(actual_solar_modules)


    # Critical Load Apparent Power
    p_mw = params.loadActivePower / 1000.0
    q_mvar = p_mw * np.tan(np.arccos(max(params.loadPowerFactor, 0.1))) if params.loadPowerFactor < 1.0 else 0.0
    pp.create_load(net, bus=b_load, p_mw=p_mw, q_mvar=q_mvar, name="Critical Load")

    try:
        pp.runpp(net)
    except Exception as e:
        print(f"Pandapower runpp failed: {e}")
        pass

    vm_load_pu = net.res_bus.vm_pu.at[b_load]
    va_load_deg = net.res_bus.va_degree.at[b_load]
    i_line_ka = net.res_line.i_ka.at[0]    # 2. Physics-based EMT Math Model over dynamic duration
    duration = params.simulationDuration if params.simulationDuration > 0 else 0.300
    
    # Parse Irradiance Profile
    # Format: "0:1000, 1:500, 2:800"
    profile_points = []
    try:
        parts = params.irradianceProfile.split(',')
        for p in parts:
            if ':' in p:
                t_val, ir_val = p.split(':')
                profile_points.append((float(t_val.strip()), float(ir_val.strip())))
        profile_points.sort(key=lambda x: x[0])
    except:
        pass
        
    if not profile_points:
        profile_points = [(0.0, params.solarIrradiance)]
    
    # Calculate step count. Keep high resolution (1ms) up to 1 second, then scale down for performance.
    # Max points = 10000 to prevent browser crash.
    max_steps = 10000
    step_size = 0.001
    steps = int(duration / step_size)
    if steps > max_steps:
        steps = max_steps
        
    t = np.linspace(0, duration, steps + 1)
    
    omega = 2 * np.pi * params.microgridFrequency
    v_base_peak = (params.microgridVoltage / np.sqrt(3)) * np.sqrt(2)
    
    # The base solar power calculation from load flow (used for delta)
    base_solar_power = sum(solar_arrays_power) if solar_arrays_power else (
        (params.solarIrradiance / 1000.0) * params.solarStringsParallel * params.solarModulesSeries * params.solarPanelWatts
    )
    
    
    # -----------------------------------------------------
    # NEW PHYSICS OVERHAUL: True Nodal KCL Mathematics
    # -----------------------------------------------------
    v_ll = params.microgridVoltage
    pf_load = max(params.loadPowerFactor, 0.1)
    
    # True Fundamental Load Current (Stays perfectly constant regardless of solar/grid!)
    p_load_kw = params.loadActivePower
    s_load_kva = p_load_kw / pf_load
    i_load_rms_true = (s_load_kva * 1000) / (np.sqrt(3) * v_ll) if v_ll > 0 else 0
    i_load_peak_true = i_load_rms_true * np.sqrt(2)
    
    # Transient Decay parameters (L/R time constant)
    L_grid = params.gridReactance / omega
    tau = L_grid / params.gridResistance if params.gridResistance > 0 else 0.001
    
    # MPPT State Initialization
    mppt_algo = mppt_params.get('mpptAlgorithm', 'PO') if 'mppt_params' in locals() else None
    mppt_step = float(mppt_params.get('mpptStepSize', 1.0)) if 'mppt_params' in locals() else 1.0
    mppt_freq = int(mppt_params.get('mpptUpdateFreq', 10)) if 'mppt_params' in locals() else 10
    
    mppt_v_ref = 35.0 * effective_modules * (1 - 0.003 * (params.solarTemperature - 25.0))
    mppt_p_prev = 0.0
    mppt_v_prev = 0.0
    mppt_i_prev = 0.0
    mppt_counter = 0

    data_points = []
    for time_sec in t:
        phase_A = 0
        phase_B = -2 * np.pi / 3
        phase_C = 2 * np.pi / 3
        
        # --- Interpolate Irradiance ---
        current_irradiance = profile_points[0][1]
        for i in range(len(profile_points)):
            if time_sec >= profile_points[i][0]:
                current_irradiance = profile_points[i][1]
                if i < len(profile_points) - 1 and time_sec < profile_points[i+1][0]:
                    t0, ir0 = profile_points[i]
                    t1, ir1 = profile_points[i+1]
                    current_irradiance = ir0 + (ir1 - ir0) * ((time_sec - t0) / (t1 - t0))
            else:
                break
                
        # True Voltage Thermal Degradation
        temp_diff = params.solarTemperature - 25.0
        v_mpp_ideal = (35.0 * effective_modules) * (1 - 0.003 * temp_diff)
        v_mpp_ideal = max(v_mpp_ideal, 1.0)
                
        # Base power available at current irradiance
        if params.solarIrradiance > 0:
            p_available = base_solar_power * (current_irradiance / params.solarIrradiance)
        else:
            p_available = 0.0
            
        # Simulating Solar P-V curve (parabola around V_mpp)
        if p_available > 0:
            v_err = (mppt_v_ref - v_mpp_ideal) / v_mpp_ideal
            dynamic_solar_power = p_available * (1 - 2.0 * (v_err ** 2))
            dynamic_solar_power = max(dynamic_solar_power, 0.0)
        else:
            dynamic_solar_power = 0.0
            
        # MPPT Controller Execution
        # Runs based on configured frequency (1 step = 1ms)
        if mppt_algo is not None and mppt_counter >= mppt_freq:
            mppt_counter = 0
            if dynamic_solar_power > 0:
                current_v = mppt_v_ref
                current_p = dynamic_solar_power
                current_i = current_p / current_v if current_v > 0 else 0
                
                delta_p = current_p - mppt_p_prev
                delta_v = current_v - mppt_v_prev
                delta_i = current_i - mppt_i_prev
                
                if mppt_algo == 'PO':
                    # Perturb & Observe
                    if abs(delta_p) > 1.0 and abs(delta_v) > 0:
                        if delta_p > 0:
                            if delta_v > 0: mppt_v_ref += mppt_step
                            else: mppt_v_ref -= mppt_step
                        else:
                            if delta_v > 0: mppt_v_ref -= mppt_step
                            else: mppt_v_ref += mppt_step
                
                elif mppt_algo == 'INC':
                    # Incremental Conductance
                    if abs(delta_v) < 0.1:
                        if delta_i > 0.01: mppt_v_ref += mppt_step
                        elif delta_i < -0.01: mppt_v_ref -= mppt_step
                    else:
                        di_dv = delta_i / delta_v
                        i_v = current_i / current_v if current_v > 0 else 0
                        
                        if di_dv > -i_v: mppt_v_ref += mppt_step
                        elif di_dv < -i_v: mppt_v_ref -= mppt_step
                        
                elif mppt_algo == 'CV':
                    # Constant Voltage (76% of Voc roughly maps to Vmpp_ideal)
                    mppt_v_ref = v_mpp_ideal
                    
                mppt_p_prev = current_p
                mppt_v_prev = current_v
                mppt_i_prev = current_i
        elif mppt_algo is None:
            # Sub-optimal fixed voltage when MPPT controller is missing or disconnected
            mppt_v_ref = v_mpp_ideal * 1.25 # 25% off optimum
            
        mppt_counter += 1
            
        # Grid Voltage (Ideal source)
        gvA = v_base_peak * np.sin(omega * time_sec + phase_A)
        gvB = v_base_peak * np.sin(omega * time_sec + phase_B)
        gvC = v_base_peak * np.sin(omega * time_sec + phase_C)
        
        # Microgrid Voltage (calculated from load flow)
        svA = vm_load_pu * v_base_peak * np.sin(omega * time_sec + np.deg2rad(va_load_deg) + phase_A)
        svB = vm_load_pu * v_base_peak * np.sin(omega * time_sec + np.deg2rad(va_load_deg) + phase_B)
        svC = vm_load_pu * v_base_peak * np.sin(omega * time_sec + np.deg2rad(va_load_deg) + phase_C)
        
        # Grid Disconnect (Islanded Mode) handled at end of step
            
        # Inrush transient decay (exponential startup envelope on current)
        inrush_env = 1.0 + 1.5 * np.exp(-time_sec / tau)
        
        # Fundamental load current (True Load Physics)
        theta_load = np.arccos(pf_load)
        icA_fund = i_load_peak_true * inrush_env * np.sin(omega * time_sec + phase_A - theta_load)
        icB_fund = i_load_peak_true * inrush_env * np.sin(omega * time_sec + phase_B - theta_load)
        icC_fund = i_load_peak_true * inrush_env * np.sin(omega * time_sec + phase_C - theta_load)
                
        # Harmonic injection from Non-Linear Load
        thd_factor = params.loadTHD / 100.0
        
        # Determine signature based on load type
        h3_mag = 0.0
        h5_mag = 0.0
        h7_mag = 0.0
        h11_mag = 0.0
        h13_mag = 0.0
        
        htype = params.loadHarmonicType.lower()
        if "vfd" in htype:
            h5_mag = 0.7; h7_mag = 0.3
        elif "arc" in htype:
            h3_mag = 0.6; h5_mag = 0.3; h7_mag = 0.1
        elif "clean" in htype:
            pass # No harmonics
        else:
            # Default 6-pulse rectifier
            h5_mag = 0.7; h7_mag = 0.5; h11_mag = 0.2; h13_mag = 0.1
            
        h3A = (thd_factor * h3_mag) * i_load_peak_true * np.sin(3 * (omega * time_sec + phase_A))
        h5A = (thd_factor * h5_mag) * i_load_peak_true * np.sin(5 * (omega * time_sec + phase_A))
        h7A = (thd_factor * h7_mag) * i_load_peak_true * np.sin(7 * (omega * time_sec + phase_A))
        h11A = (thd_factor * h11_mag) * i_load_peak_true * np.sin(11 * (omega * time_sec + phase_A))
        h13A = (thd_factor * h13_mag) * i_load_peak_true * np.sin(13 * (omega * time_sec + phase_A))
        
        h3B = (thd_factor * h3_mag) * i_load_peak_true * np.sin(3 * (omega * time_sec + phase_B))
        h5B = (thd_factor * h5_mag) * i_load_peak_true * np.sin(5 * (omega * time_sec + phase_B))
        h7B = (thd_factor * h7_mag) * i_load_peak_true * np.sin(7 * (omega * time_sec + phase_B))
        h11B = (thd_factor * h11_mag) * i_load_peak_true * np.sin(11 * (omega * time_sec + phase_B))
        h13B = (thd_factor * h13_mag) * i_load_peak_true * np.sin(13 * (omega * time_sec + phase_B))
        
        h3C = (thd_factor * h3_mag) * i_load_peak_true * np.sin(3 * (omega * time_sec + phase_C))
        h5C = (thd_factor * h5_mag) * i_load_peak_true * np.sin(5 * (omega * time_sec + phase_C))
        h7C = (thd_factor * h7_mag) * i_load_peak_true * np.sin(7 * (omega * time_sec + phase_C))
        h11C = (thd_factor * h11_mag) * i_load_peak_true * np.sin(11 * (omega * time_sec + phase_C))
        h13C = (thd_factor * h13_mag) * i_load_peak_true * np.sin(13 * (omega * time_sec + phase_C))
        
        # Total Physical Load Current
        lcA = icA_fund + h3A + h5A + h7A + h11A + h13A
        lcB = icB_fund + h3B + h5B + h7B + h11B + h13B
        lcC = icC_fund + h3C + h5C + h7C + h11C + h13C
        
        # True Solar Injection (In-phase with grid voltage, Unity PF)
        i_solar_rms = dynamic_solar_power / (np.sqrt(3) * v_ll) if v_ll > 0 else 0
        i_solar_peak = i_solar_rms * np.sqrt(2)
        
        sol_A = i_solar_peak * np.sin(omega * time_sec + phase_A)
        sol_B = i_solar_peak * np.sin(omega * time_sec + phase_B)
        sol_C = i_solar_peak * np.sin(omega * time_sec + phase_C)
        
        # UPQC Instantaneous p-q Theory Shunt Compensation (Harmonic Filtering)
        comp_efficiency = min(1.0, params.kp * 0.1 + params.ki * 0.01)
        inj_cA = - (h3A + h5A + h7A + h11A + h13A) * comp_efficiency
        inj_cB = - (h3B + h5B + h7B + h11B + h13B) * comp_efficiency
        inj_cC = - (h3C + h5C + h7C + h11C + h13C) * comp_efficiency
        
        # True Nodal KCL: Grid Current = Load - Solar + UPQC Shunt
        gcA = lcA - sol_A + inj_cA
        gcB = lcB - sol_B + inj_cB
        gcC = lcC - sol_C + inj_cC
        
        # UPQC Series Injection (Voltage compensation)
        inj_vA = gvA - svA
        inj_vB = gvB - svB
        inj_vC = gvC - svC

        # Islanded Mode Override
        if not params.isGridConnected:
            gvA, gvB, gvC = 0.0, 0.0, 0.0
            gcA, gcB, gcC = 0.0, 0.0, 0.0
            inj_cA, inj_cB, inj_cC = 0.0, 0.0, 0.0
            inj_vA, inj_vB, inj_vC = 0.0, 0.0, 0.0
        
        # True DC Link & Battery Physics
        actual_dc = params.dcLinkVoltage
        blackout = False
        
        current_soc = params.batterySOC
        dt_hours = step_size / 3600.0
        p_deficit_watts = (p_mw * 1000) - dynamic_solar_power - wind_power_total
        
        if not params.isGridConnected:
            if has_battery_node:
                if p_deficit_watts > 0:
                    # Discharging
                    p_batt = p_deficit_watts
                    c_rate_limit = params.batteryCapacityKwh * 1000.0 # 1C discharge limit in Watts
                    if p_batt > c_rate_limit:
                        blackout = True # Battery tripped on overcurrent!
                    else:
                        current_soc -= (p_batt / (params.batteryCapacityKwh * 1000.0)) * dt_hours * 100.0
                        if current_soc <= 0:
                            current_soc = 0.0
                            blackout = True # Battery empty
                else:
                    # Charging
                    p_batt = p_deficit_watts # Negative
                    current_soc -= (p_batt / (params.batteryCapacityKwh * 1000.0)) * dt_hours * 100.0
                    current_soc = min(current_soc, 100.0)
                
                params.batterySOC = current_soc
            else:
                if p_deficit_watts > 0:
                    # Capacitor discharge equation: V(t) = sqrt(V0^2 - 2*P*t/C)
                    energy_discharged = p_deficit_watts * time_sec
                    v_squared = (params.dcLinkVoltage ** 2) - (2 * energy_discharged) / (params.dcCapacitance * 1e-6)
                    if v_squared > 0:
                        actual_dc = np.sqrt(v_squared)
                    else:
                        actual_dc = 0.0
                
                # If DC link drops below AC peak voltage, inverter fails (Blackout)
                if actual_dc < v_base_peak:
                    blackout = True

        if blackout:
            # Complete blackout of load and UPQC
            svA, svB, svC = 0.0, 0.0, 0.0
            lcA, lcB, lcC = 0.0, 0.0, 0.0
            inj_cA, inj_cB, inj_cC = 0.0, 0.0, 0.0
            inj_vA, inj_vB, inj_vC = 0.0, 0.0, 0.0
            actual_dc = max(actual_dc, 35.0 * effective_modules) # Decays to solar open circuit voltage
        else:
            # Normal DC Link voltage ripple based on capacitance and AC power
            ripple = (p_mw * 1000) / (params.dcCapacitance * 1e-6 * 2 * omega) * np.sin(2 * omega * time_sec)
            actual_dc += ripple
        
        actual_solar_voltage = max(mppt_v_ref, 0.1)

        dp = SimulationDataPoint(
            time=round(time_sec, 4),
            gridVoltageA=round(gvA, 2),
            gridVoltageB=round(gvB, 2),
            gridVoltageC=round(gvC, 2),
            gridCurrentA=round(gcA, 2),
            gridCurrentB=round(gcB, 2),
            gridCurrentC=round(gcC, 2),
            supplyVoltageA=round(svA, 2),
            supplyVoltageB=round(svB, 2),
            supplyVoltageC=round(svC, 2),
            loadCurrentA=round(lcA, 2),
            loadCurrentB=round(lcB, 2),
            loadCurrentC=round(lcC, 2),
            injectingVoltageA=round(inj_vA, 2),
            injectingVoltageB=round(inj_vB, 2),
            injectingVoltageC=round(inj_vC, 2),
            injectingCurrentA=round(inj_cA, 2),
            injectingCurrentB=round(inj_cB, 2),
            injectingCurrentC=round(inj_cC, 2),
            dcLinkVoltage=round(actual_dc, 2),
            solarPowerWatts=round(dynamic_solar_power, 2),
            solarVoltageDc=round(actual_solar_voltage, 2),
            solarCurrentDc=round(dynamic_solar_power / actual_solar_voltage, 2),
            solarIrradiance=round(current_irradiance, 2),
            solarTemperature=round(params.solarTemperature, 2),
            batterySOC=round(current_soc, 6),
            windPowerWatts=round(wind_power_total, 2)
        )
        data_points.append(dp)
        
    return data_points
