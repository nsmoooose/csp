#ifndef __TYPESGRAPHICS_H__
#define __TYPESGRAPHICS_H__

#include "standard/StandardFramework.h"
#include "standard/StandardMath.h"
#include "standard/mtxlib.h"
//#include "graphics/StandardGraphics.h"

// Available Preprocessors for Graphics API
// D3D        - Direct3D 8

// List of API's
const short _NOGRAPHICS     = -1;
const short _D3D8           = 0;
const short _OGL            = 1;

// if Windows default to DX8.
#ifdef D3D
  const short DEFAULT_GRAPHICS = _D3D8;

  #define GRAPHICS_HAS_A_DEFAULT
#endif

// use OGL as the default if we don't use D3D
#ifndef D3D
#ifdef OGL
  const short DEFAULT_GRAPHICS = _OGL;
#define GRAPHICS_HAS_A_DEFAULT
#endif
#endif

// if nothing else then default to nothing.
#ifndef GRAPHICS_HAS_A_DEFAULT
  const short DEFAULT_GRAPHICS = _NOGRAPHICS;
#endif

class StandardGraphics;


#define _VertexBuffer   void
#define _IndexBuffer    void
#define _Texture        void
//#define _Material       void
#define _Light          void
/*
struct _Texture
{
	_Texture(int num) { textureNum = num; }
	int textureNum;
};
*/

enum PrimitiveTypes { FW_POINTS, FW_LINES, FW_LINE_STRIP, FW_LINE_LOOP,
				FW_TRIANGLES, FW_TRIANGLE_STRIP, FW_TRIANGLE_FAN, FW_QUADS,
				FW_QUAD_STRIP, FW_POLYGON };


struct _Color
{
    float r;
    float g;
    float b;
    float a;

	_Color()
	{
	}

	_Color(float _r, float _g, float _b)
	{
		r = _r; g = _g; b = _b; a = 1.0;
	}

	_Color(float _r, float _g, float _b, float _a)
	{
		r = _r; g = _g; b = _b; a = _a;
	}

	BOOL operator!=( _Color cs )
	{
		if( (r!=cs.r) || (g!=cs.g) || (b!=cs.b) || (a!=cs.a) )
		{
			return( TRUE );
		}
		return( FALSE );
	}
};

struct _Material
{
	_Color * emission;
	_Color * specular;
	_Color * diffuse;
	_Color * ambient;

	float  *shininess;

	_Material()
	{
		emission = NULL;
		diffuse = NULL;
		ambient = NULL;
		specular = NULL;
		shininess = NULL;
	}
	virtual ~_Material()
	{
		delete(emission);
		delete(diffuse);
		delete(ambient);
		delete(specular);
		delete(shininess);
	}
};

struct _Rect
{
    long x1;
    long y1;
    long x2;
    long y2;

	_Rect()
	{
		x1 = 0;
		y1 = 0;
		x2 = 0;
		y2 = 0;
	}

	_Rect(long  _x1,long _y1,long _x2,long _y2)
	{
		x1 = _x1;
		y1 = _y1;
		x2 = _x2;
		y2 = _y2;
	}
};

typedef struct _NormalVertexStruct
{
    _Vector       p;
    _Vector       n;
    float         tu, tv;
} _NormalVertex;

struct _GraphicsVector
{
	float x;
	float y;
	float z;
};

struct _ShadedVector 
{
	float x;
	float y;
	float z;
	float nx;
	float ny;
	float nz;
};

struct _Viewport
{
	int left;
	int top;
	int width;
	int height;
};

typedef unsigned short _Index;



/** \class PolygonListObject
 * This class simple list of triangles
 */


class PolygonListObject 
{
public:

	PolygonListObject(StandardGraphics * pDisplay) 
		{ }

	virtual ~PolygonListObject() {}

