/*$Id: ex3.c,v 1.28 2001/03/23 01:39:24 bsmith Exp balay $*/

static char help[] = "Solves a linear system in parallel with SLES.  The matrix\n\
uses simple bilinear elements on the unit square.  To test the parallel\n\
matrix assembly, the matrix is intentionally laid out across processors\n\
differently from the way it is assembled.  Input arguments are:\n\
  -m <size> : problem size\n\n";

/*T
   Concepts: SLES^basic parallel example
   Concepts: Matrices^inserting elements by blocks
   Processors: n
T*/

/* 
  Include "petscsles.h" so that we can use SLES solvers.  Note that this file
  automatically includes:
     petsc.h       - base PETSc routines   petscvec.h - vectors
     petscsys.h    - system routines       petscmat.h - matrices
     petscis.h     - index sets            petscksp.h - Krylov subspace methods
     petscviewer.h - viewers               petscpc.h  - preconditioners
*/
#include "petscsles.h"

/* Declare user-defined routines */
extern int FormElementStiffness(double,Scalar*);
extern int FormElementRhs(double,double,double,Scalar*);

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **args)
{
  Vec     u,b,ustar; /* approx solution, RHS, exact solution */
  Mat     A;           /* linear system matrix */
  SLES    sles;        /* linear solver context */
  KSP     ksp;         /* Krylov subspace method context */
  IS      is;          /* index set - used for boundary conditions */
  int     N;           /* dimension of system (global) */
  int     M;           /* number of elements (global) */
  int     rank;        /* processor rank */
  int     size;        /* size of communicator */
  Scalar  Ke[16];      /* element matrix */
  Scalar  r[4];        /* element vector */
  double  h;           /* mesh width */
  double  norm;        /* norm of solution error */
  double  x,y;
  Scalar  val,zero = 0.0,one = 1.0,none = -1.0;
  int     ierr,idx[4],count,*rows,i,m = 5,start,end,its;

  PetscInitialize(&argc,&args,(char *)0,help);
  ierr = PetscOptionsGetInt(PETSC_NULL,"-m",&m,PETSC_NULL);CHKERRQ(ierr);
  N = (m+1)*(m+1);
  M = m*m;
  h = 1.0/m;
  ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRQ(ierr);
  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRQ(ierr);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
         Compute the matrix and right-hand-side vector that define
         the linear system, Au = b.
     - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* 
     Create stiffness matrix
  */
  ierr = MatCreate(PETSC_COMM_WORLD,PETSC_DECIDE,PETSC_DECIDE,N,N,&A);CHKERRQ(ierr);
  ierr = MatSetFromOptions(A);CHKERRQ(ierr);
  start = rank*(M/size) + ((M%size) < rank ? (M%size) : rank);
  end   = start + M/size + ((M%size) > rank); 

  /*
     Assemble matrix
  */
  ierr = FormElementStiffness(h*h,Ke);
  for (i=start; i<end; i++) {
     /* location of lower left corner of element */
     x = h*(i % m); y = h*(i/m); 
     /* node numbers for the four corners of element */
     idx[0] = (m+1)*(i/m) + (i % m);
     idx[1] = idx[0]+1; idx[2] = idx[1] + m + 1; idx[3] = idx[2] - 1;
     ierr = MatSetValues(A,4,idx,4,idx,Ke,ADD_VALUES);CHKERRQ(ierr);
  }
  ierr = MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);

  /*
     Create right-hand-side and solution vectors
  */
  ierr = VecCreate(PETSC_COMM_WORLD,PETSC_DECIDE,N,&u);CHKERRQ(ierr); 
  ierr = VecSetFromOptions(u);CHKERRQ(ierr);
  ierr = PetscObjectSetName((PetscObject)u,"Approx. Solution");CHKERRQ(ierr);
  ierr = VecDuplicate(u,&b);CHKERRQ(ierr);
  ierr = PetscObjectSetName((PetscObject)b,"Right hand side");CHKERRQ(ierr);
  ierr = VecDuplicate(b,&ustar);CHKERRQ(ierr);
  ierr = VecSet(&zero,u);CHKERRQ(ierr);
  ierr = VecSet(&zero,b);CHKERRQ(ierr);

  /* 
     Assemble right-hand-side vector
  */
  for (i=start; i<end; i++) {
     /* location of lower left corner of element */
     x = h*(i % m); y = h*(i/m); 
     /* node numbers for the four corners of element */
     idx[0] = (m+1)*(i/m) + (i % m);
     idx[1] = idx[0]+1; idx[2] = idx[1] + m + 1; idx[3] = idx[2] - 1;
     ierr = FormElementRhs(x,y,h*h,r);CHKERRQ(ierr);
     ierr = VecSetValues(b,4,idx,r,ADD_VALUES);CHKERRQ(ierr);
  }
  ierr = VecAssemblyBegin(b);CHKERRQ(ierr);
  ierr = VecAssemblyEnd(b);CHKERRQ(ierr);

  /* 
     Modify matrix and right-hand-side for Dirichlet boundary conditions
  */
  ierr = PetscMalloc(4*m*sizeof(int),&rows);CHKERRQ(ierr);
  for (i=0; i<m+1; i++) {
    rows[i] = i; /* bottom */
    rows[3*m - 1 +i] = m*(m+1) + i; /* top */
  }
  count = m+1; /* left side */
  for (i=m+1; i<m*(m+1); i+= m+1) {
    rows[count++] = i;
  }
  count = 2*m; /* left side */
  for (i=2*m+1; i<m*(m+1); i+= m+1) {
    rows[count++] = i;
  }
  ierr = ISCreateGeneral(PETSC_COMM_SELF,4*m,rows,&is);CHKERRQ(ierr);
  for (i=0; i<4*m; i++) {
     x = h*(rows[i] % (m+1)); y = h*(rows[i]/(m+1)); 
     val = y;
     ierr = VecSetValues(u,1,&rows[i],&val,INSERT_VALUES);CHKERRQ(ierr);
     ierr = VecSetValues(b,1,&rows[i],&val,INSERT_VALUES);CHKERRQ(ierr);
  }    
  ierr = PetscFree(rows);CHKERRQ(ierr);
  ierr = VecAssemblyBegin(u);CHKERRQ(ierr);
  ierr = VecAssemblyEnd(u);CHKERRQ(ierr);
  ierr = VecAssemblyBegin(b);CHKERRQ(ierr); 
  ierr = VecAssemblyEnd(b);CHKERRQ(ierr);

  ierr = MatZeroRows(A,is,&one);CHKERRQ(ierr);
  ierr = ISDestroy(is);CHKERRQ(ierr);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
                Create the linear solver and set various options
     - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ierr = SLESCreate(PETSC_COMM_WORLD,&sles);CHKERRQ(ierr);
  ierr = SLESSetOperators(sles,A,A,DIFFERENT_NONZERO_PATTERN);CHKERRQ(ierr);
  ierr = SLESGetKSP(sles,&ksp);CHKERRQ(ierr);
  ierr = KSPSetInitialGuessNonzero(ksp);CHKERRQ(ierr);
  ierr = SLESSetFromOptions(sles);CHKERRQ(ierr);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
                      Solve the linear system
     - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ierr = SLESSolve(sles,b,u,&its);CHKERRQ(ierr);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
                      Check solution and clean up
     - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check error */
  ierr = VecGetOwnershipRange(ustar,&start,&end);CHKERRQ(ierr);
  for (i=start; i<end; i++) {
     x = h*(i % (m+1)); y = h*(i/(m+1)); 
     val = y;
     ierr = VecSetValues(ustar,1,&i,&val,INSERT_VALUES);CHKERRQ(ierr);
  }
  ierr = VecAssemblyBegin(ustar);CHKERRQ(ierr);
  ierr = VecAssemblyEnd(ustar);CHKERRQ(ierr);
  ierr = VecAXPY(&none,ustar,u);CHKERRQ(ierr);
  ierr = VecNorm(u,NORM_2,&norm);CHKERRQ(ierr);
  ierr = PetscPrintf(PETSC_COMM_WORLD,"Norm of error %A Iterations %d\n",norm*h,its);CHKERRQ(ierr);

  /* 
     Free work space.  All PETSc objects should be destroyed when they
     are no longer needed.
  */
  ierr = SLESDestroy(sles);CHKERRQ(ierr); ierr = VecDestroy(u);CHKERRQ(ierr);
  ierr = VecDestroy(ustar);CHKERRQ(ierr); ierr = VecDestroy(b);CHKERRQ(ierr);
  ierr = MatDestroy(A);CHKERRQ(ierr);

  /*
     Always call PetscFinalize() before exiting a program.  This routine
       - finalizes the PETSc libraries as well as MPI
       - provides summary and diagnostic information if certain runtime
         options are chosen (e.g., -log_summary).
  */
  ierr = PetscFinalize();CHKERRQ(ierr);
  return 0;
}

/* --------------------------------------------------------------------- */
#undef __FUNCT__
#define __FUNCT__ "FormElementStiffness"
   /* element stiffness for Laplacian */
int FormElementStiffness(double H,Scalar *Ke)
{
  PetscFunctionBegin;
  Ke[0]  = H/6.0;    Ke[1]  = -.125*H; Ke[2]  = H/12.0;   Ke[3]  = -.125*H;
  Ke[4]  = -.125*H;  Ke[5]  = H/6.0;   Ke[6]  = -.125*H;  Ke[7]  = H/12.0;
  Ke[8]  = H/12.0;   Ke[9]  = -.125*H; Ke[10] = H/6.0;    Ke[11] = -.125*H;
  Ke[12] = -.125*H;  Ke[13] = H/12.0;  Ke[14] = -.125*H;  Ke[15] = H/6.0;
  PetscFunctionReturn(0);
}
/* --------------------------------------------------------------------- */
#undef __FUNCT__
#define __FUNCT__ "FormElementRhs"
int FormElementRhs(double x,double y,double H,Scalar *r)
{
  PetscFunctionBegin;
  r[0] = 0.; r[1] = 0.; r[2] = 0.; r[3] = 0.0; 
  PetscFunctionReturn(0);
}
