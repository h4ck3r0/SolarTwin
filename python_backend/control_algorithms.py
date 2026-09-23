import numpy as np
import numba

@numba.njit
def clarke_transform(a, b, c):
    # Transforms abc to alpha-beta
    alpha = (2.0/3.0) * (a - 0.5 * b - 0.5 * c)
    beta = (2.0/3.0) * (np.sqrt(3.0)/2.0 * b - np.sqrt(3.0)/2.0 * c)
    return alpha, beta

@numba.njit
def park_transform(alpha, beta, theta):
    # Transforms alpha-beta to d-q
    d = alpha * np.cos(theta) + beta * np.sin(theta)
    q = -alpha * np.sin(theta) + beta * np.cos(theta)
    return d, q

@numba.njit
def inv_park_transform(d, q, theta):
    alpha = d * np.cos(theta) - q * np.sin(theta)
    beta = d * np.sin(theta) + q * np.cos(theta)
    return alpha, beta

@numba.njit
def inv_clarke_transform(alpha, beta):
    a = alpha
    b = -0.5 * alpha + (np.sqrt(3.0)/2.0) * beta
    c = -0.5 * alpha - (np.sqrt(3.0)/2.0) * beta
    return a, b, c

@numba.njit
def srf_pll(v_a, v_b, v_c, theta_prev, v_q_int_prev, dt, kp=0.5, ki=10.0, omega_ff=376.99):
    # SRF-PLL (Synchronous Reference Frame PLL)
    v_alpha, v_beta = clarke_transform(v_a, v_b, v_c)
    v_d, v_q = park_transform(v_alpha, v_beta, theta_prev)
    
    # PI Controller on V_q (Driving V_q to 0 aligns d-axis with phase A)
    v_q_int = v_q_int_prev + v_q * dt
    omega = omega_ff + kp * v_q + ki * v_q_int
    
    theta = theta_prev + omega * dt
    if theta > 2 * np.pi:
        theta -= 2 * np.pi
    
    return theta, omega, v_q_int

@numba.njit
def pq_theory_harmonic_extraction(v_a, v_b, v_c, i_a, i_b, i_c):
    # Instantaneous Active and Reactive Power (p-q) Theory
    v_alpha, v_beta = clarke_transform(v_a, v_b, v_c)
    i_alpha, i_beta = clarke_transform(i_a, i_b, i_c)
    
    # Instantaneous real (p) and imaginary (q) power
    p = v_alpha * i_alpha + v_beta * i_beta
    q = v_alpha * i_beta - v_beta * i_alpha
    
    return p, q
