// CSP project

#ifdef OGL

// openglgraphics.cpp : Defines the entry point for the DLL application.
//

#include "bitmap.h"


#include <windows.h>		// Header File For Windows
#include <iostream.h>
#include "graphics/standardgraphics.h"
#include "GLPolygonListObject.h"
#include "GLMaterialObject.h"
#include "GLCamera.h"
#include "GLTexture2D.h"
#include "openglgraphics.h"
#include "graphics/gl_globals.h"

#include <math.h>			// Math Library Header File
#include <stdio.h>			// Header File For Standard Input/Output
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#include "Texture.h"
#include "Font.h"

extern GLStateMachine	statesGL;

OpenGLGraphics::OpenGLGraphics()
{
	m_fullscreen=false;
	m_hDC=NULL;		// Private GDI Device Context
	m_hRC=NULL;		// Permanent Rendering Context
	m_hWnd=NULL;		// Holds Our Window Handle
	m_hInstance=NULL;		// Holds The Instance Of The Application

	m_depthFlag = FALSE;
	m_text = NULL;
}

OpenGLGraphics::~OpenGLGraphics()
{
	Kill3DWindow();
	delete( m_text );
}


GLvoid OpenGLGraphics::Kill3DWindow(GLvoid)								// Properly Kill The Window
{
	if (m_fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (m_hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(m_hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		m_hRC=NULL;										// Set RC To NULL
	}

	if (m_hDC && !ReleaseDC(m_hWnd,m_hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		m_hDC=NULL;										// Set DC To NULL
	}

	if (m_hWnd && !DestroyWindow(m_hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		m_hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",m_hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		m_hInstance=NULL;									// Set hInstance To NULL
	}
}

/** dumpPixelFormats
 *  Examines all available pixel formats and dumps them out
 *  cout. Useful to examine the current hardware and verify
 *  that acceleration will be used.
 */
void OpenGLGraphics::dumpAllPixelFormats(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pfd;
	int nFormatCount;

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	nFormatCount = DescribePixelFormat(hDC, 1, 0, NULL);
	cout << "Dumping pixel formats" << endl;
	cout << nFormatCount << " pixel formats available" << endl;
	for (int i=0; i<nFormatCount; i++)
	{
		DescribePixelFormat(hDC, i+1, pfd.nSize, &pfd);
		cout << "Dumping format " << i+1 << endl;
		dumpPixelFormat(pfd);
		cout << endl;

	}

}

void OpenGLGraphics::dumpPixelFormat(PIXELFORMATDESCRIPTOR pfd)
{
	cout << "dwFlags = " << pfd.dwFlags << endl;
	cout << "PFD_DRAW_TO_WINDOW is " << 
		((pfd.dwFlags & PFD_DRAW_TO_WINDOW) ? "valid" : "invalid") << endl;
	cout << "PFD_DRAW_TO_BITMAP is " << 
		((pfd.dwFlags & PFD_DRAW_TO_BITMAP) ? "valid" : "invalid") << endl;
	cout << "PFD_SUPPORT_GDI is " << 
		((pfd.dwFlags & PFD_SUPPORT_GDI) ? "valid" : "invalid") << endl;
	cout << "PFD_GENERIC_ACCELERATED" <<
		((pfd.dwFlags & PFD_GENERIC_ACCELERATED) ? "valid" : "invalid") << endl;
	cout << "PFD_GENERIC_FORMAT" <<
		((pfd.dwFlags & PFD_NEED_PALETTE) ? "valid" : "invalid") << endl;
	cout << "PFD_NEED_SYSTEM_PALETTE" << 
		((pfd.dwFlags & PFD_NEED_SYSTEM_PALETTE) ? "valid" : "invalid") << endl;
	cout << "PFD_DOUBLEBUFFER" <<
		((pfd.dwFlags & PFD_DOUBLEBUFFER) ? "valid" : "invalid") << endl;
	cout << "PFD_SWAP_LAYER_BUFFERS" <<
		((pfd.dwFlags & PFD_SWAP_LAYER_BUFFERS) ? "valid" : "invalid") << endl;
	

}

short OpenGLGraphics::Create3DWindow()
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match


	m_hWnd = (HWND)globals->window;

	//dumpAllPixelFormats(GetDC(m_hWnd));

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		(unsigned char)globals->depth,				// Select Our Color Depth
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

	dumpPixelFormat(pfd);

//	ShowWindow(m_hWnd,SW_SHOW);						// Show The Window
//	SetForegroundWindow(m_hWnd);						// Slightly Higher Priority
//	SetFocus(m_hWnd);									// Sets Keyboard Focus To The Window
//	Resize3DWindow(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		Kill3DWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	
	return _A_OK;
}


BOOL OpenGLGraphics::InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	m_text = new Font( "Font.tga" ,2);
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
	_Color c(0.8f, 0.8f, 0.8f);
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
	gluPerspective(30.0f,(GLfloat)width/(GLfloat)height,0.1f,1000.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

void OpenGLGraphics::Show3DWindow()
{
	::ShowWindow(m_hWnd, SW_SHOW);
}

void OpenGLGraphics::Hide3DWindow()
{
	::ShowWindow(m_hWnd, SW_HIDE);
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
	SwapBuffers(m_hDC);
}

bool OpenGLGraphics::CanFlip()
{
	return TRUE;
}

void OpenGLGraphics::SetFrontFace(PolygonEnum direct)
{
	switch(direct)
	{
	case CLOCKWISE:
		glFrontFace(GL_CW);
		break;
	case COUNTERCLOCKWISE:
		glFrontFace(GL_CCW);
		break;
	}

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
//	processMessage();
	// check to see if this is the current context, if not make it.
	if (wglGetCurrentContext() != m_hRC)
	{
		if(!wglMakeCurrent(m_hDC,m_hRC))					// Try To Activate The Rendering Context
		{
			Kill3DWindow();								// Reset The Display
			MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return;    //FALSE;								// Return FALSE
		}
	}

//	ClearSurface();
	glMatrixMode( GL_MODELVIEW );
	glShadeModel(GL_FLAT);
	glLoadIdentity();
	//glColor4f(1.0, 0.0, 0.0, 1.0);

//	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable( GL_DEPTH_TEST );

	//	return TRUE;

//	statesGL.dumpGLStates();
}

void  OpenGLGraphics::EndScene()
//BOOL OpenGLGraphics::EndScene()
{
//	Flip();
	//return TRUE;
}

void OpenGLGraphics::Flush()
{
	glFlush();
}

void OpenGLGraphics::CullFrontFace()
{
	glCullFace(GL_FRONT);
}

void OpenGLGraphics::CullBackFace()
{
	glCullFace(GL_BACK);
}

void OpenGLGraphics::BeginPrimitive(PrimitiveTypes type)
{
	glBegin(type);
}

void OpenGLGraphics::EndPrimitive()
{
	glEnd();
}

void OpenGLGraphics::Vertex(float x, float y, float z)
{
	glVertex3f(x,y,z);
}

void OpenGLGraphics::Normal(float x, float y, float z)
{
	glNormal3f(x,y,z);
}


void OpenGLGraphics::DrawPoint(float x, float y, float z)
{
	glBegin(GL_POINTS);
		glVertex3f(x,y,z);
	glEnd();
}

void OpenGLGraphics::DrawPoint(_GraphicsVector *v)
{
	glBegin(GL_POINTS);
		glVertex3f(v->x, v->y, v->z);
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
_Material* OpenGLGraphics::CreateMaterial(_Color *Diffuse, _Color *Ambient, _Color *Emissive)
{
	_Material * mat = new _Material();
	mat->ambient = Ambient;
	mat->diffuse = Diffuse;
	mat->emission = Emissive;

	return mat;
}

_Material* OpenGLGraphics::CreateMaterial(_Color *Diffuse, _Color *Ambient, _Color *Emissive,
						_Color *Specular, float *shininess)
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



void * OpenGLGraphics::CreateTexture( char *filename )
{

	Texture *tex = new Texture( filename, Texture::DIB );
	tex->genTexObject();

	return( tex );
}

void OpenGLGraphics::SetTextureMode(int flag)
{
	if (flag)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
}

void OpenGLGraphics::SetTexture(short stage, _Texture* texture)
{
	if( texture != NULL )
	{
		glBindTexture( GL_TEXTURE_2D, ((Texture *)texture)->getTexID() );
	}
}

void OpenGLGraphics::DeleteTexture(_Texture *tex)
{
	delete( ((Texture *)tex) );
}


_Viewport * OpenGLGraphics::CreateViewport()
{
	_Viewport * view = new _Viewport();
	return view;
}

_Viewport * OpenGLGraphics::CreateViewport(float left, float top, float width, float height) 
{
	_Viewport * view = new _Viewport();
	return view;
}

void OpenGLGraphics::ModifyViewport(float left, float top, float width, float height) 
{
}

void OpenGLGraphics::SetViewport(_Viewport *view) 
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);

}

void OpenGLGraphics::SetViewport(float left, float top, float width, float height)
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	int clientWidth = rect.right-rect.left;
	int clientHeight = rect.bottom-rect.top;
	int viewportLeft = (int)(left*(float)width);
	int viewportTop = (int)(top*(float)height);
	int viewportWidth = (int)(width*(float)clientWidth);
	int viewportHeight = (int)(height*(float)clientHeight);

	glViewport(viewportLeft, viewportTop, viewportWidth , viewportHeight);
}

void OpenGLGraphics::DeleteViewport(_Viewport *view) 
{
}

void OpenGLGraphics::SetPerspective(double fov, double aspect, 
									double nearPos, double farPos)
{
	// Save the current Matrix Mode so we can reset it later.
	int oldMatMode;
	glGetIntegerv(GL_MATRIX_MODE, &oldMatMode);

	// Select The Projection Matrix
	if (oldMatMode != GL_PROJECTION)
		glMatrixMode(GL_PROJECTION);						

	// Reset The Projection Matrix	
	glLoadIdentity();									

	// set the projection
	gluPerspective(fov,aspect,nearPos,farPos);

	// Reset the Matrix mode.
	if (oldMatMode != GL_PROJECTION)
		glMatrixMode(oldMatMode);							
}

void OpenGLGraphics::SetOrtho(double left, double right, double bottom,
		double top, double nearPos, double farPos)
{
	// Save the current Matrix Mode so we can reset it later.
	int oldMatMode;
	glGetIntegerv(GL_MATRIX_MODE, &oldMatMode);

	// Select The Projection Matrix
	if (oldMatMode != GL_PROJECTION)
		glMatrixMode(GL_PROJECTION);						

	// Reset The Projection Matrix	
	glLoadIdentity();									

	// set the ortho projection
	glOrtho(left, right, top, bottom, nearPos, farPos);

	// Reset the Matrix mode.
	if (oldMatMode != GL_PROJECTION)
		glMatrixMode(oldMatMode);							

}


_VertexBuffer* OpenGLGraphics::CreateVertexBuffer(void *buffer, unsigned long bufferbytecount, unsigned long vertexbytecount)
{
	int i;
	_Vector *vp;
	int num = bufferbytecount / vertexbytecount;
	OGLVertex *vb = new OGLVertex;
	vb->posi = NULL;
	vb->norm = NULL;
	vb->tex = NULL;
	_NormalVertex *nvp;
	nvp = (_NormalVertex *)buffer;
	if( &(*nvp).p != NULL )
	{
		vb->posi = new _Vector[num];
		for( i = 0; i < num; i++ )
		{
			vp = &((nvp+i)->p);
			(vb->posi+i)->x = vp->x;
			(vb->posi+i)->y = vp->y;
			(vb->posi+i)->z = vp->z;
		}
	}
	if( &(*nvp).n != NULL )
	{

	}
	return( vb );
}

_IndexBuffer* OpenGLGraphics::CreateIndexBuffer(void *buffer, unsigned long bufferbytecount, unsigned long indexbytecount)
{
	int num = bufferbytecount / indexbytecount;
	_Index *p = new _Index(num);
	memcpy( p, buffer, num * sizeof(_Index) );
	return( p );

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
	OGLVertex *p;
	p = (OGLVertex *)buff;
	delete[]( p->posi );
	delete[]( p->norm );

}

void    OpenGLGraphics::DeleteIndexBuffer(_IndexBuffer *buff)
{
	delete[]( buff );

}


void    OpenGLGraphics::DeleteLight(_Light *light)
{

}

void OpenGLGraphics::SetTransformWorld(_Matrix *matrix)
{
	glMatrixMode( GL_MODELVIEW );
	glMultMatrixf( (float *)matrix );

}

void OpenGLGraphics::SetTransformCamera(_Matrix *matrix)
{
	glMatrixMode( GL_MODELVIEW );
	glMultMatrixf( (float *)matrix );

}

void OpenGLGraphics::SetTransformFOV(_Matrix *matrix)
{
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixf( (float *)matrix );

}

void OpenGLGraphics::SetVertexBuffer(_VertexBuffer *VertexBuffer, short vertexbytecount)
{
	OGLVertex *p = (OGLVertex *)VertexBuffer;
	if( p->posi != NULL )
	{
		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, p->posi );
	} else {
		glDisableClientState( GL_VERTEX_ARRAY );
	}
	if( p->norm != NULL )
	{
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer( GL_FLOAT, 0, p->norm );
	} else {
		glDisableClientState( GL_NORMAL_ARRAY );
	}

}

void OpenGLGraphics::SetIndexBuffer(void* Buffer)
{
	currentIndexBuffer = (_Index *)Buffer;

}

void OpenGLGraphics::DrawTriangleListBuffer(short vertexcount, short indexcount)
{
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_BLEND );
	glEnable( GL_DEPTH );
	glDrawElements( GL_TRIANGLES, indexcount * 3, GL_UNSIGNED_SHORT, currentIndexBuffer );
	glEnable( GL_BLEND );
	glDisable( GL_DEPTH );

}

