// CSP project
// OpenGL , just for Linux, should be used for all later
// watch your defines ;)
#ifdef OGL

#include "graphics/StandardGraphics.h"
#include "OpenGLGraphics.h"

#include <math.h>			// Math Library Header File
#include <stdio.h>			// Header File For Standard Input/Output
#include <GL/gl.h>			// Header File For The OpenGL32 Library
#include <GL/glu.h>			// Header File For The GLu32 Library
#ifdef WIN32
  #include <windows.h>		// Header File For Windows
  #include <GL/glaux.h>		// Header File For The Glaux Library
#endif
#ifdef LINUX
  #include <GL/glx.h>
  #include <X11/extensions/xf86vmode.h>
  #include <X11/keysym.h>
#endif


#ifdef _USRDLL
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
#endif


/* attributes for a single buffered visual in RGBA format with at least
 * 4 bits per color and a 16 bit depth buffer */
static int attrListSgl[] = {GLX_RGBA, GLX_RED_SIZE, 4, 
    GLX_GREEN_SIZE, 4, 
    GLX_BLUE_SIZE, 4, 
    GLX_DEPTH_SIZE, 16,
    None};

/* attributes for a double buffered visual in RGBA format with at least
 * 4 bits per color and a 16 bit depth buffer */
static int attrListDbl[] = { GLX_RGBA, GLX_DOUBLEBUFFER, 
    GLX_RED_SIZE, 4, 
    GLX_GREEN_SIZE, 4, 
    GLX_BLUE_SIZE, 4, 
    GLX_DEPTH_SIZE, 16,
    None };


bool active=false;
/*
LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
				UINT	uMsg,			// Message For This Window
				WPARAM	wParam,			// Additional Message Information
				LPARAM	lParam)			// Additional Message Information
{
  switch (uMsg)									// Check For Windows Messages
    {
    case WM_ACTIVATE:							// Watch For Window Activate Message
      {
	if (!HIWORD(wParam))					// Check Minimization State
	  {
	    active=TRUE;						// Program Is Active
	  }
	else
	  {
	    active=FALSE;						// Program Is No Longer Active
	  }
	
	return 0;								// Return To The Message Loop
      }
      
    case WM_SYSCOMMAND:							// Intercept System Commands
      {
	switch (wParam)							// Check System Calls
	  {
	  case SC_SCREENSAVE:					// Screensaver Trying To Start?
	  case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
	    return 0;							// Prevent From Happening
	  }
	break;									// Exit
      }

    case WM_CLOSE:								// Did We Receive A Close Message?
      {
	//		KillWindow();
	PostQuitMessage(0);						// Send A Quit Message
	return 0;								// Jump Back
      }
      
    case WM_KEYDOWN:							// Is A Key Being Held Down?
      {
	//			m_keys[wParam] = TRUE;					// If So, Mark It As TRUE
	return 0;								// Jump Back
      }
      
    case WM_KEYUP:								// Has A Key Been Released?
      {
	//			m_keys[wParam] = FALSE;					// If So, Mark It As FALSE
	return 0;								// Jump Back
      }
      
    case WM_SIZE:								// Resize The OpenGL Window
      {
	//			ResizeWindow(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
      }
      
    }

  // Pass All Unhandled Messages To DefWindowProc
  return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

BOOL OpenGLGraphics::processMessage()
{
  if (::PeekMessage(&m_msgCur, m_hWnd, NULL, NULL, PM_REMOVE))
    {
      if (m_msgCur.message == WM_CLOSE)
	{
	  Kill3DWindow();
	}
      else
	{
	  ::TranslateMessage(&m_msgCur);
	  ::DispatchMessage(&m_msgCur);
	}
    }
  
  return TRUE;

  //	while(!done)									// Loop That Runs While done=FALSE
  //	{
  //		if (PeekMessage(&msg,m_hWnd,0,0,PM_REMOVE))	// Is There A Message Waiting?
  //		{
  //			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
//			{
//				done=TRUE;							// If So done=TRUE
//			}
//			else									// If Not, Deal With Window Messages
//			{
//				TranslateMessage(&msg);				// Translate The Message
//				DispatchMessage(&msg);				// Dispatch The Message
//			}
//		}
//	}

}
*/


