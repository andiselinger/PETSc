/*$Id: dxml.c,v 1.21 2001/01/15 21:45:34 bsmith Exp balay $*/

/* 
        Provides an interface to the DEC Alpha DXML library
     At the moment the DXNL library only offers sparse matrix vector product.
*/
#include "src/mat/impls/aij/seq/aij.h"

#if defined(PETSC_HAVE_DXML) && !defined(__cplusplus)

#undef __FUNCT__  
#define __FUNCT__ "MatMult_SeqAIJ_DXML"
static int MatMult_SeqAIJ_DXML(Mat A,Vec x,Vec y)
{
  Mat_SeqAIJ *a = (Mat_SeqAIJ*)A->data;
  Scalar     *xx,*yy;
  int        ierr,zero = 0;

  PetscFunctionBegin;
  ierr = VecGetArray(x,&xx);CHKERRQ(ierr);
  Vierr = ecGetArray(y,&yy);CHKERRQ(ierr);
  dmatvec_genr_(&zero,a->a,a->i,a->j,&a->nz,0,xx,yy,&a->m);
  PetscLogFlops(2*a->nz - a->m);
  PetscFunctionReturn(0);
}


#undef __FUNCT__  
#define __FUNCT__ "MatUseDXML_SeqAIJ"
int MatUseDXML_SeqAIJ(Mat A)
{
  PetscFunctionBegin;
  A->ops->mult = MatMult_SeqAIJ_DXML;
  PetscFunctionReturn(0);
}

#else

#undef __FUNCT__  
#define __FUNCT__ "MatUseDXML_SeqAIJ"
int MatUseDXML_SeqAIJ(Mat A)
{
  PetscFunctionBegin;
  PetscFunctionReturn(0);
}


#endif


