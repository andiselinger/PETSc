static char help[] = "Test MatMatMult() and MatPtAP() for AIJ matrices.\n\n";

#include <petscmat.h>


#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **argv)
{
  Mat            A,B,C;
  PetscScalar    a[] ={1.,1.,0.,0.,1.,1.,0.,0.,1.};
  PetscInt       ij[]={0,1,2};
  PetscErrorCode ierr;
  PetscReal      fill=4;
  PetscMPIInt    size,rank;

  PetscInitialize(&argc,&argv,(char*)0,help);
  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRQ(ierr);
  ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRQ(ierr);

  ierr = MatCreate(PETSC_COMM_WORLD,&A);CHKERRQ(ierr);
  ierr = MatSetSizes(A,PETSC_DECIDE,PETSC_DECIDE,3,3);CHKERRQ(ierr);
  ierr = MatSetType(A,MATAIJ);CHKERRQ(ierr);
  ierr = MatSetFromOptions(A);CHKERRQ(ierr);
  ierr = MatSetUp(A);CHKERRQ(ierr);
  ierr = MatSetOption(A,MAT_IGNORE_ZERO_ENTRIES,PETSC_TRUE);CHKERRQ(ierr);

  if (!rank) {
    ierr = MatSetValues(A,3,ij,3,ij,a,ADD_VALUES);CHKERRQ(ierr);
  }
  ierr = MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatSetOptionsPrefix(A,"A_");CHKERRQ(ierr);
  ierr = MatView(A,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);
  ierr = PetscPrintf(PETSC_COMM_WORLD,"\n");CHKERRQ(ierr);

  /* Test MatMatMult() */
  ierr = MatTranspose(A,MAT_INITIAL_MATRIX,&B);CHKERRQ(ierr);      /* B = A^T */
  ierr = MatMatMult(B,A,MAT_INITIAL_MATRIX,fill,&C);CHKERRQ(ierr); /* C = B*A */
  ierr = MatSetOptionsPrefix(C,"C_");CHKERRQ(ierr);
  ierr = MatView(C,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);
  if (!rank) {ierr = PetscPrintf(PETSC_COMM_SELF,"\n");CHKERRQ(ierr);}


  ierr = MatDestroy(&A);CHKERRQ(ierr);
  ierr = MatDestroy(&B);CHKERRQ(ierr);
  ierr = MatDestroy(&C);CHKERRQ(ierr);
  PetscFinalize();
  return(0);
}



