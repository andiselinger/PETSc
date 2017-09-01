import os
import linecache


ntasks_per_node = 2

m = 512
n = 512

r = range(1, 13)
s = range(14, 25, 2)
t = r + s

for N in t : 
	filename = "direct_" + str(N) + "_" + str(m) + ".txt"

	command = "srun -l -n" + str(N * ntasks_per_node) + " -N" + str(N) + " ex2 -m " + str(m) + " -n " + str(n) + " -log_view :results/direct/" + filename + "  -pc_type lu -pc_factor_mat_solver_package superlu_dist "

	f = open("ex2auto.slrm", 'w')
	f.write("""#!/bin/bash
#
#SBATCH -J ex2_direct_""" + str(N) + """
#SBATCH -N """ + str(N) + """ 
#SBATCH --ntasks-per-node=""" + str(ntasks_per_node) + """ 
#SBATCH --ntasks-per-core=1
#SBATCH --mail-type=BEGIN    # first have to state the type of event to occur 
#SBATCH --mail-user=<selinge@kth.se>   # and then your email address
#SBATCH --time=00:10:00               # Time limit hrs:min:sec

# when srun is used, you need to set:
export I_MPI_PMI_LIBRARY=/cm/shared/apps/slurm/current/lib/libpmi.so
export PETSC_DIR=/home/lv71041/selinger3d/petsc PETSC_ARCH=arch-linux2-c-opt

module load intel-mpi/2017.3 valgrind/3.12.0 python
#module load intel/17 openmpi/2.0.2 python

""" + command)

	f.close()
	os.system("sbatch ex2auto.slrm")
