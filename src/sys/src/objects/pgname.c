/*$Id: pgname.c,v 1.25 2001/01/19 20:53:45 bsmith Exp balay $*/

#include "petsc.h"        /*I    "petsc.h"   I*/

#undef __FUNCT__  
#define __FUNCT__ "PetscObjectGetName"
/*@C
   PetscObjectGetName - Gets a string name associated with a PETSc object.

   Not Collective

   Input Parameters:
+  obj - the Petsc variable
         Thus must be cast with a (PetscObject), for example, 
         PetscObjectGetName((PetscObject)mat,&name);
-  name - the name associated with obj

   Level: intermediate

   Concepts: object name

.seealso: PetscObjectSetName()
@*/
int PetscObjectGetName(PetscObject obj,char *name[])
{
  int ierr;

  PetscFunctionBegin;
  if (!obj) SETERRQ(PETSC_ERR_ARG_CORRUPT,"Null object");
  if (!name) SETERRQ(PETSC_ERR_ARG_BADPTR,"Void location for name");
  if (!obj->name) {
    ierr = PetscObjectName(obj);CHKERRQ(ierr);
  }
  *name = obj->name;
  PetscFunctionReturn(0);
}

