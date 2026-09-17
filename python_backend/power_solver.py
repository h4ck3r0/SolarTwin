import numpy as np
from pydantic import BaseModel
from typing import List

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
    solarVmpp: float = 34.1
    batterySOC: float = 80.0
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
    windSpeed: float = 8.0
    windCutIn: float = 3.0
    windCutOut: float = 25.0
    windNominalPower: float = 50.0

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
    gridVoltageA: float; gridVoltageB: float; gridVoltageC: float
    gridCurrentA: float; gridCurrentB: float; gridCurrentC: float
    supplyVoltageA: float; supplyVoltageB: float; supplyVoltageC: float
    loadCurrentA: float; loadCurrentB: float; loadCurrentC: float
    injectingVoltageA: float; injectingVoltageB: float; injectingVoltageC: float
    injectingCurrentA: float; injectingCurrentB: float; injectingCurrentC: float
    dcLinkVoltage: float
    solarPowerWatts: float; solarVoltageDc: float; solarCurrentDc: float
    solarIrradiance: float; solarTemperature: float
    batterySOC: float; windPowerWatts: float
    igbtTemperature: float = 25.0


def _compute_wind_power(wind_speed, cut_in, cut_out, nominal_kw, steps):
    arr = np.zeros(steps)
    if cut_in <= 0 or cut_out <= cut_in or nominal_kw <= 0:
        return arr
    if wind_speed < cut_in or wind_speed >= cut_out:
        return arr
    v_rated = cut_out * 0.6
    frac = 1.0 if wind_speed >= v_rated else ((wind_speed - cut_in) / (v_rated - cut_in)) ** 3
    arr[:] = nominal_kw * 1000.0 * frac
    return arr


def _parse_profile(profile_str, default_val):
    points = []
    try:
        for seg in profile_str.split(','):
            seg = seg.strip()
            if ':' in seg:
                t_s, v_s = seg.split(':', 1)
                points.append((float(t_s.strip()), float(v_s.strip())))
        points.sort(key=lambda x: x[0])
    except Exception:
        pass
    if not points:
        points = [(0.0, default_val)]
    return points


def _interp(points, t):
    if t <= points[0][0]:
        return points[0][1]
    if t >= points[-1][0]:
        return points[-1][1]
    for i in range(len(points) - 1):
        t0, v0 = points[i]; t1, v1 = points[i+1]
        if t0 <= t <= t1:
            return v0 + (v1 - v0) * (t - t0) / (t1 - t0)
    return points[-1][1]


