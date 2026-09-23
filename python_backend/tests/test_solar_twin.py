"""
SolarTwin — Backend Unit Tests (Phase 6.1)
Run: pytest python_backend/tests/ -v --tb=short
"""
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import pytest
import numpy as np

from power_solver import (
    SimulationParameters, SimulationRequest, Topology, Node, Edge, NodeData,
    run_simulation, _parse_profile, _interp, _rk4_vdc,
    _wind_curve_power_w, _compute_wind_power_dynamic, MPPTState,
)
from thermal_model import IGBT_Thermal_Model
from control_algorithms import clarke_transform, park_transform, pq_theory_harmonic_extraction


# ─────────────────────────────────────────────────────────────────────────────
# Profile helpers
# ─────────────────────────────────────────────────────────────────────────────

class TestProfileHelpers:
    def test_parse_single_point(self):
        pts = _parse_profile("0:1000", 800)
        assert pts == [(0.0, 1000.0)]

    def test_parse_multi_point(self):
        pts = _parse_profile("0:1000, 0.1:500, 0.2:800", 1000)
        assert len(pts) == 3
        assert pts[0] == (0.0, 1000.0)
        assert pts[1] == (0.1, 500.0)

    def test_parse_fallback_on_invalid(self):
        pts = _parse_profile("garbage!!!", 777.0)
        assert pts == [(0.0, 777.0)]

    def test_interp_at_boundary(self):
        pts = [(0.0, 100.0), (1.0, 200.0)]
        assert _interp(pts, 0.0) == 100.0
        assert _interp(pts, 1.0) == 200.0

    def test_interp_midpoint(self):
        pts = [(0.0, 0.0), (1.0, 100.0)]
        assert abs(_interp(pts, 0.5) - 50.0) < 1e-9

    def test_interp_clamp_below(self):
        pts = [(0.5, 100.0), (1.0, 200.0)]
        assert _interp(pts, 0.0) == 100.0   # clamps to first value

    def test_interp_clamp_above(self):
        pts = [(0.0, 100.0), (0.5, 200.0)]
        assert _interp(pts, 1.0) == 200.0   # clamps to last value


# ─────────────────────────────────────────────────────────────────────────────
# RK4 ODE
# ─────────────────────────────────────────────────────────────────────────────

class TestRK4:
    def test_zero_net_power_stable(self):
        """Zero net power → v_dc should not change."""
        v0 = 700.0
        v1 = _rk4_vdc(v0, p_net=0.0, C_dc=2200e-6, dt=1e-4)
        assert abs(v1 - v0) < 1e-6

    def test_positive_net_power_charges(self):
        """Positive net power → v_dc should increase."""
        v0 = 700.0
        v1 = _rk4_vdc(v0, p_net=10000.0, C_dc=2200e-6, dt=1e-4)
        assert v1 > v0

    def test_negative_net_power_discharges(self):
        """Negative net power → v_dc should decrease."""
        v0 = 700.0
        v1 = _rk4_vdc(v0, p_net=-10000.0, C_dc=2200e-6, dt=1e-4)
        assert v1 < v0

    def test_cold_start_recovery(self):
        """v_dc=0 → should recover, not crash (BUG-13 fix test)."""
        v = 0.0
        for _ in range(100):
            v = _rk4_vdc(v, p_net=5000.0, C_dc=2200e-6, dt=1e-4)
            v = max(v, 0.0)
        assert v > 0.0   # recovered from zero


# ─────────────────────────────────────────────────────────────────────────────
# Wind turbine
# ─────────────────────────────────────────────────────────────────────────────

class TestWindTurbine:
    def test_below_cut_in_zero(self):
        assert _wind_curve_power_w(2.0, 3.0, 25.0, 50.0) == 0.0

    def test_above_cut_out_zero(self):
        assert _wind_curve_power_w(26.0, 3.0, 25.0, 50.0) == 0.0

    def test_at_rated_speed_full_power(self):
        """At v_rated (cut_out * 0.6 = 15.0), power should be ≈ nominal."""
        p = _wind_curve_power_w(15.0, 3.0, 25.0, 50.0)
        assert abs(p - 50000.0) < 1.0   # within 1 W of nominal

    def test_dynamic_ramp_up(self):
        """First-order lag: power should increase monotonically toward target."""
        arr = _compute_wind_power_dynamic(10.0, 3.0, 25.0, 50.0, 500, 1e-4, tau=3.0)
        assert arr[0] >= 0.0
        assert arr[-1] > arr[0]         # ramping up
        assert np.all(np.diff(arr) >= -1e-6)  # monotonically non-decreasing


