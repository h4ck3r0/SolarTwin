import os
import json
import numpy as np
from pydantic import BaseModel
from typing import List, Optional

from control_algorithms import (
    clarke_transform, park_transform,
    inv_park_transform, inv_clarke_transform,
    srf_pll, pq_theory_harmonic_extraction,
)
from thermal_model import IGBT_Thermal_Model

# ─────────────────────────────────────────────────────────────────────────────
# Pydantic Models
# ─────────────────────────────────────────────────────────────────────────────

class SimulationParameters(BaseModel):
    microgridVoltage: float = 415.0
    microgridFrequency: float = 50.0
    solarIrradiance: float = 1000.0
    solarTemperature: float = 25.0
    ambientTemperature: float = 35.0        # FIX BUG-05: outdoor air temp for IGBT heatsink
    solarStringsParallel: int = 88
    solarModulesSeries: int = 7
    solarPanelWatts: float = 415.0
    solarVmpp: float = 34.1
    mpptAlgorithm: str = "PandO"            # "PandO", "INC", "Fixed"
    mpptStepSize: float = 0.5              # V — perturbation step for P&O
    mpptUpdateFreq: int = 100              # steps between MPPT perturbations
    batterySOC: float = 80.0
    batteryCapacityKwh: float = 100.0
    dcLinkVoltage: float = 700.0
    isGridConnected: bool = True
    simulationDuration: float = 0.300
    irradianceProfile: str = "0:1000"
    temperatureProfile: str = "0:25"
    gridResistance: float = 0.1
    gridReactance: float = 0.2             # FIX: now actually used in EMT loop
    loadActivePower: float = 15.0
    loadPowerFactor: float = 0.85
    loadHarmonicType: str = "Rectifier"
    filterInductance: float = 2.5
    dcCapacitance: float = 2200.0
    loadTHD: float = 28.0
    kp: float = 0.5                        # FIX BUG-03: now wired into PI controllers
    ki: float = 10.0
    isTripped: bool = False
    windSpeed: float = 8.0
    windCutIn: float = 3.0
    windCutOut: float = 25.0
    windNominalPower: float = 50.0
    windInertiaTimeConst: float = 3.0      # seconds — rotor inertia lag constant


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


# FIX BUG-06: expand semicoloned fields to proper multi-line Pydantic declarations
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
    batterySOC: Optional[float] = None  # None = battery disconnected
    windPowerWatts: float
    igbtTemperature: float = 25.0


# ─────────────────────────────────────────────────────────────────────────────
# MPPT — Perturb & Observe
# ─────────────────────────────────────────────────────────────────────────────

class MPPTState:
    """Per-panel P&O MPPT tracker."""
    def __init__(self, vmpp_init: float, step: float = 0.5, freq: int = 100):
        self.v_ref = float(vmpp_init)
        self.step = float(step)
        self.freq = int(freq)
        self.p_prev = 0.0
        self.v_prev = float(vmpp_init)
        self.counter = 0

    def perturb_observe(self, v: float, p: float) -> float:
        self.counter += 1
        if self.counter < self.freq:
            return self.v_ref
        self.counter = 0
        dp = p - self.p_prev
        dv = v - self.v_prev
        if dv != 0.0:
            if (dp > 0 and dv > 0) or (dp < 0 and dv < 0):
                self.v_ref += self.step
            else:
                self.v_ref -= self.step
        self.p_prev = p
        self.v_prev = v
        return max(self.v_ref, 1.0)


# ─────────────────────────────────────────────────────────────────────────────
# Wind — Static power curve helper
# ─────────────────────────────────────────────────────────────────────────────

def _wind_curve_power_w(wind_speed: float, cut_in: float, cut_out: float,
                        nominal_kw: float) -> float:
    """Return steady-state power in Watts for given wind speed."""
    if cut_in <= 0 or cut_out <= cut_in or nominal_kw <= 0:
        return 0.0
    if wind_speed < cut_in or wind_speed >= cut_out:
        return 0.0
    v_rated = cut_out * 0.6
    frac = 1.0 if wind_speed >= v_rated else ((wind_speed - cut_in) / (v_rated - cut_in)) ** 3
    return nominal_kw * 1000.0 * frac


