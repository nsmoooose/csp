#ifdef OGL

#ifndef __GLLINUXGRAPHICS_H_
#define __GLLINUXGRAPHICS_H_


#include "Framework.h"
#include "GLGraphics.h"


// we need this elsewhere.
//typedef struct {
//  Display *dpy;
//  int screen;
//  Window win;
//  GLXContext ctx;
//  XSetWindowAttributes attr;
//  Bool fs;
//  Bool doubleBuffered;
//  XF86VidModeModeInfo deskMode;
//  int x, y;
//  unsigned int width, height;
//  unsigned int depth;    
//} GLWindow;



/** The OpenGL-class for 3D-rendering.*/
class GLLinuxGraphics : public GLGraphics 
{
 public:
  
  GLLinuxGraphics();
  virtual ~GLLinuxGraphics();

  /** @name Window Management routines*/
  ///@{

  short   Initialize(StandardApp * app, _GRAPHICS_INIT_STRUCT * init);
  void    Uninitialize();

  virtual void   Resize(short width, short height);
  virtual void   Flip();

  virtual void BeginScene();
  virtual void EndScene();


private:

  //  GLWindow	GLWin;

};

#endif

#endif
