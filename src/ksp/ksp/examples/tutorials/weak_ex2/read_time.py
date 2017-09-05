import linecache
import os

m = 512
cores_per_node = 2

f = open("times.txt", 'w')
result_val = []

#r = range(1, 13)
#s = range(14, 21, 2)
#t = r + s
t = [1, 4, 16, 64, 256]

for N in t :

	filename = "n" + str(N) + "_128_lu" + ".txt"

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

	f.write(str(N) + ", " + str(l[0]) + '\n')
f.close()