OpenGLGraphics::OpenGLGraphics()
{
#ifdef WIN32
  m_fullscreen=false;
  m_hDC=NULL;		// Private GDI Device Context
  m_hRC=NULL;		// Permanent Rendering Context
  m_hWnd=NULL;		// Holds Our Window Handle
  m_hInstance=NULL;		// Holds The Instance Of The Application
  
  m_depthFlag = FALSE;
#endif
#ifdef LINUX
  m_depthFlag = TRUE;
#endif
}

OpenGLGraphics::~OpenGLGraphics()
{
  Kill3DWindow();
}


GLvoid OpenGLGraphics::Kill3DWindow(GLvoid)								// Properly Kill The Window
{
#ifdef WIN32
  if (m_fullscreen)		// Are We In Fullscreen Mode?
    {
      ChangeDisplaySettings(NULL,0);// If So Switch Back To The Desktop
      ShowCursor(TRUE);	// Show Mouse Pointer
    }
  
  if (m_hRC) // Do We Have A Rendering Context?
    {
      if (!wglMakeCurrent(NULL,NULL))// Are We Able To Release The DC And RC Contexts?
	{
	  MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
	}
      
      if (!wglDeleteContext(m_hRC))// Are We Able To Delete The RC?
	{
	  MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
	}
      m_hRC=NULL;// Set RC To NULL
    }
  
  if (m_hDC && !ReleaseDC(m_hWnd,m_hDC))// Are We Able To Release The DC
    {
      MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
      m_hDC=NULL;// Set DC To NULL
    }
  
  if (m_hWnd && !DestroyWindow(m_hWnd))	// Are We Able To Destroy The Window?
    {
      MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
      m_hWnd=NULL;// Set hWnd To NULL
    }
  
  if (!UnregisterClass("OpenGL",m_hInstance))// Are We Able To Unregister Class
    {
      MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
      m_hInstance=NULL;	// Set hInstance To NULL
    }
#endif
#ifdef LINUX
  if (GLWin.ctx)
    {
      if (!glXMakeCurrent(GLWin.dpy, None, NULL))
        {
            printf("Could not release drawing context.\n");
        }
      glXDestroyContext(GLWin.dpy, GLWin.ctx);
      GLWin.ctx = NULL;
    }
  /* switch back to original desktop resolution if we were in fs*/
  if (GLWin.fs)
    {
      XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, &GLWin.deskMode);
      XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
    }
  XCloseDisplay(GLWin.dpy);
#endif
}


short OpenGLGraphics::Create3DWindow()
{
  return Create3DWindow("test",0,0,640,480,16,0);
}

