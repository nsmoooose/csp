#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

// Command Line Arugments supported inside Framework
// command     - this will load the project in command line mode.

#ifdef WIN32
#include <winsock2.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

// ANSI C/C++ headers
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <assert.h>
#include <ctype.h>

// STD C++ Headers
#include <vector>
#include <list>
#include <ostream>
#include <istream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

// Logging Headers
#include "debug_types.h"
#include "LogStream.h"

// OpenGL Headers
#ifdef OGL
#include <GL/gl.h>			// Header File For The OpenGL32 Library
#include <GL/glu.h>			// Header File For The GLu32 Library

#ifdef WIN32
#include <GL/glaux.h>		// Header File For The Glaux Library
#endif

#ifndef	GL_VERSION_1_2
#include "GLExt.h"
#endif
#endif

#ifdef SDL
#include "SDL.h"
#include "SDL_image.h"
#endif

#ifdef LINUX
#include <GL/glx.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>
#endif


// If we are using Windows then Include DirectX headers
// The Direct3D ones are only included if we are also
// using Direct3D.
#ifdef WIN32
#include <dinput.h>
#include <dsound.h>
#include <process.h>
#ifdef D3D
#include <d3d8.h>
#include <d3dx8.h>
#endif
#endif


#ifdef HAVE_OPENAL
#include <AL/al.h>
#include <AL/alu.h>
#include <AL/altypes.h>
#endif


// proto types and standard classes.

class StandardApp;
class StandardFramework;
class StandardWindow3D;
class StandardBuffer;
class StandardPolygonMesh;
class StandardPolygonFlat;
class StandardPolygonBillboard;
class StandardTexture;
class StandardMaterial;
class StandardViewport;
class StandardCamera;
class StandardWindow;
class StandardScreen;
class StandardButton;
class StandardLabel;
class StandardEdit;
class StandardColor;


#include "License.h"
#include "TypesError.h"

#include "TypesEndian.h"
#include "TypesFile.h"
#include "TypesBuffer.h"

#include "TypesMath.h"
#include "TypesVector3.h"
#include "TypesVector4.h"
#include "TypesMatrix3.h"
#include "TypesMatrix4.h"
#include "TypesQuaternion.h"
#include "TypesColor.h"



// DEFINES

#define FRAMEWORK_VERSION "v1.3"

typedef void* _INSTANCE;
typedef void* _ICON;
typedef void* _SHOW;

struct _APP_INIT_STRUCT
{
  bool      PreviousInstance;
  char*     CommandLine;
  char*     ShortName;
  char*     LongName;
  _INSTANCE Instance;
  _ICON     Icon;
  _SHOW     Show;
  _APP_INIT_STRUCT() { CommandLine = NULL; ShortName = NULL; LongName = NULL; }
  _APP_INIT_STRUCT(_APP_INIT_STRUCT * pAppInit) 
  { 
    PreviousInstance = pAppInit->PreviousInstance;
    CommandLine = pAppInit->CommandLine;
    ShortName = pAppInit->ShortName;
    LongName = pAppInit->LongName;
    Instance = pAppInit->Instance;
    Icon = pAppInit->Icon;
    Show = pAppInit->Show;
  }
};

struct _FRAMEWORK_INIT_STRUCT
{
  bool GraphicsFlag;
  bool InputFlag;
  bool NetworkingFlag;
  bool TextWindowFlag;
  bool GUIFlag;
  bool LoggingFlag;
  bool AudioFlag;
};

struct _Rect
{
  float x1;
  float y1;
  float x2;
  float y2;
};

typedef void* _HANDLE;
typedef void* _FORMAT;
typedef void* _DEVICE;
typedef void* _FONT;
typedef void* _SURFACE;
typedef unsigned char _MIPMAP;

#define MAX_MIPMAP  0

struct _GRAPHICS_INIT_STRUCT
{
  unsigned short  Width;
  unsigned short  Height;
  unsigned short  Depth;
  unsigned short  ZBuffDepth;
  bool            Fullscreen;
  bool            VSync;
  bool            HardwardTL;
  bool            Wireframe;
  float           NearZ;
  float           FarZ;
  _HANDLE         Handle;
  _MIPMAP         MipLevels;
};

struct _Point
{ 
  _Point() {};
  _Point(float _x, float _y) { x = _x; y = _y; }
  float x;
  float y;
};

struct _GUI_KEY
{
  bool alt;
  bool shift;
  bool ctrl;
  unsigned char key;
};

struct _GUI_MOUSE
{
bool down;
_Point pt;
};

typedef unsigned char _KEY_UNBUFFERED;

struct _KEY_BUFFERED
{
bool down;
unsigned char key;
};

struct _MOUSE
{
int x;
int y;
int z;
bool mouse1;
bool mouse2;
bool mouse3;
};

struct _JOYSTICK
{
char x;
char y;
char z;
char throttle;
short hat;
short button1;
short button2;
short button3;
short button4;
};

struct _GUI
{
char Button;
char Window;
char Screen;
};

typedef unsigned short _Index;

