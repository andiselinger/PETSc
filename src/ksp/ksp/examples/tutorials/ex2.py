import os
import linecache


result_val = []
result_size = []
f = open('results.txt', 'w')
for num in range(3,9):
	print num
	m = pow(2, num)	# number of grid points in x direction
	n = pow(2, num) # number of grid points in y direction
	result_size.append( str(m) )
	filename = "file" + str(num) + ".txt"
	command = "srun -N16 -n256 ./ex2 -m " + str(m) + " -n " + str(n) + " -log_view :" + filename
	os.system(command) 

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
f.write(str(result_size))
f.write('\n')
f.write(str(result_val))
f.close()



