/*$Id: is.c,v 1.13 2001/01/16 18:18:01 balay Exp balay $*/
/*
    Creates a matrix class for using the Neumann-Neumann type preconditioners.
   This stores the matrices in globally unassembled form. Each processor 
   assembles only its local Neumann problem and the parallel matrix vector 
   product is handled "implicitly".

     We provide:
         MatMult()

    Currently this allows for only one subdomain per processor.

*/

#include "src/mat/impls/is/is.h"      /*I "mat.h" I*/

#undef __FUNCT__  
#define __FUNCT__ "MatDestroy_IS" 
int MatDestroy_IS(Mat A)
{
  int    ierr;
  Mat_IS *b = (Mat_IS*)A->data;

  PetscFunctionBegin;
  if (b->A) {
    ierr = MatDestroy(b->A);CHKERRQ(ierr);
  }
  if (b->ctx) {
    ierr = VecScatterDestroy(b->ctx);CHKERRQ(ierr);
  }
  if (b->x) {
    ierr = VecDestroy(b->x);CHKERRQ(ierr);
  }
  if (b->y) {
    ierr = VecDestroy(b->y);CHKERRQ(ierr);
  }
  if (b->mapping) {
    ierr = ISLocalToGlobalMappingDestroy(b->mapping);CHKERRQ(ierr);
  }
  ierr = PetscFree(b);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "MatMult_IS" 
int MatMult_IS(Mat A,Vec x,Vec y)
{
  int    ierr;
  Mat_IS *is = (Mat_IS*)A->data;
  Scalar zero = 0.0;

  PetscFunctionBegin;
  /*  scatter the global vector x into the local work vector */
  ierr = VecScatterBegin(x,is->x,INSERT_VALUES,SCATTER_FORWARD,is->ctx);CHKERRQ(ierr);
  ierr = VecScatterEnd(x,is->x,INSERT_VALUES,SCATTER_FORWARD,is->ctx);CHKERRQ(ierr);

  /* multiply the local matrix */
  ierr = MatMult(is->A,is->x,is->y);CHKERRQ(ierr);

  /* scatter product back into global memory */
  ierr = VecSet(&zero,y);CHKERRQ(ierr);
  ierr = VecScatterBegin(is->y,y,ADD_VALUES,SCATTER_REVERSE,is->ctx);CHKERRQ(ierr);
  ierr = VecScatterEnd(is->y,y,ADD_VALUES,SCATTER_REVERSE,is->ctx);CHKERRQ(ierr);

  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "MatSetLocalToGlobalMapping_IS" 
int MatSetLocalToGlobalMapping_IS(Mat A,ISLocalToGlobalMapping mapping)
{
  int    ierr,n;
  Mat_IS *is = (Mat_IS*)A->data;
  IS     from,to;
  Vec    global;

  PetscFunctionBegin;
  is->mapping = mapping;
  ierr = PetscObjectReference((PetscObject)mapping);CHKERRQ(ierr);

  /* Create the local matrix A */
  ierr = ISLocalToGlobalMappingGetSize(mapping,&n);CHKERRQ(ierr);
  ierr = MatCreate(PETSC_COMM_SELF,n,n,n,n,&is->A);CHKERRQ(ierr);
  ierr = PetscObjectSetOptionsPrefix((PetscObject)is->A,"is");CHKERRQ(ierr);
  ierr = MatSetFromOptions(is->A);CHKERRQ(ierr);

  /* Create the local work vectors */
  ierr = VecCreateSeq(PETSC_COMM_SELF,n,&is->x);CHKERRQ(ierr);
  ierr = VecDuplicate(is->x,&is->y);CHKERRQ(ierr);

  /* setup the global to local scatter */
  ierr = ISCreateStride(PETSC_COMM_SELF,n,0,1,&to);CHKERRQ(ierr);
  ierr = ISLocalToGlobalMappingApplyIS(mapping,to,&from);CHKERRQ(ierr);
  ierr = VecCreateMPI(A->comm,A->n,A->N,&global);CHKERRQ(ierr);
  ierr = VecScatterCreate(global,from,is->x,to,&is->ctx);CHKERRQ(ierr);
  ierr = VecDestroy(global);CHKERRQ(ierr);
  ierr = ISDestroy(to);CHKERRQ(ierr);
  ierr = ISDestroy(from);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}


#undef __FUNCT__  
#define __FUNCT__ "MatSetValuesLocal_IS" 
int MatSetValuesLocal_IS(Mat A,int m,int *rows,int n,int *cols,Scalar *values,InsertMode addv)
{
  int    ierr;
  Mat_IS *is = (Mat_IS*)A->data;

  PetscFunctionBegin;
  ierr = MatSetValues(is->A,m,rows,n,cols,values,addv);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "MatZeroRowsLocal_IS" 
int MatZeroRowsLocal_IS(Mat A,IS isrows,Scalar *diag)
{
  Mat_IS *is = (Mat_IS*)A->data;
  int    ierr,i,n,*rows;
  Scalar *array;

  PetscFunctionBegin;

  {
    /*
       Set up is->x as a "counting vector". This is in order to MatMult_IS
       work properly in the interface nodes.
    */
    Vec    counter;
    Scalar one=1.0, zero=0.0;
    ierr = VecCreateMPI(A->comm,A->n,A->N,&counter);CHKERRQ(ierr);
    ierr = VecSet(&zero,counter);CHKERRQ(ierr);
    ierr = VecSet(&one,is->x);CHKERRQ(ierr);
    ierr = VecScatterBegin(is->x,counter,ADD_VALUES,SCATTER_REVERSE,is->ctx);CHKERRQ(ierr);
    ierr = VecScatterEnd  (is->x,counter,ADD_VALUES,SCATTER_REVERSE,is->ctx);CHKERRQ(ierr);
    ierr = VecScatterBegin(counter,is->x,INSERT_VALUES,SCATTER_FORWARD,is->ctx);CHKERRQ(ierr);
    ierr = VecScatterEnd  (counter,is->x,INSERT_VALUES,SCATTER_FORWARD,is->ctx);CHKERRQ(ierr);
    ierr = VecDestroy(counter);CHKERRQ(ierr);
  }
  ierr = ISGetLocalSize(isrows,&n);CHKERRQ(ierr);
  if (n == 0) {
    is->pure_neumann = PETSC_TRUE;
  } else {
    is->pure_neumann = PETSC_FALSE;
    ierr = ISGetIndices(isrows,&rows);CHKERRQ(ierr);
    ierr = VecGetArray(is->x,&array);CHKERRQ(ierr);
    ierr = MatZeroRows(is->A,isrows,diag);CHKERRQ(ierr);
    for (i=0; i<n; i++) {
      ierr = MatSetValue(is->A,rows[i],rows[i],(*diag)/(array[rows[i]]),INSERT_VALUES);CHKERRQ(ierr);
    }
    ierr = MatAssemblyBegin(is->A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
    ierr = MatAssemblyEnd  (is->A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
    ierr = VecRestoreArray(is->x,&array);CHKERRQ(ierr);
    ierr = ISRestoreIndices(isrows,&rows);CHKERRQ(ierr);
  }

  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "MatAssemblyBegin_IS" 
int MatAssemblyBegin_IS(Mat A,MatAssemblyType type)
{
  Mat_IS *is = (Mat_IS*)A->data;
  int    ierr;
  PetscFunctionBegin;
  ierr = MatAssemblyBegin(is->A,type);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "MatAssemblyEnd_IS" 
int MatAssemblyEnd_IS(Mat A,MatAssemblyType type)
{
  Mat_IS *is = (Mat_IS*)A->data;
  int    ierr;
  PetscFunctionBegin;
  ierr = MatAssemblyEnd(is->A,type);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "MatCreate_IS" 
int MatCreate_IS(Mat A)
{
  int    ierr;
  Mat_IS *b;

  PetscFunctionBegin;
  ierr                = PetscNew(Mat_IS,&b);CHKERRQ(ierr);
  A->data             = (void*)b;
  ierr = PetscMemzero(b,sizeof(Mat_IS));CHKERRQ(ierr);
  ierr = PetscMemzero(A->ops,sizeof(struct _MatOps));CHKERRQ(ierr);
  A->factor           = 0;
  A->mapping          = 0;

  A->ops->mult                    = MatMult_IS;
  A->ops->destroy                 = MatDestroy_IS;
  A->ops->setlocaltoglobalmapping = MatSetLocalToGlobalMapping_IS;
  A->ops->setvalueslocal          = MatSetValuesLocal_IS;
  A->ops->zerorowslocal           = MatZeroRowsLocal_IS;
  A->ops->assemblybegin           = MatAssemblyBegin_IS;
  A->ops->assemblyend             = MatAssemblyEnd_IS;

  ierr = PetscSplitOwnership(A->comm,&A->m,&A->M);CHKERRQ(ierr);
  ierr = PetscSplitOwnership(A->comm,&A->n,&A->N);CHKERRQ(ierr);
  ierr = MPI_Scan(&A->m,&b->rend,1,MPI_INT,MPI_SUM,A->comm);CHKERRQ(ierr);
  b->rstart = b->rend - A->m;

  b->A          = 0;
  b->ctx        = 0;
  b->x          = 0;  
  b->y          = 0;  

  PetscFunctionReturn(0);
}
EXTERN_C_END













