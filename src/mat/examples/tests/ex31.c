/*$Id: ex31.c,v 1.24 2001/01/23 20:55:11 balay Exp balay $*/

static char help[] = 
"Tests binary I/O of matrices and illustrates user-defined event logging.\n\n";

#include "petscmat.h"

/* Note:  Most applications would not read and write the same matrix within
  the same program.  This example is intended only to demonstrate
  both input and output. */

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **args)
{
  Mat     C;
  Scalar  v;
  int     i,j,I,J,ierr,Istart,Iend,N,m = 4,n = 4,rank,size;
  PetscViewer  viewer;
  int     MATRIX_GENERATE,MATRIX_READ;

  PetscInitialize(&argc,&args,(char *)0,help);
  ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRQ(ierr);
  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRQ(ierr);
  ierr = PetscOptionsGetInt(PETSC_NULL,"-m",&m,PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscOptionsGetInt(PETSC_NULL,"-n",&n,PETSC_NULL);CHKERRQ(ierr);
  N = m*n;

  /* PART 1:  Generate matrix, then write it in binary format */

  ierr = PetscLogEventRegister(&MATRIX_GENERATE,"Generate Matrix",PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscLogEventBegin(MATRIX_GENERATE,0,0,0,0);CHKERRQ(ierr);

  /* Generate matrix */
  ierr = MatCreate(PETSC_COMM_WORLD,PETSC_DECIDE,PETSC_DECIDE,N,N,&C);CHKERRQ(ierr);
  ierr = MatSetFromOptions(C);CHKERRQ(ierr);
  ierr = MatGetOwnershipRange(C,&Istart,&Iend);CHKERRQ(ierr);
  for (I=Istart; I<Iend; I++) { 
    v = -1.0; i = I/n; j = I - i*n;  
    if (i>0)   {J = I - n; ierr = MatSetValues(C,1,&I,1,&J,&v,ADD_VALUES);CHKERRQ(ierr);}
    if (i<m-1) {J = I + n; ierr = MatSetValues(C,1,&I,1,&J,&v,ADD_VALUES);CHKERRQ(ierr);}
    if (j>0)   {J = I - 1; ierr = MatSetValues(C,1,&I,1,&J,&v,ADD_VALUES);CHKERRQ(ierr);}
    if (j<n-1) {J = I + 1; ierr = MatSetValues(C,1,&I,1,&J,&v,ADD_VALUES);CHKERRQ(ierr);}
    v = 4.0; ierr = MatSetValues(C,1,&I,1,&I,&v,ADD_VALUES);CHKERRQ(ierr);
  }
  ierr = MatAssemblyBegin(C,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatAssemblyEnd(C,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatView(C,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);

  ierr = PetscPrintf(PETSC_COMM_WORLD,"writing matrix in binary to matrix.dat ...\n");CHKERRQ(ierr);
  ierr = PetscViewerBinaryOpen(PETSC_COMM_WORLD,"matrix.dat",PETSC_BINARY_CREATE,&viewer);CHKERRQ(ierr);
  ierr = MatView(C,viewer);CHKERRQ(ierr);
  ierr = PetscViewerDestroy(viewer);CHKERRQ(ierr);
  ierr = MatDestroy(C);CHKERRQ(ierr);
  ierr = PetscLogEventEnd(MATRIX_GENERATE,0,0,0,0);CHKERRQ(ierr);

  /* PART 2:  Read in matrix in binary format */

  /* All processors wait until test matrix has been dumped */
  ierr = MPI_Barrier(PETSC_COMM_WORLD);CHKERRQ(ierr);

  ierr = PetscLogEventRegister(&MATRIX_READ,"Read Matrix",PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscLogEventBegin(MATRIX_READ,0,0,0,0);CHKERRQ(ierr);
  ierr = PetscPrintf(PETSC_COMM_WORLD,"reading matrix in binary from matrix.dat ...\n");CHKERRQ(ierr);
  ierr = PetscViewerBinaryOpen(PETSC_COMM_WORLD,"matrix.dat",PETSC_BINARY_RDONLY,&viewer);CHKERRQ(ierr);
  ierr = MatLoad(viewer,MATMPIAIJ,&C);CHKERRQ(ierr);
  ierr = PetscViewerDestroy(viewer);CHKERRQ(ierr);
  ierr = PetscLogEventEnd(MATRIX_READ,0,0,0,0);CHKERRQ(ierr);
  ierr = MatView(C,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);

  /* Free data structures */
  ierr = MatDestroy(C);CHKERRQ(ierr);

  ierr = PetscFinalize();CHKERRQ(ierr);
  return 0;
}


