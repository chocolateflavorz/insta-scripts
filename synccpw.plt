#c_pi = Cpi / W epsilon(inf) 
a=0.478
b=0.483
c=6.54
aa=0.4912
kappa_pi = c* abs(a-b)/2
zeta_pi = c*(aa-a)/2
f(x)=C_pi*(1-2*zeta_pi/(thetau_pi(x)+kappa_pi))
C_pi = 1
FIT_LIMIT=1.0e-15
fit f(x) "aasy.dat" u 1:(abs($3)<10 ? $3:0/0) via C_pi
