/* $Id: xops.c,v 1.142 1999/11/05 14:43:58 bsmith Exp $*/

/*
    Defines the operations for the X Draw implementation.
*/

#include "src/sys/src/draw/impls/x/ximpl.h"         /*I  "petsc.h" I*/

/*
     These macros transform from the users coordinates to the 
   X-window pixel coordinates.
*/
#define XTRANS(win,xwin,x) \
   (int)(((xwin)->w)*((win)->port_xl + (((x - (win)->coor_xl)*\
                                   ((win)->port_xr - (win)->port_xl))/\
                                   ((win)->coor_xr - (win)->coor_xl))))
#define YTRANS(win,xwin,y) \
   (int)(((xwin)->h)*(1.0-(win)->port_yl - (((y - (win)->coor_yl)*\
                                       ((win)->port_yr - (win)->port_yl))/\
                                       ((win)->coor_yr - (win)->coor_yl))))

#undef __FUNC__  
#define __FUNC__ "DrawLine_X" 
int DrawLine_X(Draw Win, double xl, double yl, double xr, double yr,int cl)
{
  Draw_X* XiWin = (Draw_X*) Win->data;
  int     x1,y_1,x2,y2;

  PetscFunctionBegin;
  XiSetColor( XiWin, cl );
  x1 = XTRANS(Win,XiWin,xl);   x2  = XTRANS(Win,XiWin,xr); 
  y_1 = YTRANS(Win,XiWin,yl);   y2  = YTRANS(Win,XiWin,yr); 
  XDrawLine( XiWin->disp, XiDrawable(XiWin), XiWin->gc.set, x1, y_1, x2, y2);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawPoint_X" 
static int DrawPoint_X(Draw Win,double x,double  y,int c)
{
  int     xx,yy;
  Draw_X* XiWin = (Draw_X*) Win->data;

  PetscFunctionBegin;
  xx = XTRANS(Win,XiWin,x);  yy = YTRANS(Win,XiWin,y);
  XiSetColor( XiWin, c );
  XDrawPoint( XiWin->disp, XiDrawable(XiWin), XiWin->gc.set,xx, yy);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawRectangle_X" 
static int DrawRectangle_X(Draw Win, double xl, double yl, double xr, double yr,
                           int c1, int c2,int c3,int c4)
{
  Draw_X* XiWin = (Draw_X*) Win->data;
  int     x1,y_1,w,h, c = (c1 + c2 + c3 + c4)/4;

  PetscFunctionBegin;
  XiSetColor( XiWin, c );
  x1 = XTRANS(Win,XiWin,xl);   w  = XTRANS(Win,XiWin,xr) - x1; 
  y_1 = YTRANS(Win,XiWin,yr);   h  = YTRANS(Win,XiWin,yl) - y_1;
  if (w <= 0) w = 1; if (h <= 0) h = 1;
  XFillRectangle( XiWin->disp, XiDrawable(XiWin), XiWin->gc.set, x1, y_1, w, h);
  PetscFunctionReturn(0);
}

extern int XiDrawInterpolatedTriangle(Draw_X*,int,int,int,int,int,int,int,int,int);

#undef __FUNC__  
#define __FUNC__ "DrawTriangle_X" 
static int DrawTriangle_X(Draw Win, double X1, double Y_1, double X2, 
                          double Y2,double X3,double Y3, int c1, int c2,int c3)
{
  Draw_X* XiWin = (Draw_X*) Win->data;
  int     ierr;

  PetscFunctionBegin;
  if (c1 == c2 && c2 == c3) {
    XPoint pt[3];
    XiSetColor( XiWin, c1 );
    pt[0].x = XTRANS(Win,XiWin,X1);
    pt[0].y = YTRANS(Win,XiWin,Y_1); 
    pt[1].x = XTRANS(Win,XiWin,X2);
    pt[1].y = YTRANS(Win,XiWin,Y2); 
    pt[2].x = XTRANS(Win,XiWin,X3);
    pt[2].y = YTRANS(Win,XiWin,Y3); 
    XFillPolygon(XiWin->disp,XiDrawable(XiWin),XiWin->gc.set,pt,3,Convex,CoordModeOrigin);
  } else {
    int x1,y_1,x2,y2,x3,y3;
    x1   = XTRANS(Win,XiWin,X1);
    y_1  = YTRANS(Win,XiWin,Y_1); 
    x2   = XTRANS(Win,XiWin,X2);
    y2   = YTRANS(Win,XiWin,Y2); 
    x3   = XTRANS(Win,XiWin,X3);
    y3   = YTRANS(Win,XiWin,Y3); 
    ierr = XiDrawInterpolatedTriangle(XiWin,x1,y_1,c1,x2,y2,c2,x3,y3,c3);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawString_X" 
static int DrawString_X(Draw Win,double x,double  y,int c,char *chrs )
{
  int     xx,yy,ierr,len;
  Draw_X* XiWin = (Draw_X*) Win->data;
  char*   substr;

  PetscFunctionBegin;
  xx = XTRANS(Win,XiWin,x);  yy = YTRANS(Win,XiWin,y);
  XiSetColor( XiWin, c );
  
  ierr = PetscStrtok(chrs,"\n",&substr);CHKERRQ(ierr);
  ierr = PetscStrlen(substr,&len);CHKERRQ(ierr);
  XDrawString( XiWin->disp, XiDrawable(XiWin), XiWin->gc.set,
               xx, yy - XiWin->font->font_descent, substr,len  );
  ierr = PetscStrtok(0,"\n",&substr);CHKERRQ(ierr);
  while (substr) {
    yy  += 4*XiWin->font->font_descent;
    ierr = PetscStrlen(substr,&len);CHKERRQ(ierr);
    XDrawString( XiWin->disp, XiDrawable(XiWin), XiWin->gc.set,
                 xx, yy - XiWin->font->font_descent, substr, len );
    ierr = PetscStrtok(0,"\n",&substr);CHKERRQ(ierr);
  }

  PetscFunctionReturn(0);
}

int XiFontFixed( Draw_X*,int, int,XiFont **);

#undef __FUNC__  
#define __FUNC__ "DrawStringSetSize_X" 
static int DrawStringSetSize_X(Draw Win,double x,double  y)
{
  Draw_X* XiWin = (Draw_X*) Win->data;
  int     ierr,w,h;

  PetscFunctionBegin;
  w = (int)((XiWin->w)*x*(Win->port_xr - Win->port_xl)/(Win->coor_xr - Win->coor_xl));
  h = (int)((XiWin->h)*y*(Win->port_yr - Win->port_yl)/(Win->coor_yr - Win->coor_yl));
  ierr = PetscFree(XiWin->font);CHKERRQ(ierr);
  ierr = XiFontFixed( XiWin,w, h, &XiWin->font);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawStringGetSize_X" 
int DrawStringGetSize_X(Draw Win,double *x,double  *y)
{
  Draw_X* XiWin = (Draw_X*) Win->data;
  double  w,h;

  PetscFunctionBegin;
  w = XiWin->font->font_w; h = XiWin->font->font_h;
  *x = w*(Win->coor_xr - Win->coor_xl)/(XiWin->w)*(Win->port_xr - Win->port_xl);
  *y = h*(Win->coor_yr - Win->coor_yl)/(XiWin->h)*(Win->port_yr - Win->port_yl);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawStringVertical_X" 
int DrawStringVertical_X(Draw Win,double x,double  y,int c,char *chrs )
{
  int     xx,yy,n,i,ierr;
  Draw_X* XiWin = (Draw_X*) Win->data;
  char    tmp[2];
  double  tw,th;
  
  PetscFunctionBegin;
  ierr   = PetscStrlen(chrs,&n);CHKERRQ(ierr);
  tmp[1] = 0;
  XiSetColor( XiWin, c );
  ierr = DrawStringGetSize_X(Win,&tw,&th);CHKERRQ(ierr);
  xx = XTRANS(Win,XiWin,x);
  for ( i=0; i<n; i++ ) {
    tmp[0] = chrs[i];
    yy = YTRANS(Win,XiWin,y-th*i);
    XDrawString( XiWin->disp, XiDrawable(XiWin), XiWin->gc.set,
                xx, yy - XiWin->font->font_descent, tmp, 1 );
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawFlush_X" 
static int DrawFlush_X(Draw Win )
{
  Draw_X* XiWin = (Draw_X*) Win->data;

  PetscFunctionBegin;
  if (XiWin->drw) {
    XCopyArea( XiWin->disp,XiWin->drw,XiWin->win,XiWin->gc.set,0,0,XiWin->w,XiWin->h,0,0);
  }
  XFlush( XiWin->disp ); XSync(XiWin->disp,False);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawSynchronizedFlush_X" 
static int DrawSynchronizedFlush_X(Draw Win )
{
  int     rank,ierr;
  Draw_X* XiWin = (Draw_X*) Win->data;

  PetscFunctionBegin;
  XFlush( XiWin->disp );
  if (XiWin->drw) {
    ierr = MPI_Comm_rank(Win->comm,&rank);CHKERRQ(ierr);
    /* make sure data has actually arrived at server */
    XSync(XiWin->disp,False);
    ierr = MPI_Barrier(Win->comm);CHKERRQ(ierr);
    if (!rank) {
      XCopyArea(XiWin->disp,XiWin->drw,XiWin->win,XiWin->gc.set,0,0,XiWin->w,XiWin->h,0,0);
      XFlush( XiWin->disp );
    }
    XSync(XiWin->disp,False);
    ierr = MPI_Barrier(Win->comm);CHKERRQ(ierr);
  } else {
    ierr = MPI_Barrier(Win->comm);CHKERRQ(ierr);
    XSync(XiWin->disp,False);
    ierr = MPI_Barrier(Win->comm);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawSetViewport_X" 
static int DrawSetViewport_X(Draw Win,double xl,double yl,double xr,double yr)
{
  Draw_X*    XiWin = (Draw_X*) Win->data;
  XRectangle box;

  PetscFunctionBegin;
  box.x = (int) (xl*XiWin->w);   box.y = (int) ((1.0-yr)*XiWin->h);
  box.width = (int) ((xr-xl)*XiWin->w);box.height = (int) ((yr-yl)*XiWin->h);
  XSetClipRectangles(XiWin->disp,XiWin->gc.set,0,0,&box,1,Unsorted);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawClear_X" 
static int DrawClear_X(Draw Win)
{
  Draw_X*  XiWin = (Draw_X*) Win->data;
  int      x,  y,  w,  h;

  PetscFunctionBegin;
  x = (int) (Win->port_xl*XiWin->w);
  w = (int) ((Win->port_xr - Win->port_xl)*XiWin->w);
  y = (int) ((1.0-Win->port_yr)*XiWin->h);
  h = (int) ((Win->port_yr - Win->port_yl)*XiWin->h);
  XiSetPixVal(XiWin, XiWin->background );
  XFillRectangle(XiWin->disp,XiDrawable(XiWin),XiWin->gc.set, x, y, w, h);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawSynchronizedClear_X" 
static int DrawSynchronizedClear_X(Draw Win)
{
  int     rank,ierr;
  Draw_X* XiWin = (Draw_X*) Win->data;

  PetscFunctionBegin;
  ierr = MPI_Barrier(Win->comm);CHKERRQ(ierr);
  ierr = MPI_Comm_rank(Win->comm,&rank);CHKERRQ(ierr);
  if (!rank) {
    ierr = DrawClear_X(Win);CHKERRQ(ierr);
  }
  XFlush( XiWin->disp );
  ierr = MPI_Barrier(Win->comm);CHKERRQ(ierr);
  XSync(XiWin->disp,False);
  ierr = MPI_Barrier(Win->comm);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawSetDoubleBuffer_X" 
static int DrawSetDoubleBuffer_X(Draw Win)
{
  Draw_X*  win = (Draw_X*) Win->data;
  int      rank,ierr;

  PetscFunctionBegin;
  if (win->drw) PetscFunctionReturn(0);

  ierr = MPI_Comm_rank(Win->comm,&rank);CHKERRQ(ierr);
  if (!rank) {
    win->drw = XCreatePixmap(win->disp,win->win,win->w,win->h,win->depth);
  }
  /* try to make sure it is actually done before passing info to all */
  XSync(win->disp,False);
  ierr = MPI_Bcast(&win->drw,1,MPI_UNSIGNED_LONG,0,Win->comm);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#include <X11/cursorfont.h>

#undef __FUNC__  
#define __FUNC__ "DrawGetMouseButton_X" 
static int DrawGetMouseButton_X(Draw draw,DrawButton *button,double* x_user,
                                double *y_user,double *x_phys,double *y_phys)
{
  XEvent       report;
  Draw_X*      win = (Draw_X*) draw->data;
  Window       root, child;
  int          root_x, root_y,px,py;
  unsigned int keys_button;
  Cursor       cursor = 0;

  PetscFunctionBegin;
  /* change cursor to indicate input */
  if (!cursor) {
    cursor = XCreateFontCursor(win->disp,XC_hand2); 
    if (!cursor) SETERRQ(PETSC_ERR_LIB,1,"Unable to create X cursor");
  }
  XDefineCursor(win->disp, win->win, cursor);

  XSelectInput( win->disp, win->win, ButtonPressMask | ButtonReleaseMask );

  while (XCheckTypedEvent( win->disp, ButtonPress, &report ));
  XMaskEvent( win->disp, ButtonReleaseMask, &report );
  switch (report.xbutton.button) {
    case Button1: *button = BUTTON_LEFT;   break;
    case Button2: *button = BUTTON_CENTER; break;
    case Button3: *button = BUTTON_RIGHT;  break;
  }
  XQueryPointer(win->disp, report.xmotion.window,&root,&child,&root_x,&root_y,
                &px,&py,&keys_button);

  if (x_phys) *x_phys = ((double) px)/((double) win->w);
  if (y_phys) *y_phys = 1.0 - ((double) py)/((double) win->h);

  if (x_user) *x_user = draw->coor_xl + ((((double) px)/((double) win->w)-draw->port_xl))*
                        (draw->coor_xr - draw->coor_xl)/(draw->port_xr - draw->port_xl);
  if (y_user) *y_user = draw->coor_yl + 
                        ((1.0 - ((double) py)/((double) win->h)-draw->port_yl))*
                        (draw->coor_yr - draw->coor_yl)/(draw->port_yr - draw->port_yl);

  XUndefineCursor(win->disp, win->win);
  XFlush( win->disp ); XSync(win->disp,False);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawPause_X" 
static int DrawPause_X(Draw draw)
{
  int ierr;

  PetscFunctionBegin;
  if (draw->pause > 0) PetscSleep(draw->pause);
  else if (draw->pause < 0) {
    DrawButton button;
    int        rank;
    ierr = MPI_Comm_rank(draw->comm,&rank);CHKERRQ(ierr);
    if (!rank) {
      ierr = DrawGetMouseButton(draw,&button,0,0,0,0);CHKERRQ(ierr);
      if (button == BUTTON_CENTER) draw->pause = 0;
    }
    ierr = MPI_Bcast(&draw->pause,1,MPI_INT,0,draw->comm);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawGetPopup_X" 
static int DrawGetPopup_X(Draw draw,Draw *popup)
{
  int     ierr;
  Draw_X* win = (Draw_X*) draw->data;

  PetscFunctionBegin;
  ierr = DrawOpenX(draw->comm,PETSC_NULL,PETSC_NULL,win->x,win->y+win->h+36,150,220,popup);CHKERRQ(ierr);
  draw->popup = *popup;
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawSetTitle_X" 
static int DrawSetTitle_X(Draw draw,char *title)
{
  Draw_X        *win = (Draw_X *) draw->data;
  XTextProperty prop;
  int           ierr,len;

  PetscFunctionBegin;
  XGetWMName(win->disp,win->win,&prop);
  prop.value  = (unsigned char *)title; 
  ierr        = PetscStrlen(title,&len);CHKERRQ(ierr);
  prop.nitems = (long) len;
  XSetWMName(win->disp,win->win,&prop); 
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawResizeWindow_X"
static int DrawResizeWindow_X(Draw draw,int w,int h)
{
  Draw_X       *win = (Draw_X *) draw->data;
  unsigned int ww, hh, border, depth;
  int          x,y;
  int          ierr;
  Window       root;

  PetscFunctionBegin;
  XResizeWindow(win->disp,win->win,w,h);
  XGetGeometry(win->disp,win->win,&root,&x,&y,&ww,&hh,&border,&depth);
  ierr = DrawCheckResizedWindow(draw);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawCheckResizedWindow_X" 
static int DrawCheckResizedWindow_X(Draw draw)
{
  Draw_X       *win = (Draw_X *) draw->data;
  int          x,y,rank,ierr;
  Window       root;
  unsigned int w, h, border, depth,geo[2];
  double       xl,xr,yl,yr;
  XRectangle   box;

  PetscFunctionBegin;
  ierr = MPI_Comm_rank(draw->comm,&rank);CHKERRQ(ierr);
  if (!rank) {
    XSync(win->disp,False);
    XGetGeometry(win->disp,win->win,&root,&x,&y,geo,geo+1,&border,&depth);
  }
  ierr = MPI_Bcast(geo,2,MPI_INT,0,draw->comm);CHKERRQ(ierr);
  w = geo[0]; 
  h = geo[1];
  if (w == win->w && h == win->h) PetscFunctionReturn(0);

  /* record new window sizes */

  win->h = h; win->w = w;

  /* Free buffer space and create new version (only first processor does this) */
  if (win->drw) {
    win->drw = XCreatePixmap(win->disp,win->win,win->w,win->h,win->depth);
  }
  /* reset the clipping */
  xl = draw->port_xl; yl = draw->port_yl;
  xr = draw->port_xr; yr = draw->port_yr;
  box.x     = (int) (xl*win->w);     box.y      = (int) ((1.0-yr)*win->h);
  box.width = (int) ((xr-xl)*win->w);box.height = (int) ((yr-yl)*win->h);
  XSetClipRectangles(win->disp,win->gc.set,0,0,&box,1,Unsorted);

  /* try to make sure it is actually done before passing info to all */
  XSync(win->disp,False);
  ierr = MPI_Bcast(&win->drw,1,MPI_UNSIGNED_LONG,0,draw->comm);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

static int DrawGetSingleton_X(Draw,Draw*);
static int DrawRestoreSingleton_X(Draw,Draw*);

#undef __FUNC__  
#define __FUNC__ "DrawDestroy_X" 
int DrawDestroy_X(Draw draw)
{
  Draw_X *win = (Draw_X *) draw->data;
  int    ierr;

  PetscFunctionBegin;
  XFreeGC(win->disp,win->gc.set);
  XCloseDisplay(win->disp);
  if (draw->popup) {ierr = DrawDestroy(draw->popup);CHKERRQ(ierr);}
  ierr = PetscFree(win->font);CHKERRQ(ierr);
  ierr = PetscFree(win);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

static struct _DrawOps DvOps = { DrawSetDoubleBuffer_X,
                                 DrawFlush_X,DrawLine_X,
                                 0,
                                 0,
                                 DrawPoint_X,
                                 0,
                                 DrawString_X,
                                 DrawStringVertical_X,
                                 DrawStringSetSize_X,
                                 DrawStringGetSize_X,
                                 DrawSetViewport_X,
                                 DrawClear_X,
                                 DrawSynchronizedFlush_X,
                                 DrawRectangle_X,
                                 DrawTriangle_X,
                                 DrawGetMouseButton_X,
                                 DrawPause_X,
                                 DrawSynchronizedClear_X, 
				 0, 
                                 0,
                                 DrawGetPopup_X,
                                 DrawSetTitle_X,
                                 DrawCheckResizedWindow_X,
                                 DrawResizeWindow_X,
                                 DrawDestroy_X,
                                 0,
                                 DrawGetSingleton_X,
                                 DrawRestoreSingleton_X };


extern int XiQuickWindow(Draw_X*,char*,char*,int,int,int,int);
extern int XiQuickWindowFromWindow(Draw_X*,char*,Window);

#undef __FUNC__  
#define __FUNC__ "DrawGetSingleton_X" 
static int DrawGetSingleton_X(Draw draw,Draw *sdraw)
{
  int      ierr;
  Draw_X   *Xwin = (Draw_X*)draw->data,*sXwin;

  PetscFunctionBegin;

  ierr = DrawCreate(PETSC_COMM_SELF,draw->display,draw->title,draw->x,draw->y,draw->w,draw->h,sdraw);CHKERRQ(ierr);
  ierr = PetscObjectChangeTypeName((PetscObject)*sdraw,DRAW_X);CHKERRQ(ierr);
  ierr = PetscMemcpy((*sdraw)->ops,&DvOps,sizeof(DvOps));CHKERRQ(ierr);
  (*sdraw)->ops->destroy = 0;

  (*sdraw)->pause   = draw->pause;
  (*sdraw)->coor_xl = draw->coor_xl;  
  (*sdraw)->coor_xr = draw->coor_xr;
  (*sdraw)->coor_yl = draw->coor_yl;
  (*sdraw)->coor_yr = draw->coor_yr;
  (*sdraw)->port_xl = draw->port_xl;
  (*sdraw)->port_xr = draw->port_xr;
  (*sdraw)->port_yl = draw->port_yl;
  (*sdraw)->port_yr = draw->port_yr;
  (*sdraw)->popup   = draw->popup;

  /* actually create and open the window */
  sXwin = (Draw_X *) PetscMalloc( sizeof(Draw_X) );CHKPTRQ(Xwin);
  ierr  = PetscMemzero(sXwin,sizeof(Draw_X));CHKERRQ(ierr);

  ierr = XiQuickWindowFromWindow( sXwin,draw->display, Xwin->win);CHKERRQ(ierr);

  sXwin->x       = Xwin->x;
  sXwin->y       = Xwin->y;
  sXwin->w       = Xwin->w;
  sXwin->h       = Xwin->h;
  (*sdraw)->data = (void *) sXwin;
 PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawRestoreSingleton_X" 
static int DrawRestoreSingleton_X(Draw draw,Draw *sdraw)
{
  int      ierr;
  Draw_X   *sXwin = (Draw_X*)(*sdraw)->data;

  XFreeGC(sXwin->disp,sXwin->gc.set);
  XCloseDisplay(sXwin->disp);
  if ((*sdraw)->popup)   {ierr = DrawDestroy((*sdraw)->popup);CHKERRQ(ierr);}
  if ((*sdraw)->title)   {ierr = PetscFree((*sdraw)->title);CHKERRQ(ierr);}
  if ((*sdraw)->display) {ierr = PetscFree((*sdraw)->display);CHKERRQ(ierr);}
  ierr = PetscFree(sXwin->font);CHKERRQ(ierr);
  ierr = PetscFree(sXwin);CHKERRQ(ierr);
  PetscHeaderDestroy(*sdraw);
  PetscFunctionBegin;
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawXGetDisplaySize_Private" 
int DrawXGetDisplaySize_Private(const char name[],int *width,int *height)
{
  Display *display;

  PetscFunctionBegin;
  display = XOpenDisplay( name );
  if (!display) {
    *width  = 0; 
    *height = 0; 
    SETERRQ1(1,1,"Unable to open display on %s\n.  Make sure your DISPLAY variable\n\
    is set, or you use the -display name option and xhost + has been\n\
    run on your displaying machine.\n",name);
  }

  *width  = DisplayWidth(display,0);
  *height = DisplayHeight(display,0);

  XCloseDisplay(display);
  PetscFunctionReturn(0);
}

EXTERN_C_BEGIN
#undef __FUNC__  
#define __FUNC__ "DrawCreate_X" 
int DrawCreate_X(Draw draw)
{
  Draw_X     *Xwin;
  int        ierr,size,rank,xywh[4],osize = 4;
  int        x = draw->x, y = draw->y, w = draw->w, h = draw->h;
  static int xavailable = 0,yavailable = 0,xmax = 0,ymax = 0, ybottom = 0;
  PetscTruth flg;

  PetscFunctionBegin;
  if (!draw->display) {
    draw->display = (char *) PetscMalloc(128*sizeof(char));CHKPTRQ(draw->display);
    ierr          = PetscGetDisplay(draw->display,128);CHKERRQ(ierr);
  }

  /*
      Initialize the display size
  */
  if (xmax == 0) {
    ierr = DrawXGetDisplaySize_Private(draw->display,&xmax,&ymax);
    /* if some processors fail on this and others succed then this is a problem ! */
    if (ierr) {
       (*PetscErrorPrintf)("PETSc unable to use X windows\nproceeding without graphics\n");
       ierr = DrawSetType(draw,DRAW_NULL);CHKERRQ(ierr);
       PetscFunctionReturn(0);
    }
  }

  if (w == PETSC_DECIDE) w = draw->w = 300;
  if (h == PETSC_DECIDE) h = draw->h = 300; 
  switch (w) {
    case DRAW_FULL_SIZE: w = draw->w = xmax - 10;
                         break;
    case DRAW_HALF_SIZE: w = draw->w = (xmax - 20)/2;
                         break;
    case DRAW_THIRD_SIZE: w = draw->w = (xmax - 30)/3;
                         break;
    case DRAW_QUARTER_SIZE: w = draw->w = (xmax - 40)/4;
                         break;
  }
  switch (h) {
    case DRAW_FULL_SIZE: h = draw->h = ymax - 10;
                         break;
    case DRAW_HALF_SIZE: h = draw->h = (ymax - 20)/2;
                         break;
    case DRAW_THIRD_SIZE: h = draw->h = (ymax - 30)/3;
                         break;
    case DRAW_QUARTER_SIZE: h = draw->h = (ymax - 40)/4;
                         break;
  }

  /* allow user to set location and size of window */
  xywh[0] = x; xywh[1] = y; xywh[2] = w; xywh[3] = h;
  ierr = OptionsGetIntArray(PETSC_NULL,"-geometry",xywh,&osize,PETSC_NULL);CHKERRQ(ierr);
  x = xywh[0]; y = xywh[1]; w = xywh[2]; h = xywh[3];


  if (draw->x == PETSC_DECIDE || draw->y == PETSC_DECIDE) {
    /*
       PETSc tries to place windows starting in the upper left corner and 
       moving across to the right. 
    
              --------------------------------------------
              |  Region used so far +xavailable,yavailable |
              |                     +                      |
              |                     +                      |
              |++++++++++++++++++++++ybottom               |
              |                                            |
              |                                            |
              |--------------------------------------------|
    */
    /*  First: can we add it to the right? */
    if (xavailable+w+10 <= xmax) {
      x       = xavailable;
      y       = yavailable;
      ybottom = PetscMax(ybottom,y + h + 30);
    } else {
      /* No, so add it below on the left */
      xavailable = 0;
      x          = 0;
      yavailable = ybottom;    
      y          = ybottom;
      ybottom    = ybottom + h + 30;
    }
  }
  /* update available region */
  xavailable = PetscMax(xavailable,x + w + 10);
  if (xavailable >= xmax) {
    xavailable = 0;
    yavailable = yavailable + h + 30;
    ybottom    = yavailable;
  }
  if (yavailable >= ymax) {
    y          = 0;
    yavailable = 0;
    ybottom    = 0;
  }

  ierr = PetscMemcpy(draw->ops,&DvOps,sizeof(DvOps));CHKERRQ(ierr);
  draw->pause   = 0;
  draw->coor_xl = 0.0;  
  draw->coor_xr = 1.0;
  draw->coor_yl = 0.0;  
  draw->coor_yr = 1.0;
  draw->port_xl = 0.0;  
  draw->port_xr = 1.0;
  draw->port_yl = 0.0;  
  draw->port_yr = 1.0;
  draw->popup   = 0;

  ierr = OptionsGetInt(PETSC_NULL,"-draw_pause",&draw->pause,PETSC_NULL);CHKERRQ(ierr);

  /* actually create and open the window */
  Xwin         = (Draw_X *) PetscMalloc( sizeof(Draw_X) );CHKPTRQ(Xwin);
  PLogObjectMemory(draw,sizeof(Draw_X)+sizeof(struct _p_Draw));
  ierr = PetscMemzero(Xwin,sizeof(Draw_X));CHKERRQ(ierr);
  ierr = MPI_Comm_size(draw->comm,&size);CHKERRQ(ierr);
  ierr = MPI_Comm_rank(draw->comm,&rank);CHKERRQ(ierr);

  if (!rank) {
    if (x < 0 || y < 0)   SETERRQ(PETSC_ERR_ARG_OUTOFRANGE,0,"Negative corner of window");
    if (w <= 0 || h <= 0) SETERRQ(PETSC_ERR_ARG_OUTOFRANGE,0,"Negative window width or height");
    ierr = XiQuickWindow(Xwin,draw->display,draw->title,x,y,w,h);CHKERRQ(ierr);
    ierr = MPI_Bcast(&Xwin->win,1,MPI_UNSIGNED_LONG,0,draw->comm);CHKERRQ(ierr);
  } else {
    unsigned long win;
    ierr = MPI_Bcast(&win,1,MPI_UNSIGNED_LONG,0,draw->comm);CHKERRQ(ierr);
    ierr = XiQuickWindowFromWindow( Xwin,draw->display, win);CHKERRQ(ierr);
  }

  Xwin->x      = x;
  Xwin->y      = y;
  Xwin->w      = w;
  Xwin->h      = h;
  draw->data    = (void *) Xwin;

  /*
    Need barrier here so processor 0 doesn't destroy the window before other 
    processors have completed XiQuickWindow()
  */
  ierr = DrawClear(draw);CHKERRQ(ierr);
  ierr = DrawSynchronizedFlush(draw);CHKERRQ(ierr);

  ierr = OptionsHasName(PETSC_NULL,"-draw_double_buffer",&flg);CHKERRQ(ierr);
  if (flg) {
     ierr = DrawSetDoubleBuffer(draw);CHKERRQ(ierr);
  } 

  PetscFunctionReturn(0);
}
EXTERN_C_END