void OpenGLGraphics::SetLight(short lightnum, _Light *Light)
{

}

void OpenGLGraphics::LightEnabled(short lightnum, short lighton)
{

}


void OpenGLGraphics::DrawTextF(char *buffer, short bufferbytecount, _Rect rect, short LeftCenterRight, _Color *color )
{
	glEnable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );
//	glEnable( GL_TEXTURE_2D );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0, globals->clientwidth, 0, globals->clientheight, -1.0f, 1.0f );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	m_text->displayText( buffer, bufferbytecount, rect, LeftCenterRight, color );
	glPopMatrix();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
//	glDisable( GL_TEXTURE_2D );
}

_Rect OpenGLGraphics::GetCoordsRect()
{
	RECT rect;
	::GetClientRect(m_hWnd, &rect);
	return _Rect(0,0,rect.right,rect.bottom);
}

void OpenGLGraphics::SetForeground(short on)
{
	( on ) ? glDepthMask( GL_TRUE ) : glDepthMask( GL_FALSE );
}

void OpenGLGraphics::Look(double x, double y, double z,
		              double tar_x, double tar_y, double tar_z,
					  double up_x, double up_y, double up_z) 
{
	gluLookAt(x,y,z,x+tar_x,y+tar_y,z+tar_z,up_x,up_y,up_z);	
}

PolygonListObject * OpenGLGraphics::CreatePolygonListObject() 
{ 
	GLPolygonListObject * pObject = new GLPolygonListObject(this); 
	return pObject;
}


MaterialObject * OpenGLGraphics::CreateMaterialObject()
{
	GLMaterialObject * pObject = new GLMaterialObject(this);
	return pObject;
}

Camera * OpenGLGraphics::CreateCamera()
{
	GLCamera * pObject = new GLCamera(this);
	return pObject;
}

Texture2D * OpenGLGraphics::CreateTexture2D()
{
	GLTexture2D * pObject = new GLTexture2D(this);
	return pObject;
}

#endif