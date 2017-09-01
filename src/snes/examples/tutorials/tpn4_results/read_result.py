import linecache
import os

m = 384

f = open("results.txt", 'w')
result_val = []

for N in range(8,129, 8) :

	filename = str(N) + "_" + str(m) + ".txt"

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

	f.write(str(N) + ", " + str(l[3] / 1e9) + '\n')
f.close()

