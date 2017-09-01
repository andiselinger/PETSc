import os
import linecache


ntasks_per_node = 4


sizes = [512, 256, 128, 64]  # dimension of grid
Ns    = [ 64,  16,   4,  1]  # number of nodes


for size, N in zip(sizes, Ns) : 
	# Create filename for the result
	filename = str(N) + "_" + str(size) + ".txt"
	# Create srun command
	command = "srun -l -n" + str(N * ntasks_per_node) + " -N" + str(N) + " ex48 -M " + str(size) + " -N " + str(size) + " -P 16 -log_view :weak/" + filename + " -ksp_max_it 50 -pc_type jacobi -snes_max_linear_solve_fail 10 -thi_mat_type sbaij "
	
	# Open slurm file and write it
	f = open("ex48auto_weak.slrm", 'w')
	f.write("""#!/bin/bash
#
#SBATCH -J ex48_""" + str(N) + """
#SBATCH -N """ + str(N) + """ 
#SBATCH --ntasks-per-node=""" + str(ntasks_per_node) + """ 
#SBATCH --ntasks-per-core=1
#SBATCH --mail-type=BEGIN    # first have to state the type of event to occur 
#SBATCH --mail-user=<selinge@kth.se>   # and then your email address
#SBATCH --time=00:20:00               # Time limit hrs:min:sec

# when srun is used, you need to set:
export I_MPI_PMI_LIBRARY=/cm/shared/apps/slurm/current/lib/libpmi.so
export PETSC_DIR=/home/lv71041/selinger3d/petsc PETSC_ARCH=arch-linux2-c-opt

# module load intel-mpi/2017.3 valgrind/3.12.0 python
module load intel/17 openmpi/2.0.2 python

""" + command)

	f.close()
	os.system("sbatch ex48auto_weak.slrm")
	print command
