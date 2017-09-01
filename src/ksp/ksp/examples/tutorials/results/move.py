import os
import linecache

r = range(1, 13)
s = range(14, 25, 2)
t = r + s

for N in t :
	old_name = "nopcjacobi_" + str(N) + "_512.txt"
	new_name = "jacobi_" + str(N) + "_512.txt"

	command = "mv " + old_name + " ./nopc/" + new_name
	os.system(command)

