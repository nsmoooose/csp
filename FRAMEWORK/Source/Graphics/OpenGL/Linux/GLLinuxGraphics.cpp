// CSP project
// OpenGL , just for Linux, should be used for all later
// watch your defines ;)

#ifdef OGL


//#include <GL/gl.h>			// Header File For The OpenGL32 Library
//#include <GL/glu.h>			// Header File For The GLu32 Library
//#include <GL/glx.h>
//#include <X11/extensions/xf86vmode.h>
//#include <X11/keysym.h>

//#include "SDL.h"



#include "GLGraphics.h"
#include "GLLinuxGraphics.h"
#include "GLGlobals.h"
#include "GLFont.h"
#include "GLTexture.h"

#include <math.h>			// Math Library Header File
#include <stdio.h>			// Header File For Standard Input/Output


///* attributes for a single buffered visual in RGBA format with at least
// * 4 bits per color and a 16 bit depth buffer */
//static int attrListSgl[] = {GLX_RGBA, GLX_RED_SIZE, 4, 
//    GLX_GREEN_SIZE, 4, 
//    GLX_BLUE_SIZE, 4, 
//    GLX_DEPTH_SIZE, 16,
//    None};

///* attributes for a double buffered visual in RGBA format with at least
// * 4 bits per color and a 16 bit depth buffer */
//static int attrListDbl[] = { GLX_RGBA, GLX_DOUBLEBUFFER, 
//    GLX_RED_SIZE, 4, 
//    GLX_GREEN_SIZE, 4, 
//    GLX_BLUE_SIZE, 4, 
//    GLX_DEPTH_SIZE, 16,
//    None };