#ifdef WIN32
/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
short OpenGLGraphics::Create3DWindow(char* title, int left, int top, 
				     int width, int height, int bits, bool fullscreenflag)
{
  GLuint		PixelFormat;			// Holds The Results After Searching For A Match
  static WNDCLASS	wc;						// Windows Class Structure
  DWORD		dwExStyle;				// Window Extended Style
  DWORD		dwStyle;				// Window Style
  RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
  WindowRect.left=(long)left;			// Set Left Value To 0
  WindowRect.right=(long)width;		// Set Right Value To Requested Width
  WindowRect.top=(long)top;				// Set Top Value To 0
  WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height
  
  m_fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag
  
  m_hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
  if ( !wc.lpszClassName )
    {
      wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
      wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
      wc.cbClsExtra		= 0;									// No Extra Window Data
      wc.cbWndExtra		= 0;									// No Extra Window Data
      wc.hInstance		= m_hInstance;							// Set The Instance
      wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
      wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
      wc.hbrBackground	= NULL;									// No Background Required For GL
      wc.lpszMenuName		= NULL;									// We Don't Want A Menu
      wc.lpszClassName	= "OpenGL";								// Set The Class Name
      
      if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
	  MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
	  return FALSE;											// Return FALSE
	}
    }
  if (m_fullscreen)												// Attempt Fullscreen Mode?
    {
      DEVMODE dmScreenSettings;								// Device Mode
      memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
      dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
      dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
      dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
      dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
      dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
      
      // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
      if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
	{
	  // If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
	  if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
	    {
	      m_fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
	    }
	  else
	    {
				// Pop Up A Message Box Letting User Know The Program Is Closing.
	      MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
	      return FALSE;									// Return FALSE
	    }
	}
    }
	
  if (m_fullscreen)												// Are We Still In Fullscreen Mode?
    {
      dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
	    dwStyle=WS_POPUP;										// Windows Style
	    ShowCursor(FALSE);										// Hide Mouse Pointer
    }
  else
    {
      dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
      dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
    }

  AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size
  
  // Create The Window
  if (!(m_hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
				"OpenGL",							// Class Name
				title,								// Window Title
				dwStyle |							// Defined Window Style
				WS_CLIPSIBLINGS |					// Required Window Style
				WS_CLIPCHILDREN,					// Required Window Style
				0, 0,								// Window Position
				WindowRect.right-WindowRect.left,	// Calculate Window Width
				WindowRect.bottom-WindowRect.top,	// Calculate Window Height
				NULL,								// No Parent Window
				NULL,								// No Menu
				m_hInstance,							// Instance
				NULL)))								// Dont Pass Anything To WM_CREATE
    {
      Kill3DWindow();								// Reset The Display
      MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;								// Return FALSE
    }
  
  static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
  {
    sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
    1,											// Version Number
    PFD_DRAW_TO_WINDOW |						// Format Must Support Window
    PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
    PFD_DOUBLEBUFFER,							// Must Support Double Buffering
    PFD_TYPE_RGBA,								// Request An RGBA Format
    bits,										// Select Our Color Depth
    0, 0, 0, 0, 0, 0,							// Color Bits Ignored
    0,											// No Alpha Buffer
    0,											// Shift Bit Ignored
    0,											// No Accumulation Buffer
    0, 0, 0, 0,									// Accumulation Bits Ignored
    16,											// 16Bit Z-Buffer (Depth Buffer)  
    0,											// No Stencil Buffer
    0,											// No Auxiliary Buffer
    PFD_MAIN_PLANE,								// Main Drawing Layer
    0,											// Reserved
    0, 0, 0										// Layer Masks Ignored
  };
  
  if (!(m_hDC=GetDC(m_hWnd)))							// Did We Get A Device Context?
    {
      Kill3DWindow();								// Reset The Display
      MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;								// Return FALSE
    }
  
  if (!(PixelFormat=ChoosePixelFormat(m_hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
    {
      Kill3DWindow();								// Reset The Display
      MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;								// Return FALSE
    }
  
  if(!SetPixelFormat(m_hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
    {
      Kill3DWindow();								// Reset The Display
      MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;								// Return FALSE
    }
  
  if (!(m_hRC=wglCreateContext(m_hDC)))				// Are We Able To Get A Rendering Context?
    {
      Kill3DWindow();								// Reset The Display
      MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;								// Return FALSE
    }
  
  if(!wglMakeCurrent(m_hDC,m_hRC))					// Try To Activate The Rendering Context
    {
      Kill3DWindow();								// Reset The Display
      MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;								// Return FALSE
    }
  
  ShowWindow(m_hWnd,SW_SHOW);						// Show The Window
  SetForegroundWindow(m_hWnd);						// Slightly Higher Priority
  SetFocus(m_hWnd);									// Sets Keyboard Focus To The Window
  Resize3DWindow(width, height);					// Set Up Our Perspective GL Screen
  
  if (!InitGL())									// Initialize Our Newly Created GL Window
    {
      Kill3DWindow();								// Reset The Display
      MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      return FALSE;								// Return FALSE
    }
  
  return TRUE;									// Success
}
#endif

#ifdef LINUX
short OpenGLGraphics::Create3DWindow(char* title, int left, int top, 
				     int width, int height, int bits, bool fullscreenflag)
{
    XVisualInfo *vi;
    int i;
    int glxMajorVersion, glxMinorVersion;
    int vidModeMajorVersion, vidModeMinorVersion;
    XF86VidModeModeInfo **modes;
    int modeNum;
    int bestMode;
    Atom wmDelete;
    Window winDummy;
    unsigned int borderDummy;
    Colormap cmap;
    int dpyWidth, dpyHeight;

    GLWin.fs = fullscreenflag;
    /* set best mode to current */
    bestMode = 0;
    /* get a connection */
    GLWin.dpy = XOpenDisplay(0);
    GLWin.screen = DefaultScreen(GLWin.dpy);
    XF86VidModeQueryVersion(GLWin.dpy, &vidModeMajorVersion,
        &vidModeMinorVersion);
    printf("XF86VidModeExtension-Version %d.%d\n", vidModeMajorVersion,
        vidModeMinorVersion);
    XF86VidModeGetAllModeLines(GLWin.dpy, GLWin.screen, &modeNum, &modes);
    /* save desktop-resolution before switching modes */
    GLWin.deskMode = *modes[0];
    /* look for mode with requested resolution */
    for (i = 0; i < modeNum; i++)
    {
        if ((modes[i]->hdisplay == width) && (modes[i]->vdisplay == height))
        {
            bestMode = i;
        }
    }
    /* get an appropriate visual */
    vi = glXChooseVisual(GLWin.dpy, GLWin.screen, attrListDbl);
    if (vi == NULL)
    {
        vi = glXChooseVisual(GLWin.dpy, GLWin.screen, attrListSgl);
        GLWin.doubleBuffered = False;
        printf("Only Singlebuffered Visual!\n");
    }
    else
    {
        GLWin.doubleBuffered = True;
        printf("Got Doublebuffered Visual!\n");
    }
    glXQueryVersion(GLWin.dpy, &glxMajorVersion, &glxMinorVersion);
    printf("glX-Version %d.%d\n", glxMajorVersion, glxMinorVersion);
    /* create a GLX context */
    GLWin.ctx = glXCreateContext(GLWin.dpy, vi, 0, GL_TRUE);

    //    InitGL();
    /* create a color map */
    cmap = XCreateColormap(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
        vi->visual, AllocNone);
    GLWin.attr.colormap = cmap;
    GLWin.attr.border_pixel = 0;

    if (GLWin.fs)
      {
        XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, modes[bestMode]);
        XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
        dpyWidth = modes[bestMode]->hdisplay;
        dpyHeight = modes[bestMode]->vdisplay;
        printf("Resolution %dx%d\n", dpyWidth, dpyHeight);
        XFree(modes);
	
	/* create a fullscreen window */
	GLWin.attr.override_redirect = True;
	GLWin.attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
	  StructureNotifyMask;
        GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
				  0, 0, dpyWidth, dpyHeight, 0, vi->depth, InputOutput, vi->visual,
				  CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
				  &GLWin.attr);
        XWarpPointer(GLWin.dpy, None, GLWin.win, 0, 0, 0, 0, 0, 0);
	XMapRaised(GLWin.dpy, GLWin.win);
        XGrabKeyboard(GLWin.dpy, GLWin.win, True, GrabModeAsync,
		      GrabModeAsync, CurrentTime);
        XGrabPointer(GLWin.dpy, GLWin.win, True, ButtonPressMask,
		     GrabModeAsync, GrabModeAsync, GLWin.win, None, CurrentTime);
      }
    else
      {
        /* create a window in window mode*/
        GLWin.attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask |
	  StructureNotifyMask;
        GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
				  0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
				  CWBorderPixel | CWColormap | CWEventMask, &GLWin.attr);
        /* only set window title and handle wm_delete_events if in windowed mode */
        wmDelete = XInternAtom(GLWin.dpy, "WM_DELETE_WINDOW", True);
        XSetWMProtocols(GLWin.dpy, GLWin.win, &wmDelete, 1);
        XSetStandardProperties(GLWin.dpy, GLWin.win, title,
			       title, None, NULL, 0, NULL);
        XMapRaised(GLWin.dpy, GLWin.win);
      }
    /* connect the glx-context to the window */
    glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
    XGetGeometry(GLWin.dpy, GLWin.win, &winDummy, &GLWin.x, &GLWin.y,
		 &GLWin.width, &GLWin.height, &borderDummy, &GLWin.depth);
    printf("Depth %d\n", GLWin.depth);
    if (glXIsDirect(GLWin.dpy, GLWin.ctx))
      printf("Congrats, you have Direct Rendering!\n");
    else
      printf("Sorry, no Direct Rendering possible!\n");
    InitGL();
    return 0;
}
#endif



BOOL OpenGLGraphics::InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
#ifdef LINUX
  glShadeModel(GL_SMOOTH);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  /* we use resizeGLScene once to set up our initial perspective */
  Resize3DWindow(GLWin.width, GLWin.height);
  glFlush();
#endif
  return TRUE;										// Initialization Went OK
}