enum StateTypes { FW_WINDING, FW_SHADING, FW_LIGHTING, 
	              FW_ZBUFFER, FW_CULLFACE, FW_TEXTURE, FW_BLENDING, FW_FOG, 
				  FW_FOGMODE, FW_FOGCOLOR, FW_FOGSTART, FW_FOGEND, FW_FOGDENSITY,
				  FW_WIREFRAME, FW_ZBUFFERWRITE};

enum PrimitiveTypes { FW_POINTS, FW_LINES, FW_LINE_STRIP, FW_LINE_LOOP,
				            FW_TRIANGLES, FW_TRIANGLE_STRIP, FW_TRIANGLE_FAN, FW_QUADS,
				            FW_QUAD_STRIP, FW_POLYGON };

enum PolygonWindingEnum { FW_CW, FW_CCW };

enum CullFaceTypes { FW_BACKFACE, FW_FRONTFACE };

enum ShadingTypes { FW_SMOOTH, FW_FLAT };

enum FogModes { FW_FOG_EXP, FW_FOG_EXP2, FW_FOG_LINEAR };

class StandardRay
{
public:
  StandardVector3 Position;
  StandardVector3 Direction;

  StandardRay()
  {
    Position.x = 0; Position.y = 0; Position.z = 0;
    Direction.x = 0; Direction.y = 0; Direction.z = 0;
  }
  StandardVector3 GetDirection()
  {
    return Direction;
  }
  StandardVector3 GetPosition()
  {
    return Position;
  }

};

/** StandardVertex
* Describes a vertex with only regular coords.
*/
class StandardVertex
{
public:
    StandardVector3 p;
    StandardVertex()  {}
    ~StandardVertex() {}
	void Initialize(float px, float py, float pz)
    {
        p.x = px; p.y = py; p.z = pz;
    }
};


/** StandardNTVertex
* Describes a vertex with both normals and textures.
*/
class StandardNTVertex
{
public:
  StandardVector3       p;
  StandardVector3       n;
  float                 tu, tv;
	StandardNTVertex()    {}
  ~StandardNTVertex()   {}

	void Initialize(float px, float py, float pz,
		              float nx, float ny, float nz,
				          float _tu, float _tv)
	{
		p.x = px; p.y = py; p.z = pz;
		n.x = nx; n.y = ny; n.z = nz;
		tu = _tu; tv = _tv;
	}
	void Initialize(float px, float py, float pz,
				          float _tu, float _tv)
	{
		p.x = px; p.y = py; p.z = pz;
		n.x = 0; n.y = 0; n.z = 0;
		tu = _tu; tv = _tv;
	}
	void SetNormal(float nx, float ny, float nz)
	{
		n.x = nx; n.y = ny; n.z = nz;
	}
};

/** StandardTVertex
* Describes a Vertex with a Texture attribute.
*/
class StandardTVertex
{
public:
  StandardVector3       p;
  float                 tu, tv;
	StandardTVertex()     {}
  ~StandardTVertex()    {}

	void Initialize(float px, float py, float pz,
		         	    float _tu, float _tv)
	{
		p.x = px; p.y = py; p.z = pz;
		tu = _tu; tv = _tv;
	}

};

/** StandardCVertex
*  Describes a vertex with a color attribute.
*/
class StandardCVertex
{
public:
  StandardVector3       p;
	StandardColor         c;
	StandardCVertex()     {}
  ~StandardCVertex()    {}

	void Initialize(float px, float py, float pz,
		         	    unsigned char r, unsigned char g,
					        unsigned char b, unsigned char a)
	{
		p.x = px; p.y = py; p.z = pz;
		c.r = r; c.g = g; c.b = b; c.a = a;
	}
	void Initialize(StandardVector3 _p, StandardColor _c)
	{
		p = _p; c = _c;	
	}
};

/** StandardTLVertex
*  Describes a vertex with a color attribute.
*/
class StandardTLVertex
{
public:
  StandardVector3       p;
  float                 rhw;
	StandardColor         c;

	StandardTLVertex()     {}
  ~StandardTLVertex()    {}

	void Initialize(float px, float py,
		         	    unsigned char r, unsigned char g,
					        unsigned char b, unsigned char a)
	{
		p.x = px; p.y = py; p.z = 0;
    rhw = 1;
		c.r = r; c.g = g; c.b = b; c.a = a;
	}
	void Initialize(StandardVector3 _p, StandardColor _c)
	{
		p = _p; c = _c;	
    rhw = 1;
	}
};

// Framework Implementation Classes

#include "TypesWindowText.h"
#include "TypesAudio.h"
#include "TypesInput.h"
#include "TypesNetwork.h"
#include "TypesCamera.h"
#include "TypesGraphics.h"
#include "TypesViewport.h"
#include "TypesPolygonMesh.h"
#include "TypesPolygonFlat.h"
#include "TypesPolygonBillboard.h"
#include "TypesLOD.h"
#include "TypesMaterial.h"
#include "TypesTexture.h"
#include "TypesEdit.h"
#include "TypesLabel.h"
#include "TypesButton.h"
#include "TypesWindow.h"
#include "TypesScreen.h"
#include "TypesGUI.h"
#include "TypesWindow3d.h"
#include "TypesFactory.h"
#include "TypesFramework.h"
#include "TypesApp.h"

#include "TypesMath.inl"
#include "GeometryMeshLib.h"
#include "BaseApp.h"


#endif