void CheckGLAndSDLErrors()
{
  fprintf(stdout, "CheckGLAndSDLErrors()\n");
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

GLLinuxGraphics::GLLinuxGraphics()
{
  fprintf(stdout, "GLLinuxGraphics::GLLinuxGraphics()\n");
  //  m_depthFlag = TRUE;

}

GLLinuxGraphics::~GLLinuxGraphics()
{
  fprintf(stdout, "GLLinuxGraphics::~GLLinuxGraphics()\n");
  //  Kill3DWindow();
}


//GLvoid GLLinuxGraphics::Kill3DWindow(GLvoid)								// Properly Kill The Window

  //{

  //  if (GLWin.ctx)
  //    {
  //      if (!glXMakeCurrent(GLWin.dpy, None, NULL))
  //        {
  //            printf("Could not release drawing context.\n");
  //        }
  //      glXDestroyContext(GLWin.dpy, GLWin.ctx);
  //      GLWin.ctx = NULL;
  //    }
  /* switch back to original desktop resolution if we were in fs*/
//  if (GLWin.fs)
//    {
//      XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, &GLWin.deskMode);
//      XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
//    }
//  XCloseDisplay(GLWin.dpy);
//}



// we may this else where.
//short GLLinuxGraphics::Create3DWindow()
//{
//  int left = 0;
//  int top = 0;
//  int width = globals->width;
//  int height = globals->height;
//  int bits=24;
//  int fullscreenflag=1;
//  char * title = "GL Graphics";
//
//    XVisualInfo *vi;
//    int i;
//    int glxMajorVersion, glxMinorVersion;
//    int vidModeMajorVersion, vidModeMinorVersion;
//    XF86VidModeModeInfo **modes;
//    int modeNum;
//    int bestMode;
//    Atom wmDelete;
//    Window winDummy;
//    unsigned int borderDummy;
//    Colormap cmap;
//    int dpyWidth, dpyHeight;

//    GLWin.fs = fullscreenflag;
//    /* set best mode to current */
//    bestMode = 0;
//    /* get a connection */
//    GLWin.dpy = XOpenDisplay(0);
//    GLWin.screen = DefaultScreen(GLWin.dpy);
//    XF86VidModeQueryVersion(GLWin.dpy, &vidModeMajorVersion,
//        &vidModeMinorVersion);
//    printf("XF86VidModeExtension-Version %d.%d\n", vidModeMajorVersion,
//        vidModeMinorVersion);
//    XF86VidModeGetAllModeLines(GLWin.dpy, GLWin.screen, &modeNum, &modes);
//    /* save desktop-resolution before switching modes */
//    GLWin.deskMode = *modes[0];
//    /* look for mode with requested resolution */
//    for (i = 0; i < modeNum; i++)
//    {
//        if ((modes[i]->hdisplay == width) && (modes[i]->vdisplay == height))
//        {
//            bestMode = i;
//        }
//    }
//    /* get an appropriate visual */
//    vi = glXChooseVisual(GLWin.dpy, GLWin.screen, attrListDbl);
//    if (vi == NULL)
//    {
//        vi = glXChooseVisual(GLWin.dpy, GLWin.screen, attrListSgl);
//       GLWin.doubleBuffered = False;
//        printf("Only Singlebuffered Visual!\n");
//    }
//    else
//    {
//        GLWin.doubleBuffered = True;
//        printf("Got Doublebuffered Visual!\n");
//    }
//    glXQueryVersion(GLWin.dpy, &glxMajorVersion, &glxMinorVersion);
//    printf("glX-Version %d.%d\n", glxMajorVersion, glxMinorVersion);
//    /* create a GLX context */
//    GLWin.ctx = glXCreateContext(GLWin.dpy, vi, 0, GL_TRUE);
//
//    //    InitGL();

//    /* create a color map */
//    cmap = XCreateColormap(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
//        vi->visual, AllocNone);
//    GLWin.attr.colormap = cmap;
//    GLWin.attr.border_pixel = 0;
//
//    if (GLWin.fs)
//      {
//        XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, modes[bestMode]);
//        XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
//        dpyWidth = modes[bestMode]->hdisplay;
//        dpyHeight = modes[bestMode]->vdisplay;
//        printf("Resolution %dx%d\n", dpyWidth, dpyHeight);
//        XFree(modes);
//	
//	/* create a fullscreen window */
//	GLWin.attr.override_redirect = True;
//	GLWin.attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
//	  StructureNotifyMask;
//        GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
//				  0, 0, dpyWidth, dpyHeight, 0, vi->depth, InputOutput, vi->visual,
//				  CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
//				  &GLWin.attr);
//        XWarpPointer(GLWin.dpy, None, GLWin.win, 0, 0, 0, 0, 0, 0);
//	XMapRaised(GLWin.dpy, GLWin.win);
//        XGrabKeyboard(GLWin.dpy, GLWin.win, True, GrabModeAsync,
//		      GrabModeAsync, CurrentTime);
//        XGrabPointer(GLWin.dpy, GLWin.win, True, ButtonPressMask,
//		     GrabModeAsync, GrabModeAsync, GLWin.win, None, CurrentTime);
//      }
//    else
//      {
//        /* create a window in window mode*/
//        GLWin.attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
//	  StructureNotifyMask;
//        GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
//				  0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
//				  CWBorderPixel | CWColormap | CWEventMask, &GLWin.attr);
//        /* only set window title and handle wm_delete_events if in windowed mode */
//        wmDelete = XInternAtom(GLWin.dpy, "WM_DELETE_WINDOW", True);
//        XSetWMProtocols(GLWin.dpy, GLWin.win, &wmDelete, 1);
//        XSetStandardProperties(GLWin.dpy, GLWin.win, title,
//			       title, None, NULL, 0, NULL);
//        XMapRaised(GLWin.dpy, GLWin.win);
//      }
//    /* connect the glx-context to the window */
//    glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
//    XGetGeometry(GLWin.dpy, GLWin.win, &winDummy, &GLWin.x, &GLWin.y,
//		 &GLWin.width, &GLWin.height, &borderDummy, &GLWin.depth);
//    printf("Depth %d\n", GLWin.depth);
//    if (glXIsDirect(GLWin.dpy, GLWin.ctx))
//      printf("Congrats, you have Direct Rendering!\n");
//    else
//      printf("Sorry, no Direct Rendering possible!\n");
//    InitGL();
//    return 0;
//}




short GLLinuxGraphics::Initialize(StandardApp * app, _GRAPHICS_INIT_STRUCT * Init)										// All Setup For OpenGL Goes Here
{

  fprintf(stdout, "GLLinuxGraphics::Initialize()\n");

  //  glShadeModel(GL_SMOOTH);
  //  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  //  glClearDepth(1.0f);
  //  glEnable(GL_DEPTH_TEST);
  //  glDepthFunc(GL_LEQUAL);
  //  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  /* we use resizeGLScene once to set up our initial perspective */
  //  Resize3DWindow(GLWin.width, GLWin.height);
  //  glFlush();


  	glViewport(0,0,640,480);						// Reset The Current Viewport


	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
    glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
//	glClearColor( 0.0f, 0.0f, 0.0f , 1.0f);
//	glEnable( GL_FOG );
//	glFogi( GL_FOG_MODE, GL_LINEAR );
	StandardColor c(200, 300, 300 );
//	glFogfv( GL_FOG_COLOR, (float *)&c );
//	glFogf( GL_FOG_DENSITY, 0.00001f);
//	glFogf( GL_FOG_START, 5000.0f );
//	glFogf( GL_FOG_END, 25000.0f );
	glEnable( GL_TEXTURE_2D );
//	glEnable( GL_BLEND );
//	glEnable( GL_LIGHTING );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	m_depthFlag = 1;
//	glPolygonMode(GL_FRONT, GL_LINE);
//	glPolygonMode(GL_FRONT, GL_LINE);
//	glColor3f(1.0f, 0.0f, 0.0f);
	m_text = new GLFont();
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

void GLLinuxGraphics::Uninitialize()
{

}

void GLLinuxGraphics::Resize(short width, short height)		// Resize And Initialize The GL Window
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

void GLLinuxGraphics::Flip()
{

  fprintf(stdout, "GLLinuxGraphics::Flip()\n");
  //  if (GLWin.doubleBuffered)
  //    {
  //      glXSwapBuffers(GLWin.dpy, GLWin.win);
      //    }
 
  SDL_GL_SwapBuffers();

  CheckGLAndSDLErrors();


}

void GLLinuxGraphics::BeginScene()
{
  fprintf(stdout, "GLLinuxGraphics::BeginScene()\n");
  glBindTexture(GL_TEXTURE_2D, 0);
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_TEXTURE_2D);

  if (p_Camera)
    p_Camera->Apply();

  CheckGLAndSDLErrors();

}

void GLLinuxGraphics::EndScene()
{
  fprintf(stdout, "GLLinuxGraphics::EndScene()\n");
  CheckGLAndSDLErrors();
}


#endif