def _compute_wind_power_dynamic(wind_speed: float, cut_in: float, cut_out: float,
                                 nominal_kw: float, steps: int, dt: float,
                                 tau: float = 3.0) -> np.ndarray:
    """
    Wind turbine power with first-order rotor inertia lag.
    tau = rotor inertia time constant (seconds).
    """
    arr = np.zeros(steps)
    p_target = _wind_curve_power_w(wind_speed, cut_in, cut_out, nominal_kw)
    p = 0.0
    alpha = dt / (tau + dt)  # discrete first-order lag coefficient
    for k in range(steps):
        p = p + alpha * (p_target - p)  # first-order lag: p → p_target
        arr[k] = max(0.0, p)
    return arr


# ─────────────────────────────────────────────────────────────────────────────
# Profile helpers
# ─────────────────────────────────────────────────────────────────────────────

def _parse_profile(profile_str: str, default_val: float):
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


def _interp(points, t: float) -> float:
    if t <= points[0][0]:
        return points[0][1]
    if t >= points[-1][0]:
        return points[-1][1]
    for i in range(len(points) - 1):
        t0, v0 = points[i]
        t1, v1 = points[i + 1]
        if t0 <= t <= t1:
            return v0 + (v1 - v0) * (t - t0) / (t1 - t0)
    return points[-1][1]


# ─────────────────────────────────────────────────────────────────────────────
# RK4 helpers for state variables
# ─────────────────────────────────────────────────────────────────────────────

def _rk4_vdc(v_dc: float, p_net: float, C_dc: float, dt: float) -> float:
    """4th-order Runge-Kutta integration for DC-link capacitor voltage."""
    def f(v):
        return p_net / (C_dc * max(abs(v), 1.0)) * (1.0 if v >= 0 else -1.0)
    k1 = f(v_dc)
    k2 = f(v_dc + 0.5 * dt * k1)
    k3 = f(v_dc + 0.5 * dt * k2)
    k4 = f(v_dc + dt * k3)
    return v_dc + (dt / 6.0) * (k1 + 2 * k2 + 2 * k3 + k4)


# ─────────────────────────────────────────────────────────────────────────────
# Core EMT Solver
# ─────────────────────────────────────────────────────────────────────────────

