#include "graphics/NoGraphics.h"

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
NoGraphicsClass::NoGraphicsClass()
{

}

NoGraphicsClass::~NoGraphicsClass()
{
}


// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  NoGraphicsClass::SetSetup(void *setup)
{
}

short NoGraphicsClass::GetDependencyCount()
{
  return 0;
}

void  NoGraphicsClass::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short NoGraphicsClass::SetDependencyData(short ID, void* Data)
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

void  NoGraphicsClass::GetSetup(void*setup)
{
}

short NoGraphicsClass::CheckStatus()
{
  return 0;
}

// -------------------------------------------
// STANDARD GRAPHICS
// -------------------------------------------
short NoGraphicsClass::Create3DWindow()
{
  return 0;
}

inline void NoGraphicsClass::SetVertexBuffer(void* VertexBuffer, short vertexbytecount)
{
}

inline void NoGraphicsClass::SetTexture(short stage, _Texture* Texture)
{
}

_Material* NoGraphicsClass::CreateMaterial(_Color* Diffuse, _Color* Ambient, _Color* Emissive)
{
  return 0;
}

inline void NoGraphicsClass::SetMaterial(_Material *Mat)
{
}

inline void NoGraphicsClass::SetIndexBuffer(_IndexBuffer *Buffer)
{
}

inline void NoGraphicsClass::DrawTriangleListBuffer(short vertexcount, short indexcount)
{
}

_VertexBuffer* NoGraphicsClass::CreateVertexBuffer(void *buffer, unsigned long bufferbytecount, unsigned long vertexbytecount)
{
  return 0;
}

_IndexBuffer* NoGraphicsClass::CreateIndexBuffer(void *buffer, unsigned long bufferbytecount, unsigned long indexbytecount)
{
  return 0;
}

_Texture* NoGraphicsClass::CreateTexture(char *Filename)
{
  return 0;
}

_Texture* NoGraphicsClass::CreateTexture(short Width, short Height, short MipMap)
{
  return 0;
}

void NoGraphicsClass::SetLight(short lightnum, _Light *Light)
{
}

void NoGraphicsClass::LightEnabled(short lightnum, short lighton)
{
}

_Light* NoGraphicsClass::CreateDirectionalLight(_Vector* Direction, _Color* Color)
{
  return 0;
}

void NoGraphicsClass::DrawTextF(char *buffer, short bufferbytecount, _Rect rect, short LeftCenterRight, _Color*color )
{
}

inline void NoGraphicsClass::BeginScene()
{
}

inline void NoGraphicsClass::ClearSurface(void)
{
}

inline void NoGraphicsClass::EndScene()
{
}

inline void NoGraphicsClass::Flip()
{
}

inline void NoGraphicsClass::SetTransformWorld(_Matrix *matrix)
{
}

inline void NoGraphicsClass::SetTransformCamera(_Matrix *matrix)
{
}

inline void NoGraphicsClass::SetTransformFOV(_Matrix *matrix)
{
}

inline _Rect NoGraphicsClass::GetCoordsRect()
{
  _Rect rect;

  memset(&rect, 0, sizeof(_Rect));

  return rect;
}

inline void NoGraphicsClass::SetForeground(short on)
{
}

bool NoGraphicsClass::CanFlip()
{
  return TRUE;
}

void NoGraphicsClass::DeleteVertexBuffer(_VertexBuffer *buff)
{
};

void NoGraphicsClass::DeleteIndexBuffer(_IndexBuffer *buff)
{
};

void NoGraphicsClass::DeleteTexture(_Texture *tex)
{
}

void NoGraphicsClass::DeleteMaterial(_Material *mat)
{
}

void NoGraphicsClass::DeleteLight(_Light *light)
{
}

// these routines orginate from OpenGL
void NoGraphicsClass::SetIdentity() {}
void NoGraphicsClass::SetModelViewMatrix() {}
void NoGraphicsClass::SetProjectionMatrix() {}
void NoGraphicsClass::SetTextureMatrix() {}