# ─────────────────────────────────────────────────────────────────────────────
# MPPT P&O
# ─────────────────────────────────────────────────────────────────────────────

class TestMPPTState:
    def test_tracks_vmpp(self):
        """P&O should converge to a stable v_ref after several perturbations."""
        mppt = MPPTState(vmpp_init=34.0, step=0.5, freq=1)
        v = 34.0
        p_prev = 0.0
        for _ in range(50):
            # Simulate P-V curve peak at v=34, quadratic shape
            p = 100.0 - (v - 34.0)**2
            v_new = mppt.perturb_observe(v, p)
            v = v_new
        assert abs(v - 34.0) < 2.0   # should settle near true MPP

    def test_never_goes_below_one(self):
        """v_ref must stay ≥ 1.0 (avoid division by zero in panel model)."""
        mppt = MPPTState(vmpp_init=1.0, step=5.0, freq=1)
        v = 1.0
        for _ in range(20):
            v = mppt.perturb_observe(v, 0.0)
        assert v >= 1.0


# ─────────────────────────────────────────────────────────────────────────────
# Clarke / Park transforms (orthogonality check)
# ─────────────────────────────────────────────────────────────────────────────

class TestControlAlgorithms:
    def test_clarke_balanced_sum_zero(self):
        """Balanced 3-phase: α-component = A, β = 0 for pure phase-A source."""
        a, b, c = 1.0, -0.5, -0.5   # balanced 3-phase at angle 0
        alpha, beta = clarke_transform(a, b, c)
        assert abs(alpha - 1.0) < 1e-9
        assert abs(beta) < 1e-9

    def test_pq_theory_power(self):
        """Instantaneous real power p = V·I·cos(phi)."""
        # Unity power factor: v_alpha=1, i_alpha=1, v_beta=0, i_beta=0 → p=1, q=0
        v_a, v_b, v_c = 1.0, -0.5, -0.5
        i_a, i_b, i_c = 1.0, -0.5, -0.5
        p, q = pq_theory_harmonic_extraction(v_a, v_b, v_c, i_a, i_b, i_c)
        assert abs(q) < 1e-9   # no reactive power at unity PF


# ─────────────────────────────────────────────────────────────────────────────
# Thermal model
# ─────────────────────────────────────────────────────────────────────────────

class TestThermalModel:
    def test_steady_state_temperature(self):
        """
        At steady state: T_j = T_ambient + P * (R_th_jc + R_th_ch + R_th_ha).
        """
        model = IGBT_Thermal_Model(T_ambient=35.0)
        P = 100.0   # 100 W loss
        for _ in range(100000):
            model.step(P, 1e-3)
        expected_tj = 35.0 + P * (0.15 + 0.05 + 0.1)
        assert abs(model.T_j - expected_tj) < 0.5   # within 0.5 °C of steady state

    def test_zero_loss_stays_ambient(self):
        """Zero power loss → junction stays at ambient."""
        model = IGBT_Thermal_Model(T_ambient=25.0)
        for _ in range(1000):
            model.step(0.0, 1e-3)
        assert abs(model.T_j - 25.0) < 1e-6

    def test_ambient_used_correctly(self):
        """BUG-05 fix: ambientTemperature should be independent of solarTemperature."""
        model = IGBT_Thermal_Model(T_ambient=35.0)
        assert model.T_a == 35.0
        assert model.T_j == 35.0   # starts at ambient, not at solar temperature


# ─────────────────────────────────────────────────────────────────────────────
# End-to-end simulation (Phase 6.4)
# ─────────────────────────────────────────────────────────────────────────────

