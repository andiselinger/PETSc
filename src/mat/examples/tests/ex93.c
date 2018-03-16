static char help[] = "Test MatMatMult() and MatPtAP() for AIJ matrices.\n\n";

#include <petscmat.h>

int main(int argc,char **argv)
{
  Mat            A,B,C,D;
  PetscScalar    a[] ={
0.,0.,0.,5.,0.,4.,
0.,3.,0.,2.,0.,0.,
-3.,0.,0.,10.,0.,0.,
0.,0.,0.,-1.,0.,-2.,
0.,0.,9.,0.,0.,-4.,
0.,8.,0.,0.,6.,-5.,

};
  PetscInt       i[]={0,1,2,3,4,5};
  PetscInt       j[]={0,1,2,3,4,5};
  PetscInt       k;
  PetscScalar    none=-1.;
  PetscErrorCode ierr;
  PetscReal      fill=4;
  PetscReal      norm;
  PetscMPIInt    size,rank;
  PetscBool      test_hypre=PETSC_FALSE;

  ierr = PetscInitialize(&argc,&argv,(char*)0,help);if (ierr) return ierr;
#if defined(PETSC_HAVE_HYPRE)
  ierr = PetscOptionsGetBool(NULL,NULL,"-test_hypre",&test_hypre,NULL);CHKERRQ(ierr);
#endif
  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRQ(ierr);
  ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRQ(ierr);

  ierr = MatCreate(PETSC_COMM_WORLD,&A);CHKERRQ(ierr);
  ierr = MatSetSizes(A,PETSC_DECIDE,PETSC_DECIDE,6,6);CHKERRQ(ierr);
  ierr = MatSetType(A,MATAIJ);CHKERRQ(ierr);
  ierr = MatSetFromOptions(A);CHKERRQ(ierr);
  ierr = MatSetUp(A);CHKERRQ(ierr);
  ierr = MatSetOption(A,MAT_IGNORE_ZERO_ENTRIES,PETSC_TRUE);CHKERRQ(ierr);

  if (!rank) {
    //for (k=0; k<4; k++) {
      ierr = MatSetValues(A,6,i,6,j,a,ADD_VALUES);CHKERRQ(ierr);
    //}
  }
  ierr = MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatSetOptionsPrefix(A,"A_");CHKERRQ(ierr);
  ierr = MatView(A,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);
  ierr = PetscPrintf(PETSC_COMM_WORLD,"\n");CHKERRQ(ierr);

  /* Test MatMatMult() */
  ierr = MatTranspose(A,MAT_INITIAL_MATRIX,&B);CHKERRQ(ierr);      /* B = A^T */
  ierr = MatSetOptionsPrefix(B,"B_");CHKERRQ(ierr);
  ierr = MatMatMult(A,B,MAT_INITIAL_MATRIX,fill,&C);CHKERRQ(ierr); /* C = A*B */
  //ierr = MatMatMultNumeric(B,A,C);CHKERRQ(ierr);                   /* recompute C=B*A */
  //ierr = MatMatMult(B,A,MAT_REUSE_MATRIX,fill,&C);CHKERRQ(ierr);   /* recompute C=B*A */
  ierr = MatSetOptionsPrefix(C,"C_");CHKERRQ(ierr);
  ierr = MatView(C,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);
  if (!rank) {ierr = PetscPrintf(PETSC_COMM_SELF,"\n");CHKERRQ(ierr);}

  ierr = MatDestroy(&A);CHKERRQ(ierr);
  ierr = MatDestroy(&B);CHKERRQ(ierr);
  ierr = MatDestroy(&C);CHKERRQ(ierr);
  //ierr = MatDestroy(&D);CHKERRQ(ierr);
  ierr = PetscFinalize();
  return ierr;
}


/*TEST

   test:

   test:
      suffix: 2
      nsize: 2
      args: -B_matmatmult_via nonscalable

   test:
      suffix: 3
      nsize: 2
      output_file: output/ex93_2.out

   test:
      suffix: 4
      nsize: 2
      args: -A_matptap_via scalable
      output_file: output/ex93_2.out

   test:
      suffix: btheap
      args: -B_matmatmult_via btheap> ex93_1.tmp 2>&1
      output_file: output/ex93_1.out

   test:
      suffix: heap
      args: -B_matmatmult_via heap> ex93_1.tmp 2>&1
      output_file: output/ex93_1.out

   test:
      suffix: hypre
      nsize: 3
      requires: hypre
      args: -B_matmatmult_via hypre -A_matptap_via hypre -test_hypre

   test:
      suffix: llcondensed
      args: -B_matmatmult_via llcondensed
      output_file: output/ex93_1.out

   test:
      suffix: scalable
      args: -B_matmatmult_via scalable
      output_file: output/ex93_1.out

   test:
      suffix: scalable_fast
      args: -B_matmatmult_via scalable_fast
      output_file: output/ex93_1.out

TEST*/
