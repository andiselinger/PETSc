/*$Id: ex24.c,v 1.17 2001/01/23 20:54:05 balay Exp balay $*/

static char help[] = "Scatters from a parallel vector to a sequential vector.\n\
Tests where the local part of the scatter is a copy.\n\n";

#include "petscvec.h"
#include "petscsys.h"

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **argv)
{
  int           n = 5,ierr,size,rank,i,*blks,bs = 1,m = 2;
  Scalar        value;
  Vec           x,y;
  IS            is1,is2;
  VecScatter    ctx = 0;
  PetscViewer        sviewer;

  ierr = PetscInitialize(&argc,&argv,(char*)0,help);CHKERRQ(ierr); 

  ierr = PetscOptionsGetInt(PETSC_NULL,"-n",&n,PETSC_NULL);CHKERRQ(ierr);
  ierr = PetscOptionsGetInt(PETSC_NULL,"-bs",&bs,PETSC_NULL);CHKERRQ(ierr);

  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRQ(ierr);
  ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRQ(ierr);

  /* create two vectors */
  ierr = VecCreateMPI(PETSC_COMM_WORLD,PETSC_DECIDE,size*bs*n,&x);CHKERRQ(ierr);

  /* create two index sets */
  if (rank < size-1) {
    m = n + 2; 
  } else {
    m = n;
  }
  ierr = PetscMalloc((m)*sizeof(int),&blks);CHKERRQ(ierr);
  blks[0] = n*rank*bs;
  for (i=1; i<m; i++) {
    blks[i] = blks[i-1] + bs;   
  }
  ierr = ISCreateBlock(PETSC_COMM_SELF,bs,m,blks,&is1);CHKERRQ(ierr);
  ierr = PetscFree(blks);CHKERRQ(ierr);

  ierr = VecCreateSeq(PETSC_COMM_SELF,bs*m,&y);CHKERRQ(ierr);
  ierr = ISCreateStride(PETSC_COMM_SELF,bs*m,0,1,&is2);CHKERRQ(ierr);

  /* each processor inserts the entire vector */
  /* this is redundant but tests assembly */
  for (i=0; i<bs*n*size; i++) {
    value = (Scalar) i;
    ierr = VecSetValues(x,1,&i,&value,INSERT_VALUES);CHKERRQ(ierr);
  }
  ierr = VecAssemblyBegin(x);CHKERRQ(ierr);
  ierr = VecAssemblyEnd(x);CHKERRQ(ierr);
  ierr = VecView(x,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);

  ierr = VecScatterCreate(x,is1,y,is2,&ctx);CHKERRQ(ierr);
  ierr = VecScatterBegin(x,y,INSERT_VALUES,SCATTER_FORWARD,ctx);CHKERRQ(ierr);
  ierr = VecScatterEnd(x,y,INSERT_VALUES,SCATTER_FORWARD,ctx);CHKERRQ(ierr);

  ierr = PetscSynchronizedPrintf(PETSC_COMM_WORLD,"----\n");CHKERRQ(ierr); 
  ierr = PetscViewerGetSingleton(PETSC_VIEWER_STDOUT_WORLD,&sviewer);CHKERRQ(ierr);
  ierr = VecView(y,sviewer);CHKERRQ(ierr); fflush(stdout);
  ierr = PetscViewerRestoreSingleton(PETSC_VIEWER_STDOUT_WORLD,&sviewer);CHKERRQ(ierr);
  ierr = PetscSynchronizedFlush(PETSC_COMM_WORLD);CHKERRQ(ierr);

  ierr = VecScatterDestroy(ctx);CHKERRQ(ierr);

  ierr = VecDestroy(x);CHKERRQ(ierr);
  ierr = VecDestroy(y);CHKERRQ(ierr);
  ierr = ISDestroy(is1);CHKERRQ(ierr);
  ierr = ISDestroy(is2);CHKERRQ(ierr);

  ierr = PetscFinalize();CHKERRQ(ierr);
  return 0;
}
 
