#ifndef _OPENGLGRAPHICS_H_
#define _OPENGLGRAPHICS_H_


//#include "StandardGraphics.h"
#include <windows.h>

#include "graphics/StandardGraphics.h"


#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#include "Font.h"

//LRESULT CALLBACK WndProc(HWND	hWnd,			// Handle For This Window
//							UINT	uMsg,			// Message For This Window
//							WPARAM	wParam,			// Additional Message Information
//							LPARAM	lParam);			// Additional Message Information



//struct GraphicsVector
//{
//	float x;
//	float y;
//	float z;
//};

//struct OGLMaterial
//{
//	GLfloat		*ambient;
//	GLfloat		*diffuse;
//	GLfloat		*emissive;
//	GLfloat		*specular;
//	GLfloat		shiny;
//};

//struct OGLTexture
//{
//	int			texNum;
//	float		*uvcoords;
//};

struct OGLVertex
{
	_Vector				*posi;
	_Vector				*norm;
//	OGLMaterial			*mat;
	float				*tex;
};

class OpenGLGraphics : public StandardGraphics 
{


public:

	OpenGLGraphics();
	virtual ~OpenGLGraphics();

	// window management routines

	virtual short Create3DWindow();
//	virtual short Create3DWindow(char* title, int left, int top, int width, int height, int bits, bool fullscreenflag);

	// creation routines
	virtual PolygonListObject * CreatePolygonListObject();
	virtual MaterialObject * CreateMaterialObject();
	virtual Camera * CreateCamera();
	virtual Texture2D * CreateTexture2D();



	virtual GLvoid Kill3DWindow(GLvoid);
	virtual GLvoid Resize3DWindow(GLsizei width, GLsizei height);
	virtual void Show3DWindow();
	virtual void Hide3DWindow();

	// Standard framework routines
    virtual void  SetSetup(void *setup);
    virtual short GetDependencyCount();
    virtual void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
    virtual short SetDependencyData(short Index, void* Data);
    virtual void  GetSetup(void*setup);
    virtual short CheckStatus();

	// Graphics Basic routines/non drawing 
	BOOL InitGL();
	virtual void Flip();
	virtual bool CanFlip();
	virtual void Flush();

	void ClearSurface();
	void SetClearColor(float r, float g, float b, float a);
	void SetColor(float r, float g, float b);
	void SetColor(float r, float g, float b, float a);
	void SetColor(_Color * color);

	// lighting/shading routines
	void SetFlatShading();
	void SetSmoothShading();
	void EnableLighting();
	void DisableLighting();

	void EnableDepthTest();
	void DisableDepthTest();

	void SetAmbientLight(_Color * color);

//	virtual BOOL BeginScene();
//	virtual BOOL EndScene();

	virtual void BeginScene();
	virtual void EndScene();

	// Matrix routines
	void SetIdentity();
	void SetModelViewMatrix();
	void SetProjectionMatrix();
	void SetTextureMatrix();

	void Translate(float x, float y, float z);
	void Rotate(float angle, float x, float y, float z);
	void Scale(float x, float y, float z);
	void LoadMatrix();
	void MultiMatrix();
	void PushMatrix();
	void PopMatrix();

	// Drawing routines

	void DrawPoint(float x, float y, float z);
	void DrawPoint(_GraphicsVector * v);
	void DrawPointArray(int vertexCount, _GraphicsVector *v);
	void DrawLine(float x1, float y1, float z1,
							  float x2, float y2, float z2);
	void DrawRect(float x1, float y1, float x2, float y2);
	void DrawTriangle(float x1, float y1, float z1,
					float x2, float y2, float z2, 
					float x3, float y3, float z3);
	void DrawTriangle(_GraphicsVector * v1, _GraphicsVector * v2,
					_GraphicsVector * v3);
	void DrawTriangleArray(int vertexCount, _GraphicsVector * v);
	void DrawTriangleFan(int vertexCount, _GraphicsVector *v);
	void DrawTriangleStrip(int vertexCount, _GraphicsVector *v);
	void DrawTriangleArray(int vertexCount, _ShadedVector *v);
	void DrawTriangleFan(int vertexCount, _ShadedVector *v);
	void DrawTriangleStrip(int vertexCount, _ShadedVector *v);
	void DrawQuad();
	void DrawString(const char * str);

