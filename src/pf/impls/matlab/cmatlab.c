/*$Id: cmatlab.c,v 1.6 2001/01/15 21:49:45 bsmith Exp balay $*/
#include "src/pf/pfimpl.h"            /*I "petscpf.h" I*/

/*
        Ths PF generates a Matlab function on the fly
*/
typedef struct {
  int               dimin,dimout;
  PetscMatlabEngine engine;
  char              *string;
} PF_Matlab;
  
#undef __FUNCT__  
#define __FUNCT__ "PFView_Matlab"
int PFView_Matlab(void *value,PetscViewer viewer)
{
  int        ierr;
  PetscTruth isascii;
  PF_Matlab  *matlab = (PF_Matlab*)value;

  PetscFunctionBegin;
  ierr = PetscTypeCompare((PetscObject)viewer,PETSC_VIEWER_ASCII,&isascii);CHKERRQ(ierr);
  if (isascii) {
    ierr = PetscViewerASCIIPrintf(viewer,"Matlab Matlab = %s\n",matlab->string);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "PFDestroy_Matlab"
int PFDestroy_Matlab(void *value)
{
  int        ierr;
  PF_Matlab  *matlab = (PF_Matlab*)value;

  PetscFunctionBegin;
  ierr = PetscStrfree(matlab->string);CHKERRQ(ierr);
  ierr = PetscMatlabEngineDestroy(matlab->engine);CHKERRQ(ierr);
  ierr = PetscFree(matlab);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "PFApply_Matlab"
int PFApply_Matlab(void *value,int n,Scalar *in,Scalar *out)
{
  PF_Matlab  *matlab = (PF_Matlab*)value;
  int        ierr;

  PetscFunctionBegin;
  if (!value) SETERRQ(1,"Need to set string for Matlab function, via -pf_matlab string");
  ierr = PetscMatlabEnginePutArray(matlab->engine,matlab->dimin,n,in,"x");CHKERRQ(ierr);
  ierr = PetscMatlabEngineEvaluate(matlab->engine,matlab->string);CHKERRQ(ierr);
  ierr = PetscMatlabEngineGetArray(matlab->engine,matlab->dimout,n,out,"f");CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "PFSetFromOptions_Matlab"
int PFSetFromOptions_Matlab(PF pf)
{
  int        ierr;
  PetscTruth flag;
  char       value[256];
  PF_Matlab  *matlab = (PF_Matlab*)pf->data;

  PetscFunctionBegin;
  ierr = PetscOptionsHead("Matlab function options");CHKERRQ(ierr);
    ierr = PetscOptionsString("-pf_matlab","Matlab function","None","",value,256,&flag);CHKERRQ(ierr);
    if (flag) {
      ierr = PetscStrallocpy((char*)value,&matlab->string);CHKERRQ(ierr);
    }
  ierr = PetscOptionsTail();CHKERRQ(ierr);
  PetscFunctionReturn(0);    
}


EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "PFCreate_Matlab"
int PFCreate_Matlab(PF pf,void *value)
{
  int       ierr;
  PF_Matlab *matlab;

  PetscFunctionBegin;
  ierr           = PetscNew(PF_Matlab,&matlab);CHKERRQ(ierr);
  matlab->dimin  = pf->dimin;
  matlab->dimout = pf->dimout;

  ierr = PetscMatlabEngineCreate(pf->comm,PETSC_NULL,&matlab->engine);CHKERRQ(ierr);
    
  if (value) {
    ierr = PetscStrallocpy((char*)value,&matlab->string);CHKERRQ(ierr);
  }
  ierr   = PFSet(pf,PFApply_Matlab,PETSC_NULL,PFView_Matlab,PFDestroy_Matlab,matlab);CHKERRQ(ierr);

  pf->ops->setfromoptions = PFSetFromOptions_Matlab;
  PetscFunctionReturn(0);
}
EXTERN_C_END





