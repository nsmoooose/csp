// CSP project
// OpenGL , just for Linux, should be used for all later
// watch your defines ;)
// $Revision: 1.1 $

#ifdef OGL

#include "Framework.h"


//#include <GL/gl.h>			// Header File For The OpenGL32 Library
//#include <GL/glu.h>			// Header File For The GLu32 Library
//#include <GL/glx.h>
//#include "SDL.h"



#include "GLGraphics.h"
#include "SDLGLGraphics.h"
#include "GLGlobals.h"
#include "GLFont.h"
#include "GLTexture.h"

#include <math.h>			// Math Library Header File
#include <stdio.h>			// Header File For Standard Input/Output


void CheckGLAndSDLErrors()
{
#ifdef TRACE_GRAPHICS
  fprintf(stdout, "CheckGLAndSDLErrors()\n");
#endif
  int gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    {
      fprintf(stderr, "GL error detected: %d\n", gl_error);
      exit(0);
    }

  char *  sdl_error = SDL_GetError();
  if (sdl_error[0] != '\0') 
    {
      fprintf(stderr, "SDL error detected: %s\n", sdl_error);
      SDL_ClearError();
      exit(0);
    }

}

bool active=false;

SDLGLGraphics::SDLGLGraphics()
{
#ifdef TRACE_GRAPHICS
  fprintf(stdout, "SDLGLGraphics::SDLGLGraphics()\n");
#endif
}

SDLGLGraphics::~SDLGLGraphics()
{
#ifdef TRACE_GRAPHICS
  fprintf(stdout, "SDLGLGraphics::~SDLGLGraphics()\n");
#endif
}

short SDLGLGraphics::Initialize(StandardApp * app, _GRAPHICS_INIT_STRUCT * Init)										// All Setup For OpenGL Goes Here
{
#ifdef TRACE_GRAPHICS
  fprintf(stdout, "SDLGLGraphics::Initialize()\n");
#endif

  memcpy(&p_Init, Init, sizeof(p_Init));


  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glMatrixMode( GL_TEXTURE );
  glLoadIdentity();
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
  glEnable( GL_TEXTURE_2D );
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LEQUAL );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE );
  m_depthFlag = 1;

  glViewport(0,0,p_Init.Width,p_Init.Height);		

  m_text = new GLFont(this);
  m_text->Create( "Font.tga" ,2);

  if (Init->Wireframe)
    SetRenderState(FW_WIREFRAME, true);
  else
    SetRenderState(FW_WIREFRAME, false);

  SetRenderState(FW_ZBUFFER, true);

  CheckGLAndSDLErrors();

  return _A_OK;
  								// Initialization Went OK

}

void SDLGLGraphics::Uninitialize()
{

}

void SDLGLGraphics::Resize(short width, short height)		// Resize And Initialize The GL Window
{ 

  //	if (height==0)										// Prevent A Divide By Zero By
  //	{
  //		height=1;										// Making Height Equal One
  //	}

  //	glViewport(0,0,width,height);						// Reset The Current Viewport
  //
  //	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
  //	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
  //	gluPerspective(30.0f,(GLfloat)width/(GLfloat)height,p_Init.NearZ,p_Init.FarZ);

  //	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
  //	glLoadIdentity();									// Reset The Modelview Matrix

}


///////////////////////////////////////////
// whole buffer routines

void SDLGLGraphics::Flip()
{
#ifdef TRACE_GRAPHICS
  fprintf(stdout, "SDLGLGraphics::Flip()\n");
#endif 

  SDL_GL_SwapBuffers();

  CheckGLAndSDLErrors();


}

void SDLGLGraphics::BeginScene()
{
#ifdef TRACE_GRAPHICS
  fprintf(stdout, "SDLGLGraphics::BeginScene()\n");
#endif

  glBindTexture(GL_TEXTURE_2D, 0);
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_TEXTURE_2D);
  //  glColor3f(1.0, 0.0, 0.0);
  //  glShadeModel(GL_SMOOTH);
  glViewport(0,0,p_Init.Width,p_Init.Height);

  if (p_Camera)
    p_Camera->Apply();

  CheckGLAndSDLErrors();

}

void SDLGLGraphics::EndScene()
{
#ifdef TRACE_GRAPHICS
  fprintf(stdout, "SDLGLGraphics::EndScene()\n");
#endif

  CheckGLAndSDLErrors();
}


#endif


