import numpy
import csv
import sys
import glob
from math import log10

PI = numpy.pi
E  = numpy.e
VB = 4031
P  = 2e-6

def convf(f):
    return f * VB / P

arg = sys.argv[1:len(sys.argv)]

input_file = "alien.dat"
output_file = "invader.dat"
if arg[0] == "":
    print("wnbeta.py [input] [output]");
    print("default input: alien.dat")
else:
    input_file = arg[0]

if arg[1] == "":
    print("default output: invader.dat")
else:
    output_file = arg[1]

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

r = []
for i in range(len(matrix1)):
    r_row = []
    r_row.append(float(matrix1[i][0]))
    rb = (2 * PI * convf(float(matrix1[i][0])) / float(matrix1[i][1]))
    rb_m1 = rb - (PI / P)
    r_row.append(rb_m1)
    ib = rb * float(matrix1[i][2]) / -(40 * PI * log10(E))
    r_row.append(ib)
    r_row.append(rb_m1 * rb_m1 - ib * ib)
    r.append(r_row)

with open(output_file, 'w', newline='') as f:
    evilwriter = csv.writer(f, delimiter=' ')
    for c in r:
        evilwriter.writerow(c)