GLvoid OpenGLGraphics::Resize3DWindow(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

void OpenGLGraphics::Show3DWindow()
{
  //::ShowWindow(m_hWnd, SW_SHOW);
}

void OpenGLGraphics::Hide3DWindow()
{
  //::ShowWindow(m_hWnd, SW_HIDE);
}


// standard framework routines

void  OpenGLGraphics::SetSetup(void *setup)
{
  globals = (_STANDARD_GRAPHICS *)setup;
}

short OpenGLGraphics::GetDependencyCount()
{
  return 0;
}

void  OpenGLGraphics::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));

  switch(Index)
  {
    case 0:
      Dependency->ID              = _PLATFORM_DEPENDENCY;
      Dependency->Description     = "It requires a windows handle.\0";
      Dependency->Required        = TRUE;
      break;
  }

}

short OpenGLGraphics::SetDependencyData(short ID, void* Data)
{
  switch(ID)
  {
    case _PLATFORM_DEPENDENCY:
      //platform = (StandardPlatform *)Data;
      break;
    default:
      return _NO_DEPENDENCY_TO_SET;
      break;
  }
  return 0;
}

void  OpenGLGraphics::GetSetup(void*setup)
{
  setup = globals;
}

short OpenGLGraphics::CheckStatus()
{
  if(globals == 0)
  {
    return _SETUP_NOT_SET;
  }
  //if(platform == 0)
  //{
  //  return _DEPENDENCY_NOT_SET;
  //}
  return 0;
}

