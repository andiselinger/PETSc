/*$Id: ex3.c,v 1.16 2001/01/17 22:20:33 bsmith Exp balay $*/

static char help[] = "Tests catching of floating point exceptions.\n\n";

#include "petsc.h"

#undef __FUNCT__
#define __FUNCT__ "CreateError"
int CreateError(double x)
{
  int ierr;
  PetscFunctionBegin;
  x = 1.0/x;
  ierr = PetscPrintf(PETSC_COMM_SELF,"x = %g\n",x);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **argv)
{
  int ierr;
  PetscInitialize(&argc,&argv,(char *)0,help);
  ierr = PetscPrintf(PETSC_COMM_SELF,"This is a contrived example to test floating pointing\n");CHKERRQ(ierr);
  ierr = PetscPrintf(PETSC_COMM_SELF,"It is not a true error.\n");CHKERRQ(ierr);
  ierr = PetscPrintf(PETSC_COMM_SELF,"Run with -fp_trap to catch the floating point error\n");CHKERRQ(ierr);
  ierr = CreateError(0.0);CHKERRQ(ierr);
  return 0;
}
 
