#ifdef OGL

#ifndef _GLWINGRAPHICS_H_
#define _GLWINGRAPHICS_H_



#include "Framework.h"
#include "GLGraphics.h"

class WinGLGraphics : public GLGraphics 
{

private:

	void dumpAllPixelFormats(HDC hDC);
	void dumpPixelFormat(PIXELFORMATDESCRIPTOR pfd);

	HDC			m_hDC;  		// Private GDI Device Context
	HGLRC		m_hRC;		// Permanent Rendering Context
	HWND		m_hWnd;		// Holds Our Window Handle
	HINSTANCE	m_hInstance;		// Holds The Instance Of The Application


public:

	WinGLGraphics();
	~WinGLGraphics();

	short       Initialize(StandardApp *App, _GRAPHICS_INIT_STRUCT * Init);
	void        Uninitialize();

	virtual void        Resize(short width, short height);
	virtual void        Flip();

	virtual void        BeginScene();
	virtual void        EndScene();



};

#endif

#endif