def emt_solver_loop(steps, dt, omega, v_base_peak,
                    load_kw, pf_load, thd_factor, h_mags,
                    L_filter, C_dc, kp, ki, dc_ref, is_grid_connected,
                    solar_power_array, wind_power_array,
                    battery_capacity_wh, initial_soc,
                    r_grid, x_grid):
    time_arr = np.zeros(steps)
    v_grid   = np.zeros((3, steps)); i_grid = np.zeros((3, steps))
    v_load   = np.zeros((3, steps)); i_load = np.zeros((3, steps))
    v_inj    = np.zeros((3, steps)); i_inj  = np.zeros((3, steps))
    v_dc_arr = np.zeros(steps);  bsoc_arr = np.zeros(steps)

    v_dc = dc_ref; battery_soc = initial_soc; v_dc_int = 0.0
    h3, h5, h7, h11, h13 = h_mags
    pA, pB, pC = 0.0, -2*np.pi/3, 2*np.pi/3

    v_ll_rms = v_base_peak * np.sqrt(3) / np.sqrt(2)
    s_va     = load_kw * 1000.0 / max(pf_load, 0.01)
    i_rms    = s_va / (np.sqrt(3) * v_ll_rms)
    i_pk     = i_rms * np.sqrt(2)
    phi      = np.arccos(np.clip(pf_load, 0.0, 1.0))

    for k in range(steps):
        t = k * dt
        time_arr[k] = t
        th = omega * t      # PLL angle (ideal lock)

        # Grid ideal source
        if is_grid_connected:
            vs_a = v_base_peak * np.sin(th + pA)
            vs_b = v_base_peak * np.sin(th + pB)
            vs_c = v_base_peak * np.sin(th + pC)
        else:
            vs_a = vs_b = vs_c = 0.0

        # Load currents with harmonics
        def load_i(ph):
            f = i_pk * np.sin(th + ph - phi)
            f += thd_factor * i_pk * (
                h5  * np.sin(5  * (th + ph)) +
                h7  * np.sin(7  * (th + ph)) +
                h3  * np.sin(3  * (th + ph)) +
                h11 * np.sin(11 * (th + ph)) +
                h13 * np.sin(13 * (th + ph)))
            return f

        ila, ilb, ilc = load_i(pA), load_i(pB), load_i(pC)
        i_load[0,k]=ila; i_load[1,k]=ilb; i_load[2,k]=ilc

        # UPQC shunt: inject harmonics back
        fund_a = i_pk * np.sin(th + pA - phi)
        fund_b = i_pk * np.sin(th + pB - phi)
        fund_c = i_pk * np.sin(th + pC - phi)
        ce = np.clip(kp * 0.1 + ki * 0.01, 0.0, 1.0)
        i_inj[0,k] = -(ila - fund_a) * ce
        i_inj[1,k] = -(ilb - fund_b) * ce
        i_inj[2,k] = -(ilc - fund_c) * ce

        # Generation injection (solar + wind)
        p_gen  = float(solar_power_array[k]) + float(wind_power_array[k])
        ig_pk  = p_gen / (1.5 * v_base_peak) if v_base_peak > 0 else 0.0

        # KCL: grid current = load - shunt_injection - generation
        ig_a = ila + i_inj[0,k] - ig_pk * np.sin(th + pA)
        ig_b = ilb + i_inj[1,k] - ig_pk * np.sin(th + pB)
        ig_c = ilc + i_inj[2,k] - ig_pk * np.sin(th + pC)

        # Thevenin: terminal voltage drop across grid impedance (R component)
        vg_a = vs_a - r_grid * ig_a
        vg_b = vs_b - r_grid * ig_b
        vg_c = vs_c - r_grid * ig_c

        v_grid[0,k]=vg_a; v_grid[1,k]=vg_b; v_grid[2,k]=vg_c
        i_grid[0,k]=ig_a; i_grid[1,k]=ig_b; i_grid[2,k]=ig_c

        # UPQC series: compensate sag to restore load voltage to nominal
        ref_a = v_base_peak * np.sin(th + pA)
        ref_b = v_base_peak * np.sin(th + pB)
        ref_c = v_base_peak * np.sin(th + pC)
        v_inj[0,k] = ref_a - vg_a
        v_inj[1,k] = ref_b - vg_b
        v_inj[2,k] = ref_c - vg_c
        v_load[0,k] = vg_a + v_inj[0,k]
        v_load[1,k] = vg_b + v_inj[1,k]
        v_load[2,k] = vg_c + v_inj[2,k]

        # DC-link power balance
        p_series = v_inj[0,k]*ig_a + v_inj[1,k]*ig_b + v_inj[2,k]*ig_c
        p_shunt  = vg_a*i_inj[0,k] + vg_b*i_inj[1,k] + vg_c*i_inj[2,k]
        p_upqc   = p_series + p_shunt

        # Battery PI to regulate Vdc
        err = dc_ref - v_dc
        v_dc_int += err * dt
        p_bat = 0.0
        if battery_capacity_wh > 0.0 and battery_soc > 2.0:
            p_bat = 2000.0 * err + 500.0 * v_dc_int
            p_bat = max(min(p_bat, battery_capacity_wh * 2.0), -battery_capacity_wh * 2.0)
            battery_soc -= (p_bat * dt) / (battery_capacity_wh * 3600.0) * 100.0
            battery_soc  = max(0.0, battery_soc)
        bsoc_arr[k] = battery_soc

        # Capacitor ODE
        if v_dc > 10.0:
            v_dc += ((p_bat - p_upqc) / (C_dc * v_dc)) * dt
        v_dc = max(v_dc, 0.0)
        v_dc_arr[k] = v_dc

    return time_arr, v_grid, i_grid, v_load, i_load, v_inj, i_inj, v_dc_arr, bsoc_arr