	virtual void Create(PrimitiveTypes type, int vertexCount, _GraphicsVector * v) = 0;
	virtual void Create(PrimitiveTypes type, int vertexCount, _GraphicsVector * v, _GraphicsVector *n) = 0;
	virtual void Create(PrimitiveTypes type, int vertexCount, _NormalVertexStruct *v) = 0;
	virtual void SetIndices(int indexCount, int size, void * indices) = 0;
	virtual void Destroy() = 0;

	virtual void Draw() = 0;


	virtual void SetName(char * name) = 0;
	virtual char * GetName() = 0;

	virtual void SetMatrix(_Matrix *  mat) = 0;
	virtual void MultiMatrix(_Matrix * mat) = 0;
	virtual void Translate(float X, float Y, float Z) = 0;
	virtual void Rotate(float angle, float x, float y, float z) = 0;


};


/** \class MaterialObject
 * This class servers as a base class for a material which
 * is a generized color format.
 */
class MaterialObject
{
public:
	MaterialObject(StandardGraphics * pDisplay) 
		{}
	virtual ~MaterialObject() {}
	virtual void Create(_Color * emission,
						_Color * specular,
						_Color * diffuse,
						_Color * ambient,
						float shininess) = 0;
	virtual void Destroy() = 0;
	virtual void Apply() = 0;
	virtual MaterialObject * GetCurrentMaterial() = 0;



};

class Texture2D
{
public:
	Texture2D(StandardGraphics * pDisplay) 
		{}
	virtual ~Texture2D() {}
	virtual void Create(const char * filename, int filetype) = 0;


	virtual void Destroy() = 0;
	virtual void Apply() = 0;

protected:

};

/** \class Font2D
 * This class represents an abstract 2D font.
 */
class Font2D
{
public:
	Font2D(StandardGraphics * pDisplay)	{}
	virtual ~Font2D() {}
	virtual void Create(const char * filename, int type) = 0;
	virtual void DrawTextF(char *buffer, short bufferbytecount, _Rect rect, short LeftCenterRight, _Color*color) = 0;
	virtual void Begin() = 0;
	virtual void End() = 0;
};

/** \class Camera
 * The camera takes sets the vantage point within the scene.
 * It is responsible for setting the perspective, position
 * and direction of viewing. The camera needs to be positioned
 * at the begining of the rendering loop. The camera will also
 * take care of right vs left handed coordinate systems transparently.
 */
class Camera
{
public:
	Camera(StandardGraphics * pDisplay)
		{ }
	virtual ~Camera() {}



	virtual void SetPosition(float loc_x, float loc_y, float loc_z,
		                     float target_x, float target_y, float target_z,
							 float up_x, float up_y, float up_z) = 0;
	virtual void SetPosition(_Vector * loc, _Vector * target, _Vector * up) = 0;
	virtual void SetPosition(vector3 &, vector3 &, vector3 &) = 0;
	virtual void SetPosition(float roll, float pitch, float heading,
		                     float loc_x, float loc_y, float loc_z) = 0;
	
	virtual void SetMatrix(_Matrix *  mat) = 0;
	virtual void MultiMatrix(_Matrix * mat) = 0;
	virtual void Translate(float X, float Y, float Z) = 0;
	virtual void Rotate(float angle, float x, float y, float z) = 0;
	virtual void Reset() = 0;

	virtual void SetPerspectiveView(float angle, float nearplane, float farplane) = 0;
	virtual void SetOrthoView(float nearplane, float farplane) = 0;

protected:


private:
	Camera() {}

};

// common structs.
struct _GRAPHICS_DESCRIPTION
{
  char   *Name;
  short   ID;
};

struct _STANDARD_GRAPHICS
{

  short   width;
  short   height;
  short   clientwidth;
  short   clientheight;

  short   fullscreen;
  short   depth;
  short   texturedepth;
  short   hardwaretl;
  short   vsync;
  short   wireframe;
  short   miplevels;

  short   ID;
  void*   window;
};


enum PolygonEnum { CLOCKWISE, COUNTERCLOCKWISE };

#endif