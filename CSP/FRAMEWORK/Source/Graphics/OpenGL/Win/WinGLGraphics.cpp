///////////////////////////////////////////////////////////////////////////
//
//   WinGLPolygonMesh  - CSP - http://csp.homeip.net
//
//   coded by Wolverine and GKW
//
///////////////////////////////////////////////////////////////////////////
//   
//  This class implements the windows specific portions required for
//  use with OpenGL graphics. This includes Initialization.
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//
///////////////////////////////////////////////////////////////////////////
#ifdef OGL

#include <GLGraphics.h>
#include <WinGLGraphics.h>
#include <GLGlobals.h>
#include <GLFont.h>
#include <GLTexture.h>

/** 
 * \fn WinGLGraphics::WinGLGraphics()
 * \brief Constructor for WinGLGraphics, Initializes member variable to NULL.
 */
WinGLGraphics::WinGLGraphics()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "WinGLGraphics::WinGLGraphics()" );
      
	m_hDC=NULL;		    // Private GDI Device Context
	m_hRC=NULL;		    // Permanent Rendering Context
	m_hWnd=NULL;		// Holds Our Window Handle
	m_hInstance=NULL;	// Holds The Instance Of The Application

}

/** 
 * \fn WinGLGraphics::~WinGLGraphics()
 * \brief Destructor for WinGLGraphics, Calls Uninitialize to cleanup the object.
 */
WinGLGraphics::~WinGLGraphics()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "WinGLGraphics::~WinGLGraphics()" );
    Uninitialize();
}

/** 
 * \fn void WinGLGraphics::Uninitialize()
 * Cleans up the WinGLGraphics object. 
 *
 * The OpenGL Context is deleted then
 * the DC associated with the window is released.
 */
void WinGLGraphics::Uninitialize()								
{

    CSP_LOG(CSP_OPENGL, CSP_TRACE, "void WinGLGraphics::Uninitialize()" );
	
    // Do We Have A Rendering Context?
    if (m_hRC)											
	{
		// Are We Able To Release The DC And RC Contexts?
        if (!wglMakeCurrent(m_hDC,NULL))					
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		// Are We Able To Delete The RC?
        if (!wglDeleteContext(m_hRC))						
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		ReleaseDC(m_hWnd, m_hDC);
		
        // Set RC To NULL
        m_hRC=NULL;										
	}

}

/** 
 * \fn short WinGLGraphics::Initialize()
 * \brief Initializes the Window for OpenGL 3D graphics. 
 * \param App The main application
 * \param Init The graphics init structure.
 * 
 * This routine initializes the window for OpenGL 3D Graphics. 
 * Options used are
 * in the _GRAPHICS_INIT_STRUCT parameter. This routine creates a 
 * PIXELFORMATDESCRIPTOR which takes the input parameters. This
 * used for the call to ChoosePixelFormat which picks the cards
 * rendering mode that is closest to the parameters in the Descriptor.
 * The pixel mode is then set for the window. Next the rendering
 * context for the window is then created and set current. Finally
 * defaults are set for OpenGL parameters like background color etc.
 */
short WinGLGraphics::Initialize(StandardApp *App, _GRAPHICS_INIT_STRUCT * Init)
{
	
    // Make a copy of the Init structure.
    memcpy(&p_Init, Init, sizeof(p_Init));

    // Save the Win handle
	m_hWnd = (HWND)p_Init.Handle;

	// Holds The Results After Searching For A Match
    GLuint		PixelFormat;

	// pfd Tells Windows How We Want Things To Be
    static	PIXELFORMATDESCRIPTOR pfd=				
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		(unsigned char)Init->Depth, 				// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		Init->ZBuffDepth,							// 24Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	// Did We Get A Device Context?
    if (!(m_hDC=GetDC(m_hWnd)))							
	{
		Uninitialize();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	dumpAllPixelFormats(m_hDC);

	if (!(PixelFormat=ChoosePixelFormat(m_hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		Uninitialize();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(m_hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		Uninitialize();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(m_hRC=wglCreateContext(m_hDC)))				// Are We Able To Get A Rendering Context?
	{
		Uninitialize();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(m_hDC,m_hRC))					// Try To Activate The Rendering Context
	{
		Uninitialize();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	int pixIndex = GetPixelFormat(m_hDC);
	DescribePixelFormat(m_hDC, pixIndex, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	dumpPixelFormat(pfd);

    // Set initial values for the OpenGL states.
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
    glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
	StandardColor c(200, 300, 300 );
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	m_depthFlag = 1;

    // Load a font for text display.
    m_text = new GLFont(this);
	m_text->Create( "Font.tga" ,2);

	if (Init->Wireframe)
		SetRenderState(FW_WIREFRAME, true);
	else
		SetRenderState(FW_WIREFRAME, false);


	SetRenderState(FW_ZBUFFER, TRUE);

	return _A_OK;
}


/**
 * \fn void WinGLGraphics::Resize(short width, short height)
 * \brief Called during a resize to adjust the graphics parameters.
 * \param width The new width of the window.
 * \param height The new heigth of the window.
 */
void WinGLGraphics::Resize(short width, short height)	
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "void WinGLGraphics::Resize(short width, short height)" );
	// Prevent A Divide By Zero By
    if (height==0)										
	{
		height=1;										
	}

	// Reset The Current Viewport
    glViewport(0,0,width,height);						

	// Select The Projection Matrix
    glMatrixMode(GL_PROJECTION);						
	
    // Reset The Projection Matrix
    glLoadIdentity();									

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(30.0f,(GLfloat)width/(GLfloat)height,p_Init.NearZ,p_Init.FarZ);

	// Select The Modelview Matrix
    glMatrixMode(GL_MODELVIEW);							
	
    // Reset The Modelview Matrix
    glLoadIdentity();									
}



///////////////////////////////////////////
// whole buffer routines
/** 
 * \fn void WinGLGraphics::Flip()
 * \brief Flips the BackBuffer with the Display screen.
 */
void WinGLGraphics::Flip()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "void WinGLGraphics::Flip()" );
	SwapBuffers(m_hDC);
}


// this would be a good place to process the event loop
//BOOL  OpenGLGraphics::BeginScene()

/**
 * \fn void  WinGLGraphics::BeginScene()
 * \brief Called at the beginning of each frame
 * 
 * Called at the beginning of each frame. It verifies the rendering
 * context is set. The Texture is then cleared. Finally the Camera is 
 * applied.
 */
void  WinGLGraphics::BeginScene()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "void  WinGLGraphics::BeginScene()" );
	// check to see if this is the current context, if not make it.
	if (wglGetCurrentContext() != m_hRC)
	{
		if(!wglMakeCurrent(m_hDC,m_hRC))					// Try To Activate The Rendering Context
		{
			//MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return;    //FALSE;								// Return FALSE
		}
	}

	glShadeModel(GL_FLAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_COLOR_MATERIAL);						// Allow color
    
	if (p_Camera)
		p_Camera->Apply();

}