def emt_solver_loop(steps, dt, omega, v_base_peak,
                    load_kw, pf_load, thd_factor, h_mags,
                    L_filter, C_dc, kp, ki, dc_ref, is_grid_connected,
                    solar_power_array, wind_power_array,
                    battery_capacity_wh, initial_soc,
                    r_grid, x_grid):
    """
    3-phase EMT solver for UPQC microgrid.
    Fixed: BUG-03 (kp/ki wired into PI), BUG-13 (cold-start), x_grid reactive drop, RK4 v_dc.
    FIX BUG-B01: Battery SOC now driven by solar surplus, not DC-link error.
    """
    time_arr = np.zeros(steps)
    v_grid   = np.zeros((3, steps))
    i_grid   = np.zeros((3, steps))
    v_load   = np.zeros((3, steps))
    i_load   = np.zeros((3, steps))
    v_inj    = np.zeros((3, steps))
    i_inj    = np.zeros((3, steps))
    v_dc_arr = np.zeros(steps)
    bsoc_arr = np.zeros(steps)

    v_dc = dc_ref
    battery_soc = initial_soc
    v_dc_int = 0.0                  # DC-link PI integral state

    # FIX BUG-03: shunt PI integral state for dynamic compensation efficiency
    shunt_err_int = 0.0

    # FIX BUG-B01: track battery connected state
    battery_connected = battery_capacity_wh > 0.0 and initial_soc >= 0.0

    h3, h5, h7, h11, h13 = h_mags
    pA, pB, pC = 0.0, -2 * np.pi / 3, 2 * np.pi / 3

    v_ll_rms = v_base_peak * np.sqrt(3) / np.sqrt(2)
    s_va     = load_kw * 1000.0 / max(pf_load, 0.01)
    i_rms    = s_va / (np.sqrt(3) * v_ll_rms)
    i_pk     = i_rms * np.sqrt(2)
    phi      = np.arccos(np.clip(pf_load, 0.0, 1.0))

    # FIX: track previous grid currents for reactive (inductive) voltage drop
    ig_prev = np.zeros(3)

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
        i_load[0, k] = ila
        i_load[1, k] = ilb
        i_load[2, k] = ilc

        # Fundamental components
        fund_a = i_pk * np.sin(th + pA - phi)
        fund_b = i_pk * np.sin(th + pB - phi)
        fund_c = i_pk * np.sin(th + pC - phi)

        # FIX BUG-03: dynamic shunt PI compensation efficiency
        harmonic_err = np.sqrt(
            (ila - fund_a)**2 + (ilb - fund_b)**2 + (ilc - fund_c)**2
        )
        shunt_err_int += harmonic_err * dt
        # PI-based compensation efficiency (clamped 0..1)
        ce = np.clip(kp * harmonic_err * 0.05 + ki * shunt_err_int * 0.001, 0.0, 1.0)

        i_inj[0, k] = -(ila - fund_a) * ce
        i_inj[1, k] = -(ilb - fund_b) * ce
        i_inj[2, k] = -(ilc - fund_c) * ce

        # Generation injection (solar + wind)
        p_gen = float(solar_power_array[k]) + float(wind_power_array[k])
        ig_pk = p_gen / (1.5 * v_base_peak) if v_base_peak > 0 else 0.0

        # KCL: grid current = load - shunt_injection - generation
        ig_a = ila + i_inj[0, k] - ig_pk * np.sin(th + pA)
        ig_b = ilb + i_inj[1, k] - ig_pk * np.sin(th + pB)
        ig_c = ilc + i_inj[2, k] - ig_pk * np.sin(th + pC)

        # FIX PHYSICS: Thevenin RL drop
        # x_grid is REACTANCE (X = ωL) in Ohms, not inductance.
        # Time-domain inductive drop = L·di/dt = (X/ω)·di/dt
        L_grid = x_grid / omega   # convert reactance → inductance (H)
        # Skip derivative for first 2 steps to avoid init transient spikes
        if k > 1:
            dig_a = (ig_a - ig_prev[0]) / dt
            dig_b = (ig_b - ig_prev[1]) / dt
            dig_c = (ig_c - ig_prev[2]) / dt
        else:
            dig_a = dig_b = dig_c = 0.0
        vg_a = vs_a - r_grid * ig_a - L_grid * dig_a
        vg_b = vs_b - r_grid * ig_b - L_grid * dig_b
        vg_c = vs_c - r_grid * ig_c - L_grid * dig_c
        ig_prev[:] = [ig_a, ig_b, ig_c]

        v_grid[0, k] = vg_a
        v_grid[1, k] = vg_b
        v_grid[2, k] = vg_c
        i_grid[0, k] = ig_a
        i_grid[1, k] = ig_b
        i_grid[2, k] = ig_c

        # UPQC series: compensate sag/swell to restore load voltage to nominal
        ref_a = v_base_peak * np.sin(th + pA)
        ref_b = v_base_peak * np.sin(th + pB)
        ref_c = v_base_peak * np.sin(th + pC)
        v_inj[0, k] = ref_a - vg_a
        v_inj[1, k] = ref_b - vg_b
        v_inj[2, k] = ref_c - vg_c
        v_load[0, k] = vg_a + v_inj[0, k]
        v_load[1, k] = vg_b + v_inj[1, k]
        v_load[2, k] = vg_c + v_inj[2, k]

        # DC-link power balance
        p_series = v_inj[0, k]*ig_a + v_inj[1, k]*ig_b + v_inj[2, k]*ig_c
        p_shunt  = vg_a*i_inj[0, k] + vg_b*i_inj[1, k] + vg_c*i_inj[2, k]
        p_upqc   = p_series + p_shunt

        # FIX BUG-B01: Battery charge/discharge driven by real solar surplus, not DC-link error.
        # p_surplus > 0 → solar exceeds load → charge battery
        # p_surplus < 0 → deficit → discharge battery to cover load
        p_surplus = float(solar_power_array[k]) + float(wind_power_array[k]) - (load_kw * 1000.0)
        p_bat = 0.0
        if battery_connected and battery_soc > 2.0:
            # Charge rate limited to 1C (battery_capacity_wh Wh → battery_capacity_wh W)
            max_charge_rate = battery_capacity_wh  # 1C rate in Watts
            p_bat = np.clip(p_surplus, -max_charge_rate, max_charge_rate)
            # Discharge stops at SOC <= 2%, charge stops at SOC >= 99%
            if p_bat < 0 and battery_soc <= 2.0:
                p_bat = 0.0
            if p_bat > 0 and battery_soc >= 99.0:
                p_bat = 0.0
            # SOC update: positive p_bat = charging (SOC increases)
            battery_soc += (p_bat * dt) / (battery_capacity_wh * 36.0)  # *36 = 3600s * 100%
            battery_soc = np.clip(battery_soc, 0.0, 100.0)
        bsoc_arr[k] = battery_soc

        # DC-link PI keeps bus voltage — uses UPQC consumption and battery feedforward
        # Gains are scaled: kp in 0..100 range → physical scale ×2000 gives ~MW-class response
        err = dc_ref - v_dc
        v_dc_int += err * dt
        p_dc_regulation = (kp * 2000.0) * err + (ki * 50.0) * v_dc_int

        # FIX BUG-13: RK4 v_dc integration — regularized denominator, no hard skip
        p_net = p_dc_regulation - p_upqc
        v_dc = _rk4_vdc(v_dc, p_net, C_dc, dt)
        v_dc = max(v_dc, 0.0)
        v_dc_arr[k] = v_dc

    return time_arr, v_grid, i_grid, v_load, i_load, v_inj, i_inj, v_dc_arr, bsoc_arr


