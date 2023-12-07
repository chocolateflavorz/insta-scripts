import numpy
import csv
import sys
import glob
import scipy.optimize as optimize
from math import log10

PI = numpy.pi
E  = numpy.e
VB = 4031
P  = 2e-6

print("VB =", VB, "P =", P)

def convf(f):
    return f * VB / P

arg = sys.argv[1:len(sys.argv)]

input_file = "alien.dat"
output_file = "invader.dat"
input_file_o = "cheese.dat"
output_file_o = "crakatoa.dat"

if arg[0] == "":
    print("openparam.py [short input] [short output] [short input] [short output]")
    print("default input: alien.dat")
else:
    input_file = arg[0]

if arg[1] == "":
    print("default output: invader.dat")
else:
    output_file = arg[1]

if arg[2] == "":
    print("default input: cheese.dat")
else:
    input_file_o = arg[2]

if arg[3] == "":
    print("default output: crakatoa.dat")
else:
    output_file_o = arg[3]

def trim(a):
    r = []
    for e in a:
        if e != '':
            r.append(e)
    return r

# fp V a eR eI K2
with open(input_file, 'r') as f:
    reader = csv.reader(f, delimiter=' ')
    matrix1 = []
    for row in reader:
        if row[0] == "#":
            continue
        row = trim(row)
        matrix1.append(row)

# ugly cat, but it works
r = []
fp = []
thetaP2 = []
for i in range(len(matrix1)):
    r_row = []
    r_row.append(float(matrix1[i][0]))
    fp.append(float(matrix1[i][0]))
    rb = (2 * PI * convf(float(matrix1[i][0])) / float(matrix1[i][1]))
    rb_m1 = (rb - (PI / P)) * P
    r_row.append(rb_m1)
    ib = (rb * float(matrix1[i][2]) / -(40 * PI * log10(E))) * P
    r_row.append(ib)
    r_row.append((rb_m1 * rb_m1 - (ib * ib)) )
    thetaP2.append((rb_m1 * rb_m1 - (ib * ib)) )
    r.append(r_row)

with open(output_file, 'w', newline='') as f:
    evilwriter = csv.writer(f, delimiter=' ')
    for c in r:
        evilwriter.writerow(c)

#open param
with open(input_file_o, 'r') as f:
    reader = csv.reader(f, delimiter=' ')
    matrix2 = []
    for row in reader:
        if row[0] == "#":
            continue
        row = trim(row)
        matrix2.append(row)

r_o = []
fp_o = []
thetaP2_o = []
for i in range(len(matrix2)):
    r_row = []
    r_row.append(float(matrix2[i][0]))
    fp_o.append(float(matrix2[i][0]))
    rb = (2 * PI * convf(float(matrix2[i][0])) / float(matrix2[i][1]))
    rb_m1 = (rb - (PI / P)) * P
    r_row.append(rb_m1)
    ib = (rb * float(matrix2[i][2]) / -(40 * PI * log10(E))) * P
    r_row.append(ib)
    r_row.append((rb_m1 * rb_m1 - (ib * ib)) )
    thetaP2_o.append((rb_m1 * rb_m1 - (ib * ib)) )
    r_o.append(r_row)

with open(output_file_o, 'w', newline='') as f:
    evilwriter = csv.writer(f, delimiter=' ')
    for c in r_o:
        evilwriter.writerow(c)

#r = [fp, reB - (pi/P), imB, thetapP^2]
def f(x, a, b, c):
    return c * c * (x - b) * (x - a)
print ("fp47 = ", convf(.47))
print("initial guess: a, b, c = .47 .48 13")
popt, pcov = optimize.curve_fit(f, fp, thetaP2, [.47, .48, 13])
print("a, b, c =", popt)
print("err =", numpy.sqrt(numpy.diag(pcov)))

a = popt[0]
b = popt[1]
c = popt[2]
theta_u = c * (a/2 - b/2) 
kappa = c* abs(a-b) / 2
print("theta_u*P =", theta_u, "kappa*P =", kappa)
print("theta_u kappa =", theta_u/P,kappa/P)

def g(x, d):
    return c * c * (x-b) * (x-d)
print("initial guess: a' = .49")
popt, pcov = optimize.curve_fit(g, fp_o, thetaP2_o, [.49])
print("a' =", popt)
print("err =", numpy.sqrt(numpy.diag(pcov)))

d = popt[0]
d_wc = c * (d -a) / 8
print("d_wc*P =", d_wc)
print("d_wc =", d_wc / P) 

print("|a-b|=", abs(a-b), "|a'-b|=", abs(d-b)) 
print("kappa is positive=" , abs(a-b) > abs(d-b))