/**
 * \fn void  WinGLGraphics::EndScene
 * \brief Called after the scene is rendered.
 *
 * This is called after scene is rendered. It currently does not
 * do anything but is required for compatiblity with DirectX.
 */
void  WinGLGraphics::EndScene()
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "void  WinGLGraphics::EndScene()" );
}

/** 
 * \fn void WinGLGraphics::dumpAllPixelFormats(HDC hDC)
 * \brief Dump all pixel formats associated with this card/hDC.
 * \hDC The Windows Device Context associated with the window.
 *
 *  Examines all available pixel formats and dumps them out
 *  cout. Useful to examine the current hardware and verify
 *  that acceleration will be used.
 */
void WinGLGraphics::dumpAllPixelFormats(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pfd;
	int nFormatCount;

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	nFormatCount = DescribePixelFormat(hDC, 1, 0, NULL);

	CSP_LOG(CSP_OPENGL, CSP_DEBUG, "Dumping pixel formats");
	CSP_LOG(CSP_OPENGL, CSP_DEBUG, nFormatCount << " pixel formats available");
	for (int i=0; i<nFormatCount; i++)
	{
		DescribePixelFormat(hDC, i+1, pfd.nSize, &pfd);
		CSP_LOG(CSP_OPENGL, CSP_DEBUG, "Dumbing format " << i+1);
		dumpPixelFormat(pfd);
		CSP_LOG(CSP_OPENGL, CSP_DEBUG, endl);

	}

}

/**
 * \fn void WinGLGraphics::dumpPixelFormat(PIXELFORMATDESCRIPTOR pfd)
 * \brief Dump a Pixel Format
 * \param pfd The pixel format to dump.
 */
void WinGLGraphics::dumpPixelFormat(PIXELFORMATDESCRIPTOR pfd)
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "WinGLGraphics()");

  CSP_LOG(CSP_OPENGL, CSP_DEBUG, "dwFlags = 0x" << hex << pfd.dwFlags);

  CSP_LOG(CSP_OPENGL, CSP_DEBUG, "PFD_DRAW_TO_WINDOW is " << 
	((pfd.dwFlags & PFD_DRAW_TO_WINDOW) ? "VALID" : "INVALID"));

  CSP_LOG(CSP_OPENGL, CSP_DEBUG, "PFD_DRAW_TO_BITMAP is " <<
	((pfd.dwFlags & PFD_DRAW_TO_BITMAP) ? "VALID" : "INVALID"));

  CSP_LOG(CSP_OPENGL, CSP_DEBUG, "PFD_SUPPORT_GDI is " <<
	((pfd.dwFlags & PFD_SUPPORT_GDI) ? "VALID" : "INVALID"));

  CSP_LOG(CSP_OPENGL, CSP_DEBUG, "PFD_GENERIC_ACCELERATED is " <<
	((pfd.dwFlags & PFD_GENERIC_ACCELERATED) ? "VALID" : "INVALID"));

  CSP_LOG(CSP_OPENGL, CSP_DEBUG, "PFD_GENERIC_FORMAT is " <<
	((pfd.dwFlags & PFD_NEED_PALETTE) ? "VALID" : "INVALID"));

  CSP_LOG(CSP_OPENGL, CSP_DEBUG, "PFD_NEED_SYSTEM_PALETTE is " <<
	((pfd.dwFlags & PFD_NEED_SYSTEM_PALETTE) ? "VALID" : "INVALID"));

  CSP_LOG(CSP_OPENGL, CSP_DEBUG, "PFD_DOUBLEBUFFER is " <<
	((pfd.dwFlags & PFD_DOUBLEBUFFER) ? "VALID" : "INVALID"));

  CSP_LOG(CSP_OPENGL, CSP_DEBUG, "PFD_SWAP_LAYER_BUFFERS is " <<
	((pfd.dwFlags & PFD_SWAP_LAYER_BUFFERS) ? "VALID" : "INVALID"));
	
}



#endif
