static char help[] = "Test MatMatMult() and MatPtAP() for AIJ matrices.\n\n";

#include <petscmat.h>

int global_index(PetscInt i,PetscInt j,PetscInt k, PetscInt m, PetscInt n) { return i + j * m + k * m * n; }

int main(int argc,char **argv)
{
  Mat            A,B,C;
  PetscScalar    none=-1.;
  PetscErrorCode ierr;
  PetscReal      fill=4;
  PetscReal      norm;
  PetscMPIInt    size,rank;
  PetscBool      test_hypre=PETSC_FALSE;
  PetscScalar    v;

  PetscInt       i,M,N,Istart,Iend,n=7,j,J,Ii,m=8,k,o=1;

  ierr = PetscInitialize(&argc,&argv,(char*)0,help);if (ierr) return ierr;
  ierr = PetscOptionsGetInt(NULL,NULL,"-m",&m,NULL);CHKERRQ(ierr);
  ierr = PetscOptionsGetInt(NULL,NULL,"-n",&n,NULL);CHKERRQ(ierr);
  ierr = PetscOptionsGetInt(NULL,NULL,"-o",&o,NULL);CHKERRQ(ierr);
  //ierr = PetscOptionsGetString(NULL,NULL,"-stencil",stencil,PETSC_MAX_PATH_LEN,NULL);CHKERRQ(ierr);
#if defined(PETSC_HAVE_HYPRE)
  ierr = PetscOptionsGetBool(NULL,NULL,"-test_hypre",&test_hypre,NULL);CHKERRQ(ierr);
#endif
  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRQ(ierr);
  ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRQ(ierr);

  M    = N = m*n*o;
  ierr = MatCreate(PETSC_COMM_WORLD,&A);CHKERRQ(ierr);
  ierr = MatSetSizes(A,PETSC_DECIDE,PETSC_DECIDE,M,N);CHKERRQ(ierr);
  ierr = MatSetType(A,MATAIJ);CHKERRQ(ierr);
  ierr = MatSetFromOptions(A);CHKERRQ(ierr);


  if (0) {      /* 13-point stencil, 3D */
    ierr = MatMPIAIJSetPreallocation(A,13,NULL,13,NULL);CHKERRQ(ierr);
    ierr = MatSeqAIJSetPreallocation(A,13,NULL);CHKERRQ(ierr);
    ierr = MatGetOwnershipRange(A,&Istart,&Iend);CHKERRQ(ierr);
    for (Ii=Istart; Ii<Iend; Ii++) {
      v = -1.0; k = Ii / (m*n); j = (Ii - k * m * n) / m; i = (Ii - k * m * n - j * m);
      if (i>0)   {J = global_index(i-1,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i>1)   {J = global_index(i-2,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-1) {J = global_index(i+1,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-2) {J = global_index(i+2,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j>0)   {J = global_index(i,j-1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j>1)   {J = global_index(i,j-2,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j<n-1) {J = global_index(i,j+1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j<n-2) {J = global_index(i,j+2,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (k>0)   {J = global_index(i,j,k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (k>1)   {J = global_index(i,j,k-2,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (k<o-1) {J = global_index(i,j,k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (k<o-2) {J = global_index(i,j,k+2,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      v = 12.0; ierr = MatSetValues(A,1,&Ii,1,&Ii,&v,INSERT_VALUES);CHKERRQ(ierr);
    }
  }
if (1) {
 ierr = MatMPIAIJSetPreallocation(A,27,NULL,27,NULL);CHKERRQ(ierr);
    ierr = MatSeqAIJSetPreallocation(A,27,NULL);CHKERRQ(ierr);
    ierr = MatGetOwnershipRange(A,&Istart,&Iend);CHKERRQ(ierr);
    for (Ii=Istart; Ii<Iend; Ii++) {
      v = -1.0; k = Ii / (m*n); j = (Ii - k * m * n) / m; i = (Ii - k * m * n - j * m);
      if (k>0) {
        if (j>0) {
          if (i>0)   {J = global_index(i-1,j-1,k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
                      J = global_index(i,  j-1,k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);
          if (i<m-1) {J = global_index(i+1,j-1,k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
        }
        {
          if (i>0)   {J = global_index(i-1,j,  k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
                      J = global_index(i,  j,  k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);
          if (i<m-1) {J = global_index(i+1,j,  k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
        }
        if (j<n-1) {
          if (i>0)   {J = global_index(i-1,j+1,k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
                      J = global_index(i,  j+1,k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);
          if (i<m-1) {J = global_index(i+1,j+1,k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
        }
      }
      {
        if (j>0) {
          if (i>0)   {J = global_index(i-1,j-1,k  ,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
                      J = global_index(i,  j-1,k  ,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);
          if (i<m-1) {J = global_index(i+1,j-1,k  ,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
        }
        {
          if (i>0)   {J = global_index(i-1,j,  k  ,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
                      J = global_index(i,  j,  k  ,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);
          if (i<m-1) {J = global_index(i+1,j,  k  ,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
        }
        if (j<n-1) {
          if (i>0)   {J = global_index(i-1,j+1,k  ,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
                      J = global_index(i,  j+1,k  ,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);
          if (i<m-1) {J = global_index(i+1,j+1,k  ,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
        }
      }
      if (k<o-1) {
        if (j>0) {
          if (i>0)   {J = global_index(i-1,j-1,k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
                      J = global_index(i,  j-1,k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);
          if (i<m-1) {J = global_index(i+1,j-1,k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
        }
        {
          if (i>0)   {J = global_index(i-1,j,  k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
                      J = global_index(i,  j,  k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);
          if (i<m-1) {J = global_index(i+1,j,  k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
        }
        if (j<n-1) {
          if (i>0)   {J = global_index(i-1,j+1,k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
                      J = global_index(i,  j+1,k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);
          if (i<m-1) {J = global_index(i+1,j+1,k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
        }
      }
      v = 26.0; ierr = MatSetValues(A,1,&Ii,1,&Ii,&v,INSERT_VALUES);CHKERRQ(ierr);
    }
  }
  ierr = MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatSetOptionsPrefix(A,"A_");CHKERRQ(ierr);
  //ierr = MatView(A,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);
  ierr = PetscPrintf(PETSC_COMM_WORLD,"\n");CHKERRQ(ierr);

  /* Test MatMatMult() */
  ierr = MatTranspose(A,MAT_INITIAL_MATRIX,&B);CHKERRQ(ierr);      /* B = A^T */
  ierr = MatSetOptionsPrefix(B,"B_");CHKERRQ(ierr);
  ierr = MatMatMult(A,B,MAT_INITIAL_MATRIX,fill,&C);CHKERRQ(ierr); /* C = A*B */
#if 0  
ierr = MatDestroy(&C);CHKERRQ(ierr);
  ierr = MatMatMult(A,B,MAT_INITIAL_MATRIX,fill,&C);CHKERRQ(ierr); /* C = A*B */
  ierr = MatDestroy(&C);CHKERRQ(ierr);
  ierr = MatMatMult(A,B,MAT_INITIAL_MATRIX,fill,&C);CHKERRQ(ierr); /* C = A*B */
  ierr = MatDestroy(&C);CHKERRQ(ierr);
  ierr = MatMatMult(A,B,MAT_INITIAL_MATRIX,fill,&C);CHKERRQ(ierr); /* C = A*B */
  ierr = MatDestroy(&C);CHKERRQ(ierr);
#endif
  //ierr = MatView(C,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);
  if (!rank) {ierr = PetscPrintf(PETSC_COMM_SELF,"\n");CHKERRQ(ierr);}

  ierr = MatDestroy(&A);CHKERRQ(ierr);
  ierr = MatDestroy(&B);CHKERRQ(ierr);
  ierr = MatDestroy(&C);CHKERRQ(ierr);
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
