import sys
import csv

arg_length = len(sys.argv)
arg_list = sys.argv[1:arg_length]
with open(arg_list[0]) as f:
    reader = csv.reader(f, delimiter=' ')
    for row in reader:
        if len(row) == 0 or row[0] == '#':
            continue
        print("frequency: " + row[0])
        z12 = float(row[4]) + 1j * float(row[5])
        z11 = float(row[1]) + 1j * float(row[2])
        h = z12 / z11
        print("h: " + str(h))