void NoGraphicsClass::Translate(float x, float y, float z) {}
void NoGraphicsClass::Rotate(float angle, float x, float y, float z) {}
void NoGraphicsClass::Scale(float x, float y, float z) {}
void NoGraphicsClass::LoadMatrix() {}
void NoGraphicsClass::MultiMatrix() {}
void NoGraphicsClass::PushMatrix() {}
void NoGraphicsClass::PopMatrix() {}

void NoGraphicsClass::DrawPoint(float x, float y, float z) {}
void NoGraphicsClass::DrawPoint(_GraphicsVector * v) {}
void NoGraphicsClass::DrawPointArray(int vertexCount, _GraphicsVector *v) {}
void NoGraphicsClass::DrawLine(float x1, float y1, float z1,
						   float x2, float y2, float z2) {}
void NoGraphicsClass::DrawRect(float x1, float y1, float x2, float y2) {}
void NoGraphicsClass::DrawTriangle(float x1, float y1, float z1,
					float x2, float y2, float z2, 
					float x3, float y3, float z3) {}
void NoGraphicsClass::DrawTriangle(_GraphicsVector * v1, _GraphicsVector * v2,
							   _GraphicsVector * v3) {}
void NoGraphicsClass::DrawTriangleArray(int vertexCount, _GraphicsVector * v) {}
void NoGraphicsClass::DrawTriangleFan(int vertexCount, _GraphicsVector *v) {}
void NoGraphicsClass::DrawTriangleStrip(int vertexCount, _GraphicsVector *v) {}
void NoGraphicsClass::DrawTriangleArray(int vertexCount, _ShadedVector *v) {}
void NoGraphicsClass::DrawTriangleFan(int vertexCount, _ShadedVector *v) {}
void NoGraphicsClass::DrawTriangleStrip(int vertexCount, _ShadedVector *v) {}
void NoGraphicsClass::DrawQuad() {}
void NoGraphicsClass::DrawString(const char * str) {}

_Viewport * NoGraphicsClass::CreateViewport() { return NULL; }
_Viewport * NoGraphicsClass::CreateViewport(float left, float top, float width, float height) { return NULL; }
void NoGraphicsClass::ModifyViewport(float left, float top, float width, float height) {}
void NoGraphicsClass::SetViewport(_Viewport *view) {}
void NoGraphicsClass::SetViewport(float left, float top, float width, float height) {}
void NoGraphicsClass::SetOrtho(double left, double right, double bottom,
						   double top, double nearPos, double farPos) {}
void NoGraphicsClass::DeleteViewport(_Viewport *view) {}
void NoGraphicsClass::SetPerspective(double fov, double aspect, double nearPos, double farPos) {}

void NoGraphicsClass::BeginPrimitive(PrimitiveTypes) {}
void NoGraphicsClass::EndPrimitive() {}
void NoGraphicsClass::Vertex(float x, float y, float z) {}
void NoGraphicsClass::Normal(float x, float y, float z) {}

void NoGraphicsClass::SetColor(float r, float g, float b, float a) {}
void NoGraphicsClass::SetColor(float r, float g, float b) {}

void NoGraphicsClass::Flush() {}
void NoGraphicsClass::SetClearColor(float r, float g, float b, float a) {}

void NoGraphicsClass::CullFrontFace() {}
void NoGraphicsClass::CullBackFace() {}
void NoGraphicsClass::SetFrontFace(PolygonEnum direct) {}
void NoGraphicsClass::SetTextureMode(int flag) {}
void NoGraphicsClass::Look(double x, double y, double z,
		              double tar_x, double tar_y, double tar_z,
					  double up_x, double up_y, double up_z) {}

PolygonListObject * NoGraphicsClass::CreatePolygonListObject() { return NULL; }
MaterialObject * NoGraphicsClass::CreateMaterialObject() { return NULL; }

Camera * NoGraphicsClass::CreateCamera() { return NULL; }
Texture2D * NoGraphicsClass::CreateTexture2D() { return NULL;}


