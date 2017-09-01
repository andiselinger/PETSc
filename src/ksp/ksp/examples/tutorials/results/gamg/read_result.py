import linecache
import os

m = 512
cores_per_node = 2

f = open("perf.txt", 'w')
result_val = []

r = range(1, 13)
s = range(14, 21, 2)
t = r + s

for N in t :

	filename = "gamg_" + str(N) + "_" + str(m) + ".txt"

	line = linecache.getline(filename, 14)
       	print line

        l = []
        for t in line.split():
       	    try:
                l.append(float(t))
            except ValueError:
                pass
        print l[3]
        result_val.append(l[3])
	print result_val

	f.write(str(N * cores_per_node) + ", " + str(l[3] / 1e9) + '\n')
f.close()

