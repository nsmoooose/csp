// Direct3D specific for StandardGraphics

#include "graphics/StandardGraphics.h"
//#include "StandardPlatform.h"

#include <d3d8.h>
#include <d3dx8.h>

#define _NormalVertexType (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

class D3DGraphics: public StandardGraphics
{

private:

  _STANDARD_GRAPHICS *globals;

  LPDIRECT3D8 D3DObj;
  LPDIRECT3DDEVICE8 Device;
  LPD3DXFONT defaultfont;
  D3DCOLOR colorkey;
  D3DCOLOR skycolor;

  D3DFORMAT GetTextureDepth();

  FrameworkError Error;

  //StandardPlatform *platform;

public:
  D3DGraphics();
  ~D3DGraphics();

  // Standard Framework - required for every framework object
  void  SetSetup(void *setup);
  short GetDependencyCount();
  void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
  short SetDependencyData(short Index, void* Data);
  void  GetSetup(void *setup);
  short CheckStatus();

  inline LPDIRECT3DDEVICE8 GetDevice() { return Device; }

  // Standard Graphics
  short           Create3DWindow();
  _Texture*           CreateTexture(short Width, short Height, short MipMap);
  _Light*         CreateDirectionalLight(_Vector* Direction, _Color* Color);
  _VertexBuffer*  CreateVertexBuffer(void *buffer, unsigned long bufferbytecount, unsigned long vertexbytecount);
  _IndexBuffer*   CreateIndexBuffer(void *buffer, unsigned long bufferbytecount, unsigned long indexbytecount);
  _Texture*       CreateTexture(char *Filename);
  _Material*      CreateMaterial(_Color *Diffuse, _Color *Ambient, _Color *Emissive);

  virtual PolygonListObject * CreatePolygonListObject();
  virtual MaterialObject * CreateMaterialObject();
  virtual Camera * CreateCamera();
  virtual Texture2D * CreateTexture2D();


  friend class D3DPolygonListObject;
  friend class D3DMaterialObject;
  friend class D3DCamera;
  friend class D3DTexture2D;




  void    BeginScene();
  void    ClearSurface();
  void    EndScene();
  void    Flip();
  void    SetVertexBuffer(_VertexBuffer *VertexBuffer, short vertexbytecount);
  void    SetShader(long type);
  void    SetTexture(short stage, _Texture* Texture);
  void    SetMaterial(_Material*Mat);
  void    DrawVertexBuffer(short polygoncount);
  void    SetIndexBuffer(_IndexBuffer* Buffer);
  void    DrawTriangleListBuffer(short vertexcount, short indexcount);
  void    SetTransformWorld(_Matrix *matrix);
  void    SetTransformCamera(_Matrix *matrix);
  void    SetTransformFOV(_Matrix *matrix);
  void    SetLight(short lightnum, _Light*Light);
  void    LightEnabled(short lightnum, short lighton);
  void    DrawTextF(char *buffer, short bufferbytecount, _Rect rect, short LeftCenterRight, _Color *color );
  _Rect   GetCoordsRect();
  void    SetForeground(short on);
  bool    CanFlip();

  void    DeleteVertexBuffer(_VertexBuffer *buff);
  void    DeleteIndexBuffer(_IndexBuffer *buff);
  void    DeleteTexture(_Texture *tex);
  void    DeleteMaterial(_Material *mat);
  void    DeleteLight(_Light *light);

  // these routines orginate from OpenGL
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

  _Viewport * CreateViewport();
  _Viewport * CreateViewport(float left, float top, float width, float height);
  void ModifyViewport(float left, float top, float width, float height);
  void SetViewport(_Viewport *view);
  void SetViewport(float left, float top, float width, float height);
  void SetOrtho(double left, double right, double bottom,
		double top, double nearPos, double farPos);
  void DeleteViewport(_Viewport *view);
  void SetPerspective(double fov, double aspect, double nearPos, double farPos);

  void BeginPrimitive(PrimitiveTypes);
  void EndPrimitive();
  void Vertex(float x, float y, float z);
  void Normal(float x, float y, float z);

  void SetColor(float r, float g, float b, float a);
  void SetColor(float r, float g, float b);

  void Flush();
  void SetClearColor(float r, float g, float b, float a);

  void CullFrontFace();
  void CullBackFace(); 
  void SetFrontFace(PolygonEnum direct);
  void SetTextureMode(int flag);

  void Look(double x, double y, double z,
		              double tar_x, double tar_y, double tar_z,
					  double up_x, double up_y, double up_z);


};