///////////////////////////////////////////
// whole buffer routines

void OpenGLGraphics::Flip()
{
#ifdef WIN32
  SwapBuffers(m_hDC);
#endif
#ifdef LINUX
  if (GLWin.doubleBuffered)
    {
      glXSwapBuffers(GLWin.dpy, GLWin.win);
    }
#endif
}

BOOL OpenGLGraphics::CanFlip()
{
  return TRUE;
}


void OpenGLGraphics::ClearSurface()
{
  glClear(GL_COLOR_BUFFER_BIT | (m_depthFlag ? GL_DEPTH_BUFFER_BIT : 0));
}

void OpenGLGraphics::SetClearColor(float r, float g, float b, float a)
{
  glClearColor(r,g,b,a);
}


// this would be a good place to process the event loop
//BOOL  OpenGLGraphics::BeginScene()
void  OpenGLGraphics::BeginScene()
{
  //processMessage();
  // check to see if this is the current context, if not make it.
  /*
  if (wglGetCurrentContext() != m_hRC)
    {
      if(!wglMakeCurrent(m_hDC,m_hRC))		// Try To Activate The Rendering Context
	{
	  Kill3DWindow();			// Reset The Display
	  MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
	  return;    //FALSE;			// Return FALSE
	}
	}*/
  //	return TRUE;
}

