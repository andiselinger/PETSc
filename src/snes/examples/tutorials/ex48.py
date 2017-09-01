import os
import linecache


os.system("#SBATCH -N 4")
os.system("#SBATCH --ntasks-per-node=16")



result_val = []
result_size = []
sizes = [16, 32, 64, 92, 128, 192, 256, 320, 384, 512, 640, 768]
f = open('results.txt', 'w')
for num in sizes:
	print num
	m = num		# number of grid points in x direction
	n = num	 	# number of grid points in y direction
	result_size.append( str(m) )
	filename = "NP64_" + str(num) + ".txt"
	command = "srun -l -n64 -N4 ex48 -M " + str(m) + " -N " + str(n) + " -P 16 -log_view :" + filename + " -ksp_max_it 50 -pc_type jacobi -snes_max_linear_solve_fail 10 -thi_mat_type sbaij "
	print command
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

