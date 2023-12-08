
import numpy as np

PI = np.pi
E = np.e
N = 16384

#frequency range
freq_s = 800e6
freq_e = 1100e6

# IDT interval[m]
p_i = 4.e-6
# IDT numbers
n_i = 50
# circuit condactance[S]
g_s = 20e-3
# air gap[m]
#l_t = 50.e-6 * p_i
l_t = 0
# propagation velocity in air gap
v_s = 3708
# IDT gap
w = 40e-6

# normalize velocity
v_b = 4031

#com param
a=0.4781
b=0.4831
c=6.5378
aa=0.4912

kappa_2p = c*abs(a-b)/2
kappa_2p = (-1) * abs(kappa_2p)

cpi = 0.9782
epsinf = 55.6414 * 8.854e-12
c_2p = epsinf * cpi * w

for i in range(N):
    f = (freq_e - freq_s) * i / N + freq_s
    omega = 2 * PI * f
    
    thetau_2p = c * (f*p_i/v_b/2 - (a+b)/2)
    zeta_2p = np.sqrt(c*(aa-a)*PI*f*c_2p/4)

    thetap_2p = np.sqrt(thetau_2p**2 - kappa_2p**2)
    if (thetap_2p.imag > 0):
        thetap_2p = -thetap_2p
    gamma = (thetap_2p-thetau_2p)/kappa_2p
    alpha = zeta_2p/(thetau_2p+kappa_2p)

    e_0 = np.exp(-1j * thetap_2p * n_i)
    p_11 = gamma * (1-e_0*e_0)/ (1-(gamma*e_0)**2)
    p_12 = e_0 * (1-gamma*gamma)/(1-(gamma*e_0)**2)
    p_13 = alpha * (1-gamma)*(1-e_0)/(1+(gamma*e_0))
    p_23 = p_13
    p_33 = 8 * alpha * zeta_2p * (
        (1-e_0)*(1+gamma)/(1+gamma*e_0)/thetap_2p - (-1j *n_i) + 1j *omega *c_2p*n_i
    )

    beta_s = omega / v_s
    t = np.exp(-1j * beta_s * l_t)
    y_11 = p_33 - 4 * p_11*p_13*p_13/(1/t**2-p_11*p_11)
    y_12 = -4 * t * p_13*p_13/(1-t*t*p_11*p_11)

    s_11 = (g_s**2-y_11**2+y_12**2) / ((g_s+y_11)**2+y_12**2)
    s_21 = -2 * y_12 * g_s / ((g_s+y_11)**2-y_12**2)

    print (f, " ", 20*np.log10(abs(s_21)), " ", 20 * np.log10(abs(s_11)))