void  OpenGLGraphics::EndScene()
//BOOL OpenGLGraphics::EndScene()
{
	//return TRUE;
}

void OpenGLGraphics::DrawPoint(float x, float y, float z)
{
  glBegin(GL_POINTS);
  glVertex3f(x,y,z);
  glEnd();
}

void OpenGLGraphics::DrawPointArray(int vertexCount, _GraphicsVector * v)
{
  glBegin(GL_POINTS);
  for (int i=0;i<vertexCount;i++)
    glVertex3f(v[i].x, v[i].y, v[i].z);
  glEnd();
}

void OpenGLGraphics::DrawLine(float x1, float y1, float z1,
			      float x2, float y2, float z2)
{
  glBegin(GL_POINTS);
  glVertex3f(x1,y1,z1);
  glVertex3f(x2,y2,z2);
  glEnd();
}


void OpenGLGraphics::DrawTriangle(float x1, float y1, float z1,
				  float x2, float y2, float z2, 
				  float x3, float y3, float z3)
{
  glBegin(GL_TRIANGLES);
  glVertex3f(x1, y1, z1);
  glVertex3f(x2, y2, z2);
  glVertex3f(x3, y3, z3);
  glEnd();
}

void OpenGLGraphics::DrawTriangle(_GraphicsVector * v1, _GraphicsVector * v2,
				  _GraphicsVector * v3)
{
  glBegin(GL_TRIANGLES);
  glVertex3f(v1->x, v1->y, v1->z);
  glVertex3f(v2->x, v2->y, v2->z);
  glVertex3f(v3->x, v3->y, v3->z);
  glEnd();
}



void OpenGLGraphics::DrawRect(float x1, float y1, float x2, float y2)
{
  glRectf(x1,y1,x2,y2);
}

void OpenGLGraphics::DrawQuad()
{

}

void OpenGLGraphics::DrawTriangleArray(int vertexCount, _GraphicsVector * v)
{
  glBegin(GL_TRIANGLES);
  for (int i=0;i<vertexCount;i++)
    {
      glVertex3f(v[i].x, v[i].y, v[i].z);
    }
  glEnd();
}

void OpenGLGraphics::DrawTriangleFan(int vertexCount, _GraphicsVector * v)
{
  glBegin(GL_TRIANGLE_FAN);
  for (int i=0;i<vertexCount;i++)
    {
      glVertex3f(v[i].x, v[i].y, v[i].z);
    }
  glEnd();
}


void OpenGLGraphics::DrawTriangleStrip(int vertexCount, _GraphicsVector * v)
{
  glBegin(GL_TRIANGLE_STRIP);
  for (int i=0;i<vertexCount;i++)
    {
      glVertex3f(v[i].x, v[i].y, v[i].z);
    }
  glEnd();
}

void OpenGLGraphics::DrawTriangleArray(int vertexCount, _ShadedVector *v)
{
  glBegin(GL_TRIANGLES);
  for (int i=0;i<vertexCount;i++)
    {
      glVertex3f(v[i].nx, v[i].ny, v[i].nz);
      glVertex3f(v[i].x, v[i].y, v[i].z);
    }
  glEnd();
}

void OpenGLGraphics::DrawTriangleFan(int vertexCount, _ShadedVector *v)
{
  glBegin(GL_TRIANGLE_FAN);
  for (int i=0;i<vertexCount;i++)
    {
      glVertex3f(v[i].nx, v[i].ny, v[i].nz);
      glVertex3f(v[i].x, v[i].y, v[i].z);
    }
  glEnd();
}

