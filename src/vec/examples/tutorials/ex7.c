/*$Id: ex7.c,v 1.32 2001/03/06 20:24:04 balay Exp balay $*/

static char help[] = "Demonstrates calling a Fortran computational routine from C.\n\
Also demonstrates passing  PETSc objects, MPI Communicators from C to Fortran\n\
and from Fortran to C\n\n";

#include "petscvec.h"

/*
  Ugly stuff to insure the function names match between Fortran 
  and C. Sorry, but this is out of our PETSc hands to cleanup.
*/
#if defined(PETSC_HAVE_FORTRAN_CAPS)
#define ex7f_ EX7F
#define ex7c_ EX7C
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE)
#define ex7f_ ex7f
#define ex7c_ ex7c
#endif
EXTERN_C_BEGIN
EXTERN void PETSC_STDCALL ex7f_(Vec *,int*);
EXTERN_C_END

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **args)
{
  int              ierr,m = 10;
  int              fcomm;
  Vec              vec;

  PetscInitialize(&argc,&args,(char *)0,help);

  /* This function should be called to be able to use PETSc routines
     from the FORTRAN subroutines needed by this program */

  PetscInitializeFortran();  

  ierr = VecCreate(PETSC_COMM_WORLD,PETSC_DECIDE,m,&vec);CHKERRQ(ierr);
  ierr = VecSetFromOptions(vec);CHKERRQ(ierr);

  /* 
     Call Fortran routine - the use of MPICCommToFortranComm() allows 
     translation of the MPI_Comm from C so that it can be properly 
     interpreted from Fortran.
  */
  ierr = MPICCommToFortranComm(PETSC_COMM_WORLD,&fcomm);CHKERRQ(ierr);

  ex7f_(&vec,&fcomm);

  ierr = VecView(vec,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);
  ierr = VecDestroy(vec);CHKERRQ(ierr);
  ierr = PetscFinalize();CHKERRQ(ierr);
  return 0;
}

EXTERN_C_BEGIN
#undef __FUNCT__
#define __FUNCT__ "ex7c_"
void PETSC_STDCALL ex7c_(Vec *fvec,int *fcomm,int* ierr)
{
  MPI_Comm comm;
  int size;

  /*
    Translate Fortran integer pointer back to C and
    Fortran Communicator back to C communicator
  */
  *ierr = MPIFortranCommToCComm(*fcomm,&comm);
  
  /* Some PETSc/MPI operations on Vec/Communicator objects */
  *ierr = VecGetSize(*fvec,&size);
  *ierr = MPI_Barrier(comm);

}
EXTERN_C_END
