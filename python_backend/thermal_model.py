import numpy as np

class IGBT_Thermal_Model:
    def __init__(self, R_th_jc=0.15, R_th_ch=0.05, R_th_ha=0.1, C_th_j=0.01, C_th_c=0.1, C_th_h=1.0, T_ambient=25.0):
        # Foster Thermal Network Parameters (Junction-to-Case-to-Heatsink-to-Ambient)
        self.R_th_jc = R_th_jc # K/W
        self.R_th_ch = R_th_ch
        self.R_th_ha = R_th_ha
        
        self.C_th_j = C_th_j   # J/K
        self.C_th_c = C_th_c
        self.C_th_h = C_th_h
        
        self.T_a = T_ambient
        self.T_j = T_ambient
        self.T_c = T_ambient
        self.T_h = T_ambient
        
    def compute_losses(self, current, voltage, switching_freq):
        # Simplified loss model: Conduction + Switching losses
        v_ce_sat = 1.7 + 0.015 * current
        e_on = 0.005 * current * voltage / 1000.0
        e_off = 0.006 * current * voltage / 1000.0
        
        P_cond = current * v_ce_sat * 0.5  # Assuming 50% duty cycle avg
        P_sw = (e_on + e_off) * switching_freq
        
        return P_cond + P_sw
        
    def step(self, power_loss, dt):
        # 3-RC Node Differential Equations (Euler Integration)
        # dT_j/dt = (P_loss - (T_j - T_c)/R_th_jc) / C_th_j
        
        q_jc = (self.T_j - self.T_c) / self.R_th_jc
        q_ch = (self.T_c - self.T_h) / self.R_th_ch
        q_ha = (self.T_h - self.T_a) / self.R_th_ha
        
        dTj = (power_loss - q_jc) / self.C_th_j
        dTc = (q_jc - q_ch) / self.C_th_c
        dTh = (q_ch - q_ha) / self.C_th_h
        
        self.T_j += dTj * dt
        self.T_c += dTc * dt
        self.T_h += dTh * dt
        
        return self.T_j