void OpenGLGraphics::DrawTriangleStrip(int vertexCount, _ShadedVector *v)
{
  glBegin(GL_TRIANGLE_STRIP);
  for (int i=0;i<vertexCount;i++)
    {
      glVertex3f(v[i].nx, v[i].ny, v[i].nz);
      glVertex3f(v[i].x, v[i].y, v[i].z);
    }
  glEnd();
}


void OpenGLGraphics::DrawString(const char * str)
{

}


void OpenGLGraphics::SetColor(float r, float g, float b)
{
  glColor3f(r,g,b);
}

void OpenGLGraphics::SetColor(float r, float g, float b, float a)
{
  glColor4f(r,g,b,a);
}	

void OpenGLGraphics::SetColor(_Color * color)
{
  glColor3fv((GLfloat*)color);
}


void OpenGLGraphics::SetFlatShading()
{
  glShadeModel(GL_FLAT);
}

void OpenGLGraphics::SetSmoothShading()
{
  glShadeModel(GL_SMOOTH);
}

void OpenGLGraphics::EnableLighting()
{
  glEnable(GL_LIGHTING);
}

void OpenGLGraphics::DisableLighting()
{
  glDisable(GL_LIGHTING);
}

void OpenGLGraphics::SetAmbientLight(_Color * color)
{	
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (GLfloat*)color);
}

void OpenGLGraphics::EnableDepthTest()
{
  glEnable(GL_DEPTH_TEST);
  m_depthFlag = TRUE;
}

void OpenGLGraphics::DisableDepthTest()
{
  glDisable(GL_DEPTH_TEST);
  m_depthFlag = FALSE;
}


// matrices
void OpenGLGraphics::SetIdentity()
{
  glLoadIdentity();
}

void OpenGLGraphics::SetModelViewMatrix()
{
  glMatrixMode(GL_MODELVIEW);		
}

void OpenGLGraphics::SetProjectionMatrix()
{
  glMatrixMode(GL_PROJECTION);
}

void OpenGLGraphics::SetTextureMatrix()
{	
  glMatrixMode(GL_TEXTURE);
}


void OpenGLGraphics::Translate(float x, float y, float z)
{
  glTranslatef(x,y,z);
}

void OpenGLGraphics::Rotate(float angle, float x, float y, float z)
{
  glRotatef(angle, x,y,z);
}

void OpenGLGraphics::Scale(float x, float y, float z)
{
  glScalef(x,y,z);
}

void OpenGLGraphics::LoadMatrix()
{
	
}

void OpenGLGraphics::MultiMatrix()
{

}

void OpenGLGraphics::PushMatrix()
{
  glPushMatrix();
}

void OpenGLGraphics::PopMatrix()
{
  glPopMatrix();
}

// Display lists
void OpenGLGraphics::StartList(int index)
{
  glNewList(index, GL_COMPILE);
}

void OpenGLGraphics::EndList()
{
  glEndList();
}

void OpenGLGraphics::DisplayList(int index)
{
  glCallList(index);
}

// Materials 
/*
_Material* OpenGLGraphics::CreateMaterial(_Color *Diffuse, _Color *Ambient, _Color *Emissive)
{
  _Material * mat = new _Material();
  mat->ambient = Ambient;
  mat->diffuse = Diffuse;
  mat->emission = Emissive;

  return mat;
}*/

_Material* OpenGLGraphics::CreateMaterial(_Color *Diffuse, _Color *Ambient, _Color *Emissive,
						_Color *Specular, int shininess)
{
  _Material * mat = new _Material();
  mat->ambient = Ambient;
  mat->diffuse = Diffuse;
  mat->emission = Emissive;
  mat->shininess = shininess;
  mat->specular = Specular;
  return mat;
}


