///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Base code header file based upon Jeff Molofee's Revised OpenGL Basecode
//	 found at http://nehe.gamedev.net  
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	09/25/2001	file created	-	Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#ifndef GL_FRAMEWORK__INCLUDED
#define GL_FRAMEWORK__INCLUDED

#define MAX_STR_LENGTH 256
#define MAP_TYPE_SCN	1
#define MAP_TYPE_RANDOM 2

#define DLG_LOAD_NEW			1					// DlgStartup return values
#define DLG_GENERATE_RANDOM		2
#define DLG_LOAD_DEFAULT		3

#define MODE_FRACTAL			1
#define MODE_QUAD_MAP			2

#define RENDER_MODE_SQUARES		1
#define RENDER_MODE_TRIANGLES	2
#define RENDER_MODE_SHADED		3


#include <windows.h>								// Header File For Windows

typedef struct {									// Structure For Keyboard Stuff
	BOOL keyDown [256];								// Holds TRUE / FALSE For Each Key
} Keys;												// Keys

typedef struct {									// Contains Information Vital To Applications
	HINSTANCE		hInstance;						// Application Instance
	const char*		className;						// Application ClassName
} Application;										// Application

typedef struct {									// Window Creation Info
	Application*		application;				// Application Structure
	char*				title;						// Window Title
	int					width;						// Width
	int					height;						// Height
	int					bitsPerPixel;				// Bits Per Pixel
	BOOL				isFullScreen;				// FullScreen?
} GL_WindowInit;									// GL_WindowInit

typedef struct {									// Contains Information Vital To A Window
	Keys*				keys;						// Key Structure
	HWND				hWnd;						// Window Handle
	HDC					hDC;						// Device Context
	HGLRC				hRC;						// Rendering Context
	GL_WindowInit		init;						// Window Init
	BOOL				isVisible;					// Window Visible?
	DWORD				lastTickCount;				// Tick Counter
} GL_Window;										// GL_Window

void TerminateApplication (GL_Window* window);		// Terminate The Application

void ToggleFullscreen (GL_Window* window);			// Toggle Fullscreen / Windowed Mode

// These Are The Function You Must Provide
BOOL Initialize (GL_Window* window, Keys* keys);	// Performs All Your Initialization
void Deinitialize (void);							// Performs All Your DeInitialization
void Update (DWORD milliseconds);					// Perform Motion Updates
void Draw (void);									// Perform All Your Scene Drawing
void DrawFractalMesh(void);
void UpdateCamera();								// called after every direction change input
void CalculateFPS(DWORD milliseconds);
void ProcessFractKeys(DWORD milliseconds);

#endif												// GL_FRAMEWORK__INCLUDED