def run_simulation(req: SimulationRequest):
    params = req.parameters
    duration = max(float(params.simulationDuration), 0.1)
    dt = 1e-4
    steps = min(int(duration / dt), 30000)

    omega = 2.0 * np.pi * params.microgridFrequency
    v_base_peak = (params.microgridVoltage / np.sqrt(3)) * np.sqrt(2)

    htype = (params.loadHarmonicType or "Rectifier").lower()
    h3=h5=h7=h11=h13=0.0
    if   "vfd" in htype: h5,h7 = 0.7,0.3
    elif "arc" in htype: h3,h5,h7 = 0.6,0.3,0.1
    elif "clean" in htype: pass
    else: h5,h7,h11,h13 = 0.7,0.5,0.2,0.1
    h_mags = np.array([h3,h5,h7,h11,h13])

    irrad_pts = _parse_profile(params.irradianceProfile, params.solarIrradiance)
    temp_pts  = _parse_profile(params.temperatureProfile, params.solarTemperature)

    # ── Aggregate ALL solar nodes from topology ──────────────────────────────────
    # Each solar node has its own strings/modules/watts/vmpp but shares global sky conditions
    solar_panels = []
    is_solar_node = lambda n: any(kw in (n.type + " " + n.data.label).lower()
                                  for kw in ['solar', 'pv', 'microgrid', 'photovoltaic'])
    
    for node in req.topology.nodes:
        if is_solar_node(node):
            np_dict = node.data.parameters or {}
            # Per-panel config: fall back to global params if not set on node
            panel = {
                'strings':  int(float(np_dict.get('solarStringsParallel', params.solarStringsParallel))),
                'modules':  int(float(np_dict.get('solarModulesSeries',   params.solarModulesSeries))),
                'watts':    float(np_dict.get('solarPanelWatts', params.solarPanelWatts)),
                'vmpp':     float(np_dict.get('solarVmpp',       params.solarVmpp)),
            }
            solar_panels.append(panel)

    # If no solar nodes in topology, fall back to global params (default array)
    if not solar_panels:
        solar_panels = [{
            'strings': params.solarStringsParallel,
            'modules':  params.solarModulesSeries,
            'watts':    params.solarPanelWatts,
            'vmpp':     params.solarVmpp,
        }]

    solar_pwr = np.zeros(steps); solar_v = np.zeros(steps)
    solar_i   = np.zeros(steps); irr_arr = np.zeros(steps)
    tmp_arr   = np.zeros(steps)

    for k in range(steps):
        t  = k * dt
        G  = _interp(irrad_pts, t)   # same irradiance for all panels
        T  = _interp(temp_pts,  t)   # same temperature for all panels
        dT = T - 25.0
        irr_arr[k] = G; tmp_arr[k] = T

        total_pwr = 0.0
        total_v   = 0.0
        total_i   = 0.0

        for panel in solar_panels:
            vmpp_m = panel['vmpp']
            impp_m = panel['watts'] / vmpp_m if vmpp_m > 0 else 0.0

            # Temperature & irradiance derating (IEC 61215)
            Vm = vmpp_m * (1.0 - 0.003 * dT)
            Im = impp_m * (G / 1000.0) * (1.0 + 0.0005 * dT)
            Vm = max(Vm, 1.0); Im = max(Im, 0.0)

            # Array topology: series→voltage, parallel→current
            Va = Vm * panel['modules']
            Ia = Im * panel['strings']
            total_pwr += Va * Ia
            # Aggregate voltage = max array voltage (parallel arrays share bus)
            total_v = max(total_v, Va)
            total_i += Ia   # parallel arrays sum currents

        solar_pwr[k] = total_pwr
        solar_v[k]   = total_v
        solar_i[k]   = total_i

    # ── Aggregate wind turbine nodes from topology ──────────────────────────────
    total_wind_power_kw = 0.0
    has_wind_node = False
    for node in req.topology.nodes:
        node_id_label = (node.id + " " + node.data.label).lower()
        if any(kw in node_id_label for kw in ['wind', 'turbine']):
            has_wind_node = True
            np_dict = node.data.parameters or {}
            ws  = float(np_dict.get('windSpeed',        params.windSpeed))
            ci  = float(np_dict.get('windCutIn',        params.windCutIn))
            co  = float(np_dict.get('windCutOut',       params.windCutOut))
            kw  = float(np_dict.get('windNominalPower', params.windNominalPower))
            # Add power contribution of this turbine
            wind_arr_tmp = _compute_wind_power(ws, ci, co, kw, steps)
            total_wind_power_kw += wind_arr_tmp[0]  # store kW, convert after

    if has_wind_node:
        wind_pwr = _compute_wind_power(
            params.windSpeed, params.windCutIn, params.windCutOut, 0.0, steps)
        wind_pwr[:] = total_wind_power_kw  # Already in W from _compute_wind_power
        # Re-run properly summing all turbines
        wind_pwr = np.zeros(steps)
        for node in req.topology.nodes:
            node_id_label = (node.id + " " + node.data.label).lower()
            if any(kw in node_id_label for kw in ['wind', 'turbine']):
                np_dict = node.data.parameters or {}
                ws = float(np_dict.get('windSpeed',        params.windSpeed))
                ci = float(np_dict.get('windCutIn',        params.windCutIn))
                co = float(np_dict.get('windCutOut',       params.windCutOut))
                kw = float(np_dict.get('windNominalPower', params.windNominalPower))
                wind_pwr += _compute_wind_power(ws, ci, co, kw, steps)
    else:
        # No wind node connected → zero wind contribution
        wind_pwr = np.zeros(steps)

    # ── Aggregate battery capacity from topology ─────────────────────────────────
    total_battery_kwh = 0.0
    effective_battery_soc = params.batterySOC
    for node in req.topology.nodes:
        node_id_label = (node.id + " " + node.data.label).lower()
        if any(kw in node_id_label for kw in ['battery', 'bess', 'storage']):
            np_dict = node.data.parameters or {}
            total_battery_kwh += float(np_dict.get('batteryCapacityKwh', params.batteryCapacityKwh))
            effective_battery_soc = float(np_dict.get('batterySOC', params.batterySOC))

    if total_battery_kwh == 0.0:
        # No battery node on canvas → use global parameter panel values.
        # The DC link capacitor itself has inherent charge; global battery params
        # represent the UPQC's internal energy buffer even without an explicit battery node.
        total_battery_kwh = params.batteryCapacityKwh
        effective_battery_soc = params.batterySOC

    (time_arr, v_grid, i_grid, v_load, i_load,
     v_inj, i_inj, v_dc_arr, bsoc_arr) = emt_solver_loop(
        steps, dt, omega, v_base_peak,
        params.loadActivePower, params.loadPowerFactor,
        params.loadTHD / 100.0, h_mags,
        params.filterInductance * 1e-3,
        params.dcCapacitance  * 1e-6,
        params.kp, params.ki,
        params.dcLinkVoltage,
        params.isGridConnected,
        solar_pwr, wind_pwr,
        params.batteryCapacityKwh * 1000.0,
        params.batterySOC,
        params.gridResistance,
        params.gridReactance,
    )

    thermal_net = IGBT_Thermal_Model(T_ambient=params.solarTemperature)
    sub = max(1, steps // 2000)
    data_points: List[SimulationDataPoint] = []

    for k in range(0, steps, sub):
        i_sh_rms = float(np.sqrt((i_inj[0,k]**2+i_inj[1,k]**2+i_inj[2,k]**2)/3.0))
        p_loss   = thermal_net.compute_losses(i_sh_rms, float(v_dc_arr[k]), 10000.0)
        tj       = thermal_net.step(p_loss, dt * sub)

        dp = SimulationDataPoint(
            time=round(float(time_arr[k]),6),
            gridVoltageA=round(float(v_grid[0,k]),2),
            gridVoltageB=round(float(v_grid[1,k]),2),
            gridVoltageC=round(float(v_grid[2,k]),2),
            gridCurrentA=round(float(i_grid[0,k]),2),
            gridCurrentB=round(float(i_grid[1,k]),2),
            gridCurrentC=round(float(i_grid[2,k]),2),
            supplyVoltageA=round(float(v_load[0,k]),2),
            supplyVoltageB=round(float(v_load[1,k]),2),
            supplyVoltageC=round(float(v_load[2,k]),2),
            loadCurrentA=round(float(i_load[0,k]),2),
            loadCurrentB=round(float(i_load[1,k]),2),
            loadCurrentC=round(float(i_load[2,k]),2),
            injectingVoltageA=round(float(v_inj[0,k]),2),
            injectingVoltageB=round(float(v_inj[1,k]),2),
            injectingVoltageC=round(float(v_inj[2,k]),2),
            injectingCurrentA=round(float(i_inj[0,k]),2),
            injectingCurrentB=round(float(i_inj[1,k]),2),
            injectingCurrentC=round(float(i_inj[2,k]),2),
            dcLinkVoltage=round(float(v_dc_arr[k]),2),
            solarPowerWatts=round(float(solar_pwr[k]),2),
            solarVoltageDc=round(float(solar_v[k]),2),
            solarCurrentDc=round(float(solar_i[k]),2),
            solarIrradiance=round(float(irr_arr[k]),2),
            solarTemperature=round(float(tmp_arr[k]),2),
            batterySOC=round(float(bsoc_arr[k]),4),
            windPowerWatts=round(float(wind_pwr[k]),2),
            igbtTemperature=round(float(tj),2),
        )
        data_points.append(dp)

    import json
    try:
        with open("full_simulation_results.json","w") as f:
            json.dump([dp.dict() for dp in data_points], f)
    except Exception:
        pass

    return data_points[:500]
