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

#pragma once
#include <windows.h>								// Header File For Windows
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <math.h>
#include <float.h>
#include "n_math.h"

#include <gl\gl.h>								// Header File For The OpenGL32 Library
#include <gl\glu.h>								// Header File For The GLu32 Library
#include <gl\glaux.h>								// Header File For The GLaux Library
#include "glext.h"								// Header File For Multitexturing

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
#define RENDER_MODE_TEXTURED	4

#define SHADING_MODE_ECO_COLOR		1
#define SHADING_MODE_ECO_TEXTURE	2
#define SHADING_MODE_REL_ELEVATION	3
#define SHADING_MODE_SLOPE			4

#define ECOSYSTEM_MODE_SHARP	1
#define ECOSYSTEM_MODE_SMOOTH	2

#define __ARB_ENABLE true							// Used To Disable ARB Extensions Entirely
#define MAX_EXTENSION_SPACE 10240					// Characters For Extension-Strings
#define MAX_EXTENSION_LENGTH 256					// Maximum Characters In One Extension-String
#define WM_TOGGLEFULLSCREEN (WM_USER+1)				// Application Define Message For Toggling

#define UNKNOWN		2

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
void UpdateCamera();								// called after every direction change input
void CalculateFPS(DWORD milliseconds);
void ProcessFractKeys(DWORD milliseconds);
bool initMultitexture(void);
bool isInString(char *string, const char *search);

inline int __stdcall ifloor(float  x)
{
  unsigned long e = (0x7F + 31) - ((* (unsigned long *) &x & 0x7F800000) >> 23);
  unsigned long m = 0x80000000 | (* (unsigned long *) &x << 8);
  return (m >> e) & -(e < 32);
};

inline unsigned char __stdcall FloatToByte(float  x)
{
  float  t = x + (float) 0xC00000;
  return * (unsigned char *) &t;
};

#endif												// GL_FRAMEWORK__INCLUDED
