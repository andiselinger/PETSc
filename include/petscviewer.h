/* $Id: viewer.h,v 1.55 1998/08/26 22:04:55 balay Exp bsmith $ */
/*
     Viewers are objects where other objects can be looked at or stored.
*/

#if !defined(__VIEWER_H)
#define __VIEWER_H

typedef struct _p_Viewer*            Viewer;

/*
    petsc.h must be included AFTER the definition of Viewer for ADIC to 
   process correctly.
*/
#include "petsc.h"

#define VIEWER_COOKIE              PETSC_COOKIE+1
typedef enum { MATLAB_VIEWER,ASCII_FILE_VIEWER, ASCII_FILES_VIEWER, 
               BINARY_FILE_VIEWER, STRING_VIEWER, DRAW_VIEWER,
               AMS_VIEWER} ViewerType;

extern int ViewerFileOpenASCII(MPI_Comm,const char[],Viewer*);
typedef enum {BINARY_RDONLY, BINARY_WRONLY, BINARY_CREATE} ViewerBinaryType;
extern int ViewerFileOpenBinary(MPI_Comm,const char[],ViewerBinaryType,Viewer*);
extern int ViewerMatlabOpen(MPI_Comm,const char[],int,Viewer*);
extern int ViewerStringOpen(MPI_Comm,char[],int, Viewer*);
extern int ViewerDrawOpenX(MPI_Comm,const char[],const char[],int,int,int,int,Viewer*);
extern int ViewerDrawOpenVRML(MPI_Comm,const char[],const char[],Viewer*);
extern int ViewerDrawClear(Viewer);


extern int ViewerGetType(Viewer,ViewerType*);
extern int ViewerDestroy(Viewer);

extern int ViewerASCIIGetPointer(Viewer,FILE**);

extern int ViewerBinaryGetDescriptor(Viewer,int*);
extern int ViewerBinaryGetInfoPointer(Viewer,FILE **);

#define VIEWER_FORMAT_ASCII_DEFAULT       0
#define VIEWER_FORMAT_ASCII_MATLAB        1
#define VIEWER_FORMAT_ASCII_IMPL          2
#define VIEWER_FORMAT_ASCII_INFO          3
#define VIEWER_FORMAT_ASCII_INFO_LONG     4
#define VIEWER_FORMAT_ASCII_COMMON        5
#define VIEWER_FORMAT_ASCII_SYMMODU       6
#define VIEWER_FORMAT_ASCII_INDEX         7

#define VIEWER_FORMAT_BINARY_DEFAULT      0
#define VIEWER_FORMAT_BINARY_NATIVE       1

#define VIEWER_FORMAT_DRAW_BASIC          0
#define VIEWER_FORMAT_DRAW_LG             1
#define VIEWER_FORMAT_DRAW_CONTOUR        2

extern int    ViewerSetFormat(Viewer,int,char[]);
extern int    ViewerPushFormat(Viewer,int,char[]);
extern int    ViewerPopFormat(Viewer);
extern int    ViewerGetFormat(Viewer,int*);

extern int    ViewerFlush(Viewer);
extern int    ViewerStringSPrintf(Viewer,char *,...);

extern Viewer VIEWER_STDOUT_SELF;  
extern Viewer VIEWER_STDERR_SELF;
extern Viewer VIEWER_STDOUT_WORLD;
extern Viewer VIEWER_STDOUT_(MPI_Comm);
extern int    VIEWER_STDOUT_Destroy(MPI_Comm);
extern Viewer VIEWER_STDERR_WORLD;
extern Viewer VIEWER_STDERR_(MPI_Comm);
extern int    VIEWER_STDERR_Destroy(MPI_Comm);
extern Viewer VIEWER_DRAWX_WORLD_PRIVATE_0;
extern Viewer VIEWER_DRAWX_WORLD_PRIVATE_1;
extern Viewer VIEWER_DRAWX_WORLD_PRIVATE_2;
extern Viewer VIEWER_DRAWX_SELF_PRIVATE; 
extern Viewer VIEWER_MATLAB_WORLD_PRIVATE;
extern Viewer VIEWER_MATLAB_SELF_PRIVATE;  /* not yet used */

extern int    ViewerInitializeDrawXWorld_Private_0(void);
extern int    ViewerInitializeDrawXWorld_Private_1(void);
extern int    ViewerInitializeDrawXWorld_Private_2(void);
extern int    ViewerInitializeDrawXSelf_Private(void);
extern int    ViewerInitializeMatlabWorld_Private(void);
extern Viewer VIEWER_DRAWX_(MPI_Comm);
extern int    VIEWER_DRAWX_Destroy(MPI_Comm);

#define VIEWER_DRAWX_WORLD_0 \
              (ViewerInitializeDrawXWorld_Private_0(),VIEWER_DRAWX_WORLD_PRIVATE_0) 
#define VIEWER_DRAWX_WORLD_1 \
              (ViewerInitializeDrawXWorld_Private_1(),VIEWER_DRAWX_WORLD_PRIVATE_1) 
#define VIEWER_DRAWX_WORLD_2 \
              (ViewerInitializeDrawXWorld_Private_2(),VIEWER_DRAWX_WORLD_PRIVATE_2) 

#define VIEWER_DRAWX_SELF \
              (ViewerInitializeDrawXSelf_Private(),VIEWER_DRAWX_SELF_PRIVATE) 
#define VIEWER_DRAWX_WORLD VIEWER_DRAWX_WORLD_0

#define VIEWER_MATLAB_WORLD \
        (ViewerInitializeMatlabWorld_Private(),VIEWER_MATLAB_WORLD_PRIVATE) 

extern int ViewersDrawOpenX(MPI_Comm,char *,char **,int,int,int,Viewer**);
extern int ViewersDestroy(int,Viewer[]);

#if defined(HAVE_AMS)
#include "ams.h"
extern int    ViewerAMSGetAMSComm(Viewer,AMS_Comm *);
extern int    ViewerAMSOpen(MPI_Comm,const char[],Viewer*);
extern Viewer VIEWER_AMS_(MPI_Comm);
extern int    VIEWER_AMS_Destroy(MPI_Comm);
extern Viewer VIEWER_AMS_WORLD_PRIVATE;
extern int    ViewerInitializeAMSWorld_Private(void);
#define VIEWER_AMS_WORLD (ViewerInitializeAMSWorld_Private(),VIEWER_AMS_WORLD_PRIVATE) 
#endif

#endif

