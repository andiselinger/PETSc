static char help[] = "Test MatMatMult() and MatPtAP() for AIJ matrices.\n\n";

#include <petscmat.h>

int global_index(PetscInt i,PetscInt j,PetscInt k, PetscInt m, PetscInt n) { return i + j * m + k * m * n; }

int main(int argc,char **argv)
{
  Mat            A,B,C;
  PetscErrorCode ierr;
  PetscReal      fill=4;
  PetscMPIInt    size,rank;
  PetscBool      test_hypre=PETSC_FALSE;
  PetscScalar    v;
  PetscBool      equal=PETSC_FALSE;
  char           stencil[PETSC_MAX_PATH_LEN];

  PetscInt       i,M,N,Istart,Iend,n=7,j,J,Ii,m=8,k,o=1;

  PetscInitialize(&argc,&argv,(char*)0,help);
  
  ierr = PetscOptionsGetInt(NULL,NULL,"-m",&m,NULL);CHKERRQ(ierr);
  ierr = PetscOptionsGetInt(NULL,NULL,"-n",&n,NULL);CHKERRQ(ierr);
  ierr = PetscOptionsGetInt(NULL,NULL,"-o",&o,NULL);CHKERRQ(ierr);

  ierr = PetscOptionsGetString(NULL,NULL,"-stencil",stencil,PETSC_MAX_PATH_LEN,NULL);CHKERRQ(ierr);

  /* Create a aij matrix A */
  M    = N = m*n*o;
  ierr = MatCreate(PETSC_COMM_WORLD,&A);CHKERRQ(ierr);
  ierr = MatSetSizes(A,PETSC_DECIDE,PETSC_DECIDE,M,N);CHKERRQ(ierr);
  ierr = MatSetType(A,MATAIJ);CHKERRQ(ierr);
  ierr = MatSetFromOptions(A);CHKERRQ(ierr);

  /* Consistency checks */
  if (o < 1 || m < 1 || n < 1) SETERRQ(PETSC_COMM_WORLD,1,"Dimensions need to be larger than zero!");

  /************ 2D stencils ***************/
  ierr = PetscStrcmp(stencil, "2d5point", &equal);CHKERRQ(ierr);
  if (equal) {   /* 5-point stencil, 2D */
    ierr = MatMPIAIJSetPreallocation(A,5,NULL,5,NULL);CHKERRQ(ierr);
    ierr = MatSeqAIJSetPreallocation(A,5,NULL);CHKERRQ(ierr);
    ierr = MatGetOwnershipRange(A,&Istart,&Iend);CHKERRQ(ierr);
    for (Ii=Istart; Ii<Iend; Ii++) {
      v = -1.0; k = Ii / (m*n); j = (Ii - k * m * n) / m; i = (Ii - k * m * n - j * m);
      if (i>0)   {J = global_index(i-1,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-1) {J = global_index(i+1,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j>0)   {J = global_index(i,j-1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j<n-1) {J = global_index(i,j+1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      v = 4.0; ierr = MatSetValues(A,1,&Ii,1,&Ii,&v,INSERT_VALUES);CHKERRQ(ierr);
    }
  }
  ierr = PetscStrcmp(stencil, "2d9point", &equal);CHKERRQ(ierr);
  if (equal) {      /* 9-point stencil, 2D */
    ierr = MatMPIAIJSetPreallocation(A,9,NULL,9,NULL);CHKERRQ(ierr);
    ierr = MatSeqAIJSetPreallocation(A,9,NULL);CHKERRQ(ierr);
    ierr = MatGetOwnershipRange(A,&Istart,&Iend);CHKERRQ(ierr);
    for (Ii=Istart; Ii<Iend; Ii++) {
      v = -1.0; k = Ii / (m*n); j = (Ii - k * m * n) / m; i = (Ii - k * m * n - j * m);
      if (i>0)            {J = global_index(i-1,j,  k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i>0   && j>0)   {J = global_index(i-1,j-1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (         j>0)   {J = global_index(i,  j-1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-1 && j>0)   {J = global_index(i+1,j-1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-1)          {J = global_index(i+1,j,  k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-1 && j<n-1) {J = global_index(i+1,j+1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j<n-1)          {J = global_index(i,  j+1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i>0   && j<n-1) {J = global_index(i-1,j+1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      v = 8.0; ierr = MatSetValues(A,1,&Ii,1,&Ii,&v,INSERT_VALUES);CHKERRQ(ierr);
    }
  }
  ierr = PetscStrcmp(stencil, "2d9point2", &equal);CHKERRQ(ierr);
  if (equal) {      /* 9-point Cartesian stencil (width 2 per coordinate), 2D */
    ierr = MatMPIAIJSetPreallocation(A,9,NULL,9,NULL);CHKERRQ(ierr);
    ierr = MatSeqAIJSetPreallocation(A,9,NULL);CHKERRQ(ierr);
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
      v = 8.0; ierr = MatSetValues(A,1,&Ii,1,&Ii,&v,INSERT_VALUES);CHKERRQ(ierr);
    }
  }
  ierr = PetscStrcmp(stencil, "2d13point", &equal);CHKERRQ(ierr);
  if (equal) {      /* 13-point Cartesian stencil (width 3 per coordinate), 2D */
    ierr = MatMPIAIJSetPreallocation(A,13,NULL,13,NULL);CHKERRQ(ierr);
    ierr = MatSeqAIJSetPreallocation(A,13,NULL);CHKERRQ(ierr);
    ierr = MatGetOwnershipRange(A,&Istart,&Iend);CHKERRQ(ierr);
    for (Ii=Istart; Ii<Iend; Ii++) {
      v = -1.0; k = Ii / (m*n); j = (Ii - k * m * n) / m; i = (Ii - k * m * n - j * m);
      if (i>0)   {J = global_index(i-1,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i>1)   {J = global_index(i-2,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i>2)   {J = global_index(i-3,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-1) {J = global_index(i+1,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-2) {J = global_index(i+2,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-3) {J = global_index(i+3,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j>0)   {J = global_index(i,j-1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j>1)   {J = global_index(i,j-2,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j>2)   {J = global_index(i,j-3,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j<n-1) {J = global_index(i,j+1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j<n-2) {J = global_index(i,j+2,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j<n-3) {J = global_index(i,j+3,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      v = 12.0; ierr = MatSetValues(A,1,&Ii,1,&Ii,&v,INSERT_VALUES);CHKERRQ(ierr);
    }
  }
  /************ 3D stencils ***************/
  ierr = PetscStrcmp(stencil, "3d7point", &equal);CHKERRQ(ierr);
  if (equal) {      /* 7-point stencil, 3D */
    ierr = MatMPIAIJSetPreallocation(A,7,NULL,7,NULL);CHKERRQ(ierr);
    ierr = MatSeqAIJSetPreallocation(A,7,NULL);CHKERRQ(ierr);
    ierr = MatGetOwnershipRange(A,&Istart,&Iend);CHKERRQ(ierr);
    for (Ii=Istart; Ii<Iend; Ii++) {
      v = -1.0; k = Ii / (m*n); j = (Ii - k * m * n) / m; i = (Ii - k * m * n - j * m);
      if (i>0)   {J = global_index(i-1,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-1) {J = global_index(i+1,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j>0)   {J = global_index(i,j-1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j<n-1) {J = global_index(i,j+1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (k>0)   {J = global_index(i,j,k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (k<o-1) {J = global_index(i,j,k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      v = 6.0; ierr = MatSetValues(A,1,&Ii,1,&Ii,&v,INSERT_VALUES);CHKERRQ(ierr);
    }
  }
  ierr = PetscStrcmp(stencil, "3d13point", &equal);CHKERRQ(ierr);
  if (equal) {      /* 13-point stencil, 3D */
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
  ierr = PetscStrcmp(stencil, "3d19point", &equal);CHKERRQ(ierr);
  if (equal) {      /* 19-point stencil, 3D */
    ierr = MatMPIAIJSetPreallocation(A,19,NULL,19,NULL);CHKERRQ(ierr);
    ierr = MatSeqAIJSetPreallocation(A,19,NULL);CHKERRQ(ierr);
    ierr = MatGetOwnershipRange(A,&Istart,&Iend);CHKERRQ(ierr);
    for (Ii=Istart; Ii<Iend; Ii++) {
      v = -1.0; k = Ii / (m*n); j = (Ii - k * m * n) / m; i = (Ii - k * m * n - j * m);
      /* one hop */
      if (i>0)   {J = global_index(i-1,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-1) {J = global_index(i+1,j,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j>0)   {J = global_index(i,j-1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j<n-1) {J = global_index(i,j+1,k,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (k>0)   {J = global_index(i,j,k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (k<o-1) {J = global_index(i,j,k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      /* two hops */
      if (i>0   && j>0)   {J = global_index(i-1,j-1,k  ,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i>0   && k>0)   {J = global_index(i-1,j,  k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i>0   && j<n-1) {J = global_index(i-1,j+1,k  ,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i>0   && k<o-1) {J = global_index(i-1,j,  k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-1 && j>0)   {J = global_index(i+1,j-1,k  ,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-1 && k>0)   {J = global_index(i+1,j,  k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-1 && j<n-1) {J = global_index(i+1,j+1,k  ,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (i<m-1 && k<o-1) {J = global_index(i+1,j,  k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j>0   && k>0)   {J = global_index(i,  j-1,k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j>0   && k<o-1) {J = global_index(i,  j-1,k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j<n-1 && k>0)   {J = global_index(i,  j+1,k-1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      if (j<n-1 && k<o-1) {J = global_index(i,  j+1,k+1,m,n); ierr = MatSetValues(A,1,&Ii,1,&J,&v,INSERT_VALUES);CHKERRQ(ierr);}
      v = 18.0; ierr = MatSetValues(A,1,&Ii,1,&Ii,&v,INSERT_VALUES);CHKERRQ(ierr);
    }
  }
  ierr = PetscStrcmp(stencil, "3d27point", &equal);CHKERRQ(ierr);
  if (equal) {      /* 27-point stencil, 3D */
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
  ierr = MatMatMult(B,A,MAT_INITIAL_MATRIX,fill,&C);CHKERRQ(ierr); /* C = B*A */
  ierr = MatSetOptionsPrefix(C,"C_");CHKERRQ(ierr);
  //ierr = MatView(C,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);
  if (!rank) {ierr = PetscPrintf(PETSC_COMM_SELF,"\n");CHKERRQ(ierr);}


  ierr = MatDestroy(&A);CHKERRQ(ierr);
  ierr = MatDestroy(&B);CHKERRQ(ierr);
  ierr = MatDestroy(&C);CHKERRQ(ierr);
  PetscFinalize();
  return(0);


}

