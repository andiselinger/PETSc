/*$Id: psleep.c,v 1.29 2001/03/22 18:45:57 balay Exp balay $*/

#include "petsc.h"                 /*I   "petsc.h"    I*/
#if defined (PETSC_HAVE_UNISTD_H)
#include <unistd.h>
#endif
#if defined (PETSC_HAVE_STDLIB_H)
#include <stdlib.h>
#endif
#if defined (PETSC_HAVE_DOS_H)   /* borland */
#include <dos.h>
#endif
#include "petscfix.h"

#undef __FUNCT__  
#define __FUNCT__ "PetscSleep"
/*@
   PetscSleep - Sleeps some number of seconds.

   Not Collective

   Input Parameters:
.  s - number of seconds to sleep

   Notes:
      If s is negative waits for keyboard input

   Level: intermediate

   Concepts: sleeping
   Concepts: pause
   Concepts: waiting

@*/
int PetscSleep(int s)
{
  PetscFunctionBegin;
  if (s < 0) getc(stdin);
#if defined (PETSC_HAVE_U_SLEEP)
  else       _sleep(s*1000);
#else
  else       sleep(s);
#endif
  PetscFunctionReturn(0);
}