	// Display List routines
	void StartList(int index);
	void EndList();
	void DisplayList(int index);

	// materials
    virtual _Material*      CreateMaterial(_Color *Diffuse, _Color *Ambient, _Color *Emissive);
	virtual _Material*      CreateMaterial(_Color *Diffuse, _Color *Ambient, _Color *Emissive,
						_Color *Specular, float *shininess);
	virtual   void    SetMaterial(_Material*Mat);
    virtual   void    DeleteMaterial(_Material *mat);


	// Textures
    virtual _Texture*       CreateTexture(char *Filename);
    virtual _Texture*       CreateTexture(short Width, short Height, short MipMap);
	virtual  void    SetTexture(short stage, _Texture* Texture);
    virtual   void    DeleteTexture(_Texture *tex);
	virtual void SetTextureMode(int flag);


	// Viewports
	virtual _Viewport * CreateViewport();
	virtual _Viewport * CreateViewport(float left, float top, float width, float height);
	virtual void ModifyViewport(float left, float top, float width, float height);
	virtual void SetViewport(_Viewport *view);
	virtual void SetViewport(float left, float top, float width, float height);
	virtual void SetOrtho(double left, double right, double bottom,
		double top, double nearPos, double farPos);

	virtual void CullFrontFace();
	virtual void CullBackFace();
	virtual void SetFrontFace(PolygonEnum direct);


	virtual void DeleteViewport(_Viewport *view);

	virtual void SetPerspective(double fov, double aspect, double near, double far);

	// drawing primitive
	virtual void BeginPrimitive(PrimitiveTypes type);
	virtual void EndPrimitive();
	virtual void Vertex(float x, float y, float z);
	virtual void Normal(float x, float y, float z);



	// stuff from D3D class
	virtual _VertexBuffer*  CreateVertexBuffer(void *buffer, unsigned long bufferbytecount, unsigned long vertexbytecount);
    virtual _IndexBuffer*   CreateIndexBuffer(void *buffer, unsigned long bufferbytecount, unsigned long indexbytecount);
    virtual _Light*         CreateDirectionalLight(_Vector* Direction, _Color* Color);

    virtual   void    DeleteVertexBuffer(_VertexBuffer *buff);
    virtual   void    DeleteIndexBuffer(_IndexBuffer *buff);
    virtual   void    DeleteLight(_Light *light);

    virtual void            SetTransformWorld(_Matrix *matrix);
    virtual void            SetTransformCamera(_Matrix *matrix);
    virtual void            SetTransformFOV(_Matrix *matrix);

    virtual void            SetVertexBuffer(_VertexBuffer *VertexBuffer, short vertexbytecount);
    virtual void            SetIndexBuffer(void* Buffer);
    virtual void            DrawTriangleListBuffer(short vertexcount, short indexcount);
    virtual void            SetLight(short lightnum, _Light *Light);
    virtual void            LightEnabled(short lightnum, short lighton);

    virtual void            DrawTextF(char *buffer, short bufferbytecount, _Rect rect, short LeftCenterRight, _Color *color );
    virtual _Rect           GetCoordsRect();
    virtual void            SetForeground(short on);
	virtual void            Look(double x, double y, double z,
		              double tar_x, double tar_y, double tar_z,
					  double up_x, double up_y, double up_z);



	// Working toward being a subclass of StandardGraphics
//	_Material * CreateMaterial( _Color *, _Color *, _Color * );

private:

//	BOOL processMessage();
	void dumpAllPixelFormats(HDC hDC);
	void dumpPixelFormat(PIXELFORMATDESCRIPTOR pfd);



	bool m_fullscreen;
	HDC			m_hDC;  		// Private GDI Device Context
	HGLRC		m_hRC;		// Permanent Rendering Context
	HWND		m_hWnd;		// Holds Our Window Handle
	HINSTANCE	m_hInstance;		// Holds The Instance Of The Application
	MSG       m_msgCur;
  _STANDARD_GRAPHICS *globals;
	BOOL m_depthFlag;

	Font	*m_text;
	_Index	*currentIndexBuffer;

//	bool	m_keys[256];			// Array Used For The Keyboard Routine

};

#endif

