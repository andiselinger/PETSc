/* $Id: ex1.c,v 1.20 2001/01/23 20:57:50 balay Exp balay $ */

static char help[] = "Demonstrates constructing an application ordering\n\n";

#include "petsc.h"
#include "petscao.h"

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **argv)
{
  int      n = 5,ierr,rank,size,getpetsc[] = {0,3,4};
  int      getapp[] = {2,1,3,4};
  IS       ispetsc,isapp;
  AO       ao;

  ierr = PetscInitialize(&argc,&argv,(char*)0,help);CHKERRQ(ierr); 
  ierr = PetscOptionsGetInt(PETSC_NULL,"-n",&n,PETSC_NULL);CHKERRQ(ierr);
  ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRQ(ierr);
  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRQ(ierr);

  /* create the index sets */
  ierr = ISCreateStride(PETSC_COMM_WORLD,n,rank,size,&ispetsc);CHKERRQ(ierr);
  ierr = ISCreateStride(PETSC_COMM_WORLD,n,n*rank,1,&isapp);CHKERRQ(ierr);

  /* create the application ordering */
  ierr = AOCreateBasicIS(isapp,ispetsc,&ao);CHKERRQ(ierr);

  ierr = ISDestroy(ispetsc);CHKERRQ(ierr);
  ierr = ISDestroy(isapp);CHKERRQ(ierr);

  ierr = AOView(ao,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);

  ierr = AOPetscToApplication(ao,4,getapp);CHKERRQ(ierr);
  ierr = AOApplicationToPetsc(ao,3,getpetsc);CHKERRQ(ierr);
  ierr = PetscSynchronizedPrintf(PETSC_COMM_WORLD,"[%d] 2,1,3,4 PetscToApplication %d %d %d %d\n",
          rank,getapp[0],getapp[1],getapp[2],getapp[3]);CHKERRQ(ierr);
  ierr = PetscSynchronizedPrintf(PETSC_COMM_WORLD,"[%d] 0,3,4 ApplicationToPetsc %d %d %d\n",
          rank,getpetsc[0],getpetsc[1],getpetsc[2]);CHKERRQ(ierr);
  ierr = PetscSynchronizedFlush(PETSC_COMM_WORLD);CHKERRQ(ierr);

  ierr = AODestroy(ao);CHKERRQ(ierr);
  ierr = PetscFinalize();CHKERRQ(ierr);
  return 0;
}
 