void OpenGLGraphics::SetMaterial(_Material*Mat)
{
  if (Mat->ambient) 
    glMaterialfv(GL_FRONT, GL_AMBIENT, (GLfloat*)Mat->ambient);
  if (Mat->diffuse)
    glMaterialfv(GL_FRONT, GL_DIFFUSE, (GLfloat*)Mat->diffuse);
  if (Mat->emission)
    glMaterialfv(GL_FRONT, GL_EMISSION, (GLfloat*)Mat->emission);
  if (Mat->specular)
    glMaterialfv(GL_FRONT, GL_SPECULAR, (GLfloat*)Mat->specular);
  if (Mat->shininess)
    glMaterialfv(GL_FRONT, GL_SHININESS, (GLfloat*)Mat->shininess);

}

void OpenGLGraphics::DeleteMaterial(_Material *mat)
{
  delete (mat);
}

///////////// textures
_Texture* OpenGLGraphics::CreateTexture(char *filename)
{
  //BITMAPINFO * info;
  //GLubyte * bits;
  //	bits = LoadDIBitmap(filename, &info);
  return NULL;
}

void OpenGLGraphics::SetTexture(short stage, _Texture* Texture)
{

}

void OpenGLGraphics::DeleteTexture(_Texture *tex)
{

}






_VertexBuffer* OpenGLGraphics::CreateVertexBuffer(void *buffer, unsigned long bufferbytecount, unsigned long vertexbytecount)
{
  return NULL;
}

_IndexBuffer* OpenGLGraphics::CreateIndexBuffer(void *buffer, unsigned long bufferbytecount, unsigned long indexbytecount)
{
  return NULL;
}



_Light* OpenGLGraphics::CreateDirectionalLight(_Vector* Direction, _Color* Color)
{
  return NULL;
}

_Texture* OpenGLGraphics::CreateTexture(short Width, short Height, short MipMap)
{
  return NULL;
}


void    OpenGLGraphics::DeleteVertexBuffer(_VertexBuffer *buff)
{

}

void    OpenGLGraphics::DeleteIndexBuffer(_IndexBuffer *buff)
{

}


void    OpenGLGraphics::DeleteLight(_Light *light)
{

}

void OpenGLGraphics::SetTransformWorld(_Matrix *matrix)
{

}

void OpenGLGraphics::SetTransformCamera(_Matrix *matrix)
{

}

void OpenGLGraphics::SetTransformFOV(_Matrix *matrix)
{

}

void OpenGLGraphics::SetVertexBuffer(_VertexBuffer *VertexBuffer, short vertexbytecount)
{

}

void OpenGLGraphics::SetIndexBuffer(void* Buffer)
{

}

void OpenGLGraphics::DrawTriangleListBuffer(short vertexcount, short indexcount)
{

}

void OpenGLGraphics::SetLight(short lightnum, _Light *Light)
{

}

void OpenGLGraphics::LightEnabled(short lightnum, short lighton)
{

}

void OpenGLGraphics::DrawTextF(char *buffer, short bufferbytecount, _Rect rect, short LeftCenterRight, _Color *color )
{

}

_Rect OpenGLGraphics::GetCoordsRect()
{
  return _Rect(0,0,1,1);
}

void OpenGLGraphics::SetForeground(short on)
{

}


_Material * OpenGLGraphics::CreateMaterial( _Color *Ambient, _Color *Diffuse, _Color *Emissive )
{
  OGLMaterial *material = new OGLMaterial;
  memset( material, 0, sizeof( OGLMaterial ) );
  material->ambient = (GLfloat *)Ambient;
  material->diffuse = (GLfloat *)Diffuse;
  material->emissive = (GLfloat *)Emissive;
  float *specular = new float[4];
  specular[0] = specular[1] = specular[2] = 0.0f;
  specular[3] = 1.0f;
  material->specular = specular;
  material->shiny = 0.0f;

  return((_Material*) material );
}


#endif
