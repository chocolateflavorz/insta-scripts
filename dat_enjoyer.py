import csv
import sys
import glob
from math import log10

arg = sys.argv[1:len(sys.argv)]

input_file = "alien.dat"
output_file = "invader.dat"
if arg[0] == "":
    print("cac.py [input] [output]");
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
    r.append(r_row)

with open(output_file, 'w', newline='') as f:
    evilwriter = csv.writer(f, delimiter=' ')
    for c in r:
        evilwriter.writerow(c)
