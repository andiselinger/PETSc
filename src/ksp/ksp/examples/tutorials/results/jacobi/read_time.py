import linecache
import os

m = 512
cores_per_node = 2

f = open("times.txt", 'w')
result_val = []

r = range(1, 13)
s = range(14, 25, 2)
t = r + s

for N in t :

	filename = "jacobi_" + str(N) + "_" + str(m) + ".txt"

	line = linecache.getline(filename, 11)
       	print line

        l = []
        for t in line.split():
       	    try:
                l.append(float(t))
            except ValueError:
                pass
        print l[0]
        result_val.append(l[0])
	print result_val

	f.write(str(N * cores_per_node) + ", " + str(l[0]) + '\n')
f.close()

