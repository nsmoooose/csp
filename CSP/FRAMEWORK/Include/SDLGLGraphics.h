#ifdef OGL

#ifndef __GLSDLGRAPHICS_H_
#define __GLSDLGRAPHICS_H_


#include "Framework.h"
#include "GLGraphics.h"



/** The OpenGL-class for 3D-rendering.*/
class SDLGLGraphics : public GLGraphics 
{
 public:
  
  SDLGLGraphics();
  virtual ~SDLGLGraphics();

  /** @name Window Management routines*/
  ///@{

  short   Initialize(StandardApp * app, _GRAPHICS_INIT_STRUCT * init);
  void    Uninitialize();

  virtual void   Resize(short width, short height);
  virtual void   Flip();

  virtual void BeginScene();
  virtual void EndScene();


private:

};

#endif

#endif