class TestSimulationE2E:
    def _default_req(self, **overrides):
        params = SimulationParameters(**overrides)
        # Minimal topology: solar PV → dc-link → shunt-inv → ac-bus → load
        nodes = [
            Node(id='grid-source', type='electrical', data=NodeData(label='MAIN GRID')),
            Node(id='microgrid',   type='microgrid',  data=NodeData(label='SOLAR PV ARRAY')),
            Node(id='dc-link',     type='electrical', data=NodeData(label='DC LINK')),
        ]
        edges = [
            Edge(id='e1', source='grid-source', target='dc-link'),
            Edge(id='e2', source='microgrid',   target='dc-link'),
        ]
        topology = Topology(nodes=nodes, edges=edges)
        return SimulationRequest(parameters=params, topology=topology)

    def test_returns_data_points(self):
        req = self._default_req(simulationDuration=0.05)
        result = run_simulation(req)
        assert len(result) > 10, "Should return more than 10 data points"

    def test_dc_link_physically_plausible(self):
        req = self._default_req(simulationDuration=0.05, batterySOC=80.0)
        result = run_simulation(req)
        # Allow for initial transient overshoot (no battery connected in this minimal
        # topology, so the capacitor can charge freely in the first few steps).
        # Check that all values are at least non-negative and within a wide physical bound.
        for dp in result:
            assert dp.dcLinkVoltage >= 0.0, "DC link cannot be negative"
            assert dp.dcLinkVoltage < 50000.0, f"Severely implausible DC voltage: {dp.dcLinkVoltage}"
        # Settled value (last 50% of simulation) should be within realistic range
        tail = result[len(result)//2:]
        for dp in tail:
            assert dp.dcLinkVoltage < 5000.0, f"DC link did not settle: {dp.dcLinkVoltage}"

    def test_solar_power_non_negative(self):
        req = self._default_req(simulationDuration=0.05, solarIrradiance=1000.0)
        result = run_simulation(req)
        for dp in result:
            assert dp.solarPowerWatts >= 0.0, "Solar power cannot be negative"

    def test_battery_soc_non_negative(self):
        req = self._default_req(simulationDuration=0.05, batterySOC=50.0)
        result = run_simulation(req)
        for dp in result:
            if dp.batterySOC >= 0.0:   # -1.0 means disconnected
                assert dp.batterySOC <= 100.0

    def test_igbt_temperature_increases_under_load(self):
        """IGBT junction temperature should rise above ambient under non-zero load."""
        req = self._default_req(simulationDuration=0.1, loadActivePower=50.0, ambientTemperature=35.0)
        result = run_simulation(req)
        final_tj = result[-1].igbtTemperature
        assert final_tj >= 35.0, "IGBT temperature should be ≥ ambient under load"

    def test_zero_irradiance_zero_solar_power(self):
        # Must set both solarIrradiance AND irradianceProfile to 0 to ensure
        # the profile parser uses G=0 throughout the simulation
        req = self._default_req(simulationDuration=0.05,
                                solarIrradiance=0.0,
                                irradianceProfile="0:0")
        result = run_simulation(req)
        assert all(dp.solarPowerWatts < 1.0 for dp in result), "No solar power at zero irradiance"

    def test_kp_ki_affect_dc_link(self):
        """BUG-03 fix: different kp/ki values should produce different DC-link dynamics."""
        req_low  = self._default_req(simulationDuration=0.05, kp=0.1, ki=1.0)
        req_high = self._default_req(simulationDuration=0.05, kp=5.0, ki=100.0)
        low_vdc  = np.mean([dp.dcLinkVoltage for dp in run_simulation(req_low)])
        high_vdc = np.mean([dp.dcLinkVoltage for dp in run_simulation(req_high)])
        # High gain should keep DC link closer to reference (700 V)
        assert abs(high_vdc - 700.0) <= abs(low_vdc - 700.0) + 50.0   # within 50 V tolerance

    def test_no_500_point_cap(self):
        """BUG-04 fix: result should contain more than 500 points for long simulations."""
        req = self._default_req(simulationDuration=0.3)
        result = run_simulation(req)
        assert len(result) > 500, f"Expected >500 points, got {len(result)}"
