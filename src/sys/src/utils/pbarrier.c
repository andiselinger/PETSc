/*$Id: pbarrier.c,v 1.15 2001/01/15 21:44:00 bsmith Exp balay $*/

#include "petsc.h"              /*I "petsc.h" I*/

#undef __FUNCT__  
#define __FUNCT__ "PetscBarrier"
/*@C
    PetscBarrier - Blocks until this routine is executed by all
                   processors owning the object A.

   Input Parameters:
.  A - PETSc object  (Mat, Vec, IS, SNES etc...)
        Must be caste with a (PetscObject), can use PETSC_NULL (for MPI_COMM_WORLD)

  Level: intermediate

  Notes: 
  This routine calls MPI_Barrier with the communicator of the PETSc Object "A". 

   Concepts: barrier

@*/
int PetscBarrier(PetscObject obj)
{
  int      ierr;
  MPI_Comm comm;

  PetscFunctionBegin;
  if (obj) PetscValidHeader(obj); 
  ierr = PetscLogEventBegin(Petsc_Barrier,obj,0,0,0);CHKERRQ(ierr);
  if (obj) {
    ierr = PetscObjectGetComm(obj,&comm);CHKERRQ(ierr);
  } else {
    comm = PETSC_COMM_WORLD;
  }
  ierr = MPI_Barrier(comm);CHKERRQ(ierr);
  ierr = PetscLogEventEnd(Petsc_Barrier,obj,0,0,0);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