# ─────────────────────────────────────────────────────────────────────────────
# Main simulation entry point
# ─────────────────────────────────────────────────────────────────────────────

def run_simulation(req: SimulationRequest):
    params = req.parameters
    duration = max(float(params.simulationDuration), 0.1)
    dt = 1e-4
    steps = min(int(duration / dt), 30000)

    omega = 2.0 * np.pi * params.microgridFrequency
    v_base_peak = (params.microgridVoltage / np.sqrt(3)) * np.sqrt(2)

    htype = (params.loadHarmonicType or "Rectifier").lower()
    h3 = h5 = h7 = h11 = h13 = 0.0
    if   "vfd" in htype:   h5, h7 = 0.7, 0.3
    elif "arc" in htype:   h3, h5, h7 = 0.6, 0.3, 0.1
    elif "clean" in htype: pass
    else:                  h5, h7, h11, h13 = 0.7, 0.5, 0.2, 0.1
    h_mags = np.array([h3, h5, h7, h11, h13])

    irrad_pts = _parse_profile(params.irradianceProfile, params.solarIrradiance)
    temp_pts  = _parse_profile(params.temperatureProfile, params.solarTemperature)

    # ── Collect connected node IDs ──────────────────────────────────────────
    connected_node_ids: set = set()
    for edge in req.topology.edges:
        connected_node_ids.add(edge.source)
        connected_node_ids.add(edge.target)

    # ── Aggregate solar nodes from topology ─────────────────────────────────
    is_solar_node = lambda n: any(
        kw in (n.type + " " + n.data.label).lower()
        for kw in ['solar', 'pv', 'microgrid', 'photovoltaic']
    )

    solar_panels = []
    for node in req.topology.nodes:
        if not is_solar_node(node):
            continue
        if node.id not in connected_node_ids:
            continue
        np_dict = node.data.parameters or {}
        if np_dict.get('isTripped') in [True, 'true', 'True', 1, '1']:
            continue
        solar_panels.append({
            'strings': int(float(np_dict.get('solarStringsParallel', params.solarStringsParallel))),
            'modules': int(float(np_dict.get('solarModulesSeries',   params.solarModulesSeries))),
            'watts':   float(np_dict.get('solarPanelWatts', params.solarPanelWatts)),
            'vmpp':    float(np_dict.get('solarVmpp',       params.solarVmpp)),
        })

    # FIX BUG-B02: if no solar nodes matched the topology labels, fall back to
    # the global params so solar power is never silently zeroed out.
    if not solar_panels:
        solar_panels.append({
            'strings': int(params.solarStringsParallel),
            'modules': int(params.solarModulesSeries),
            'watts':   float(params.solarPanelWatts),
            'vmpp':    float(params.solarVmpp),
        })

    # Create one MPPT tracker per panel (P&O algorithm)
    use_mppt = params.mpptAlgorithm.strip().upper() not in ("FIXED", "")
    mppt_trackers = [
        MPPTState(p['vmpp'] * p['modules'], step=params.mpptStepSize, freq=params.mpptUpdateFreq)
        for p in solar_panels
    ]

    solar_pwr = np.zeros(steps)
    solar_v   = np.zeros(steps)
    solar_i   = np.zeros(steps)
    irr_arr   = np.zeros(steps)
    tmp_arr   = np.zeros(steps)

    for k in range(steps):
        t = k * dt
        G  = _interp(irrad_pts, t)
        T  = _interp(temp_pts,  t)
        dT = T - 25.0
        irr_arr[k] = G
        tmp_arr[k] = T

        total_pwr = 0.0
        total_v   = 0.0
        total_i   = 0.0

        for idx, panel in enumerate(solar_panels):
            vmpp_m = panel['vmpp']
            impp_m = panel['watts'] / vmpp_m if vmpp_m > 0 else 0.0

            # Temperature & irradiance derating (IEC 61215)
            Vm_stc = vmpp_m * (1.0 - 0.003 * dT)
            Im     = impp_m * (G / 1000.0) * (1.0 + 0.0005 * dT)
            Im     = max(Im, 0.0)

            # Array topology: series→voltage, parallel→current
            Va_stc = max(Vm_stc, 1.0) * panel['modules']
            Ia     = Im * panel['strings']
            P_stc  = Va_stc * Ia

            if use_mppt and k > 0:
                # P&O MPPT: tracker adjusts v_ref per step
                v_ref = mppt_trackers[idx].perturb_observe(Va_stc, P_stc)
                # Scale actual power to MPPT operating point (linear approx)
                Va = min(v_ref, Va_stc)
                Pa = Va * Ia  # simplified: current unchanged, voltage tracked
            else:
                Va = Va_stc
                Pa = P_stc

            total_pwr += Pa
            total_v = max(total_v, Va)  # parallel arrays share bus → max voltage
            total_i += Ia

        solar_pwr[k] = total_pwr
        solar_v[k]   = total_v
        solar_i[k]   = total_i

    # ── Aggregate wind turbine nodes (FIX BUG-01: single clean loop) ────────
    # FIX BUG-01: Removed the dead first-pass accumulation.
    # Only one loop now; wind_pwr is built directly.
    wind_pwr = np.zeros(steps)
    for node in req.topology.nodes:
        node_id_label = (node.id + " " + node.data.label).lower()
        if node.id not in connected_node_ids and not any(
            core in node.id.lower() for core in ['ac-bus', 'dc-link', 'series', 'shunt', 'upqc']
        ):
            continue
        if any(kw in node_id_label for kw in ['wind', 'turbine']):
            np_dict = node.data.parameters or {}
            if np_dict.get('isTripped') in [True, 'true', 'True', 1, '1']:
                continue
            ws  = float(np_dict.get('windSpeed',        params.windSpeed))
            ci  = float(np_dict.get('windCutIn',        params.windCutIn))
            co  = float(np_dict.get('windCutOut',       params.windCutOut))
            kw  = float(np_dict.get('windNominalPower', params.windNominalPower))
            tau = float(np_dict.get('windInertiaTimeConst', params.windInertiaTimeConst))
            # Phase 2: dynamic wind with rotor inertia
            wind_pwr += _compute_wind_power_dynamic(ws, ci, co, kw, steps, dt, tau)

    # ── Aggregate battery capacity (FIX BUG-02: capacity-weighted SOC) ──────
    total_battery_kwh = 0.0
    effective_battery_soc = params.batterySOC
    soc_capacity_pairs = []

    for node in req.topology.nodes:
        node_id_label = (node.id + " " + node.data.label).lower()
        if node.id not in connected_node_ids and not any(
            core in node.id.lower() for core in ['ac-bus', 'dc-link', 'series', 'shunt', 'upqc']
        ):
            continue
        if any(kw in node_id_label for kw in ['battery', 'bess', 'storage']):
            np_dict = node.data.parameters or {}
            if np_dict.get('isTripped') in [True, 'true', 'True', 1, '1']:
                continue
            cap = float(np_dict.get('batteryCapacityKwh', params.batteryCapacityKwh))
            soc = float(np_dict.get('batterySOC', params.batterySOC))
            total_battery_kwh += cap
            soc_capacity_pairs.append((soc, cap))

    # FIX BUG-B04 (revised): Three cases:
    # 1. Battery nodes found in topology → use capacity-weighted SOC
    # 2. No battery nodes in topology AND battery params are set → use global params (battery in circuit but not a canvas node)
    # 3. No battery nodes AND capacity = 0 → truly disconnected → return None
    if soc_capacity_pairs and total_battery_kwh > 0:
        effective_battery_soc = sum(s * c for s, c in soc_capacity_pairs) / total_battery_kwh
    elif not soc_capacity_pairs:
        if params.batteryCapacityKwh > 0:
            # No battery node in topology, but global params define one → use global params
            effective_battery_soc = float(params.batterySOC)
            total_battery_kwh = float(params.batteryCapacityKwh)
        else:
            # Explicitly no battery → None signals disconnect to frontend
            effective_battery_soc = None

    # ── Detect grid connection from topology ─────────────────────────────────
    # FIX BUG-B03: fallback to params.isGridConnected if no grid node found in topology
    actual_grid_connected = False
    found_grid_node = False
    for node in req.topology.nodes:
        if node.id not in connected_node_ids:
            continue
        if any(kw in (node.id + " " + node.data.label).lower() for kw in ['grid', 'main', 'source']):
            found_grid_node = True
            np_dict = node.data.parameters or {}
            if np_dict.get('isTripped') not in [True, 'true', 'True', 1, '1']:
                actual_grid_connected = True
                break
    if not found_grid_node:
        # No grid node in topology — fall back to global parameter
        actual_grid_connected = bool(params.isGridConnected)

    # ── Run EMT solver ───────────────────────────────────────────────────────
    (time_arr, v_grid, i_grid, v_load, i_load,
     v_inj, i_inj, v_dc_arr, bsoc_arr) = emt_solver_loop(
        steps, dt, omega, v_base_peak,
        params.loadActivePower, params.loadPowerFactor,
        params.loadTHD / 100.0, h_mags,
        params.filterInductance * 1e-3,
        params.dcCapacitance  * 1e-6,
        params.kp, params.ki,
        params.dcLinkVoltage,
        actual_grid_connected,
        solar_pwr, wind_pwr,
        total_battery_kwh * 1000.0,
        effective_battery_soc,
        params.gridResistance,
        params.gridReactance,
    )

    # FIX BUG-05: use ambientTemperature for IGBT heatsink, not solarTemperature
    thermal_net = IGBT_Thermal_Model(T_ambient=params.ambientTemperature)
    sub = max(1, steps // 2000)
    data_points: List[SimulationDataPoint] = []

    for k in range(0, steps, sub):
        i_sh_rms = float(np.sqrt((i_inj[0, k]**2 + i_inj[1, k]**2 + i_inj[2, k]**2) / 3.0))
        p_loss   = thermal_net.compute_losses(i_sh_rms, float(v_dc_arr[k]), 10000.0)
        tj       = thermal_net.step(p_loss, dt * sub)

        dp = SimulationDataPoint(
            time=round(float(time_arr[k]), 6),
            gridVoltageA=round(float(v_grid[0, k]), 2),
            gridVoltageB=round(float(v_grid[1, k]), 2),
            gridVoltageC=round(float(v_grid[2, k]), 2),
            gridCurrentA=round(float(i_grid[0, k]), 2),
            gridCurrentB=round(float(i_grid[1, k]), 2),
            gridCurrentC=round(float(i_grid[2, k]), 2),
            supplyVoltageA=round(float(v_load[0, k]), 2),
            supplyVoltageB=round(float(v_load[1, k]), 2),
            supplyVoltageC=round(float(v_load[2, k]), 2),
            loadCurrentA=round(float(i_load[0, k]), 2),
            loadCurrentB=round(float(i_load[1, k]), 2),
            loadCurrentC=round(float(i_load[2, k]), 2),
            injectingVoltageA=round(float(v_inj[0, k]), 2),
            injectingVoltageB=round(float(v_inj[1, k]), 2),
            injectingVoltageC=round(float(v_inj[2, k]), 2),
            injectingCurrentA=round(float(i_inj[0, k]), 2),
            injectingCurrentB=round(float(i_inj[1, k]), 2),
            injectingCurrentC=round(float(i_inj[2, k]), 2),
            dcLinkVoltage=round(float(v_dc_arr[k]), 2),
            solarPowerWatts=round(float(solar_pwr[k]), 2),
            solarVoltageDc=round(float(solar_v[k]), 2),
            solarCurrentDc=round(float(solar_i[k]), 2),
            solarIrradiance=round(float(irr_arr[k]), 2),
            solarTemperature=round(float(tmp_arr[k]), 2),
            # FIX BUG-B04: batterySOC is None when battery is disconnected
            batterySOC=round(float(bsoc_arr[k]), 4) if effective_battery_soc is not None else None,
            windPowerWatts=round(float(wind_pwr[k]), 2),
            igbtTemperature=round(float(tj), 2),
        )
        data_points.append(dp)

    # Persist full results to disk for post-processing / statistics page
    _root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    results_path = os.path.join(_root, "full_simulation_results.json")
    try:
        with open(results_path, "w") as f:
            # FIX BUG-17: use model_dump() (Pydantic v2 API)
            json.dump([dp.model_dump() for dp in data_points], f)
    except Exception:
        pass

    # FIX BUG-04: return ALL downsampled points (already ~2000), not a hard :500 slice
    return data_points
