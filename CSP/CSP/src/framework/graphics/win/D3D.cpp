#ifdef D3D

#include "D3D.h"
#include "D3DPolygonListObject.h"
#include "D3DMaterialObject.h"
#include "D3DCamera.h"
#include "D3DTexture2D.h"


// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
D3DGraphics::D3DGraphics()
{
  globals = 0;

  defaultfont = 0;
  Device = 0;
  D3DObj = 0;

}

D3DGraphics::~D3DGraphics()
{

  _REL(defaultfont);
  _REL(Device);
  _REL(D3DObj);

  globals = 0;
}


// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  D3DGraphics::SetSetup(void *setup)
{
  globals = (_STANDARD_GRAPHICS *)setup;
}

short D3DGraphics::GetDependencyCount()
{
  return 0;
}

void  D3DGraphics::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
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

short D3DGraphics::SetDependencyData(short ID, void* Data)
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

void  D3DGraphics::GetSetup(void*setup)
{
  setup = globals;
}

short D3DGraphics::CheckStatus()
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

// -------------------------------------------
// STANDARD GRAPHICS
// -------------------------------------------
short D3DGraphics::Create3DWindow()
{

  D3DFORMAT tryformat = D3DFMT_UNKNOWN;
  D3DFORMAT depthformat = D3DFMT_UNKNOWN;
  D3DFORMAT displayformat = D3DFMT_UNKNOWN;

  short threeshot;

  D3DObj = Direct3DCreate8( D3D_SDK_VERSION );

  D3DDISPLAYMODE d3ddm;

  D3DObj->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );

  D3DPRESENT_PARAMETERS d3dpp; 
  ZeroMemory( &d3dpp, sizeof(d3dpp) );
  if(globals->fullscreen == TRUE)
  {
    d3dpp.Windowed   = FALSE;

    for(threeshot = 0; threeshot < 3; threeshot ++)
    {
      switch(globals->depth)
      {
        case 32:
          switch(threeshot)
          {
            case 0:
              tryformat = D3DFMT_A8R8G8B8;
              break;
            case 1:
              tryformat = D3DFMT_X8R8G8B8;
              break;
            case 2:
              tryformat = D3DFMT_R8G8B8;
              break;
          }
          break;
        default:
          switch(threeshot)
          {
            case 0:
              tryformat = D3DFMT_R5G6B5;
              break;
            case 1:
              tryformat = D3DFMT_X1R5G5B5;
              break;
            case 2:
              tryformat = D3DFMT_A1R5G5B5;
              break;
          }
          break;
      }
      Error.lError = D3DObj->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, tryformat, tryformat, FALSE);

      if(Error.lError == 0)
      {
        displayformat = tryformat;
        break;
      }
    }
    
    if(displayformat == D3DFMT_UNKNOWN)
    {
      return _FUNKY_VIDEO_FORMAT;
    }

	  if(globals->vsync == FALSE)
	  {
		  d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	  }
  }
  else
  {
    d3dpp.Windowed   = TRUE;
    displayformat = d3ddm.Format;
    globals->width = globals->clientwidth;
    globals->height = globals->clientheight;
  }

  d3dpp.BackBufferFormat = displayformat;
  d3dpp.BackBufferWidth = globals->width;
  d3dpp.BackBufferHeight = globals->height;
  d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;

  depthformat = D3DFMT_UNKNOWN;

  for(threeshot = 0; threeshot < 5; threeshot ++)
  {
    switch(threeshot)
    {
      case 0:
        tryformat = D3DFMT_D24S8;
        break;
      case 1:
        tryformat = D3DFMT_D15S1;
        break;
      case 2:
        tryformat = D3DFMT_D32;
        break;
      case 3:
        tryformat = D3DFMT_D24X8;
        break;
      case 4:
        tryformat = D3DFMT_D16;
        break;                
    }
    Error.lError = D3DObj->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, displayformat, displayformat, tryformat);
    if(Error.lError == 0)
    {
      depthformat = tryformat;
      break;
    }
  }

  if(depthformat == D3DFMT_UNKNOWN)
  {
      return _FUNKY_DEPTH_FORMAT;
  }

  d3dpp.EnableAutoDepthStencil = TRUE;
  d3dpp.AutoDepthStencilFormat = depthformat;

  if(globals->hardwaretl == TRUE)
  {
    Error.lError = D3DObj->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)globals->window,
                        D3DCREATE_HARDWARE_VERTEXPROCESSING,
                        &d3dpp, &Device );
  }
  else
  {
    Error.lError = D3DObj->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)globals->window,
                        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                        &d3dpp, &Device );
  }

  if(Error.lError != 0)
  {
    return _CREATING_3DDEVICE_FAILED;
  }

  // Set up the textures
  Device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
  Device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
  Device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

  Device->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
  Device->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
  Device->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

  Device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

  //Device->SetRenderState(D3DRS_COLORKEYENABLE, TRUE);
  Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  Device->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
  Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

  // Set miscellaneous render states
  Device->SetRenderState( D3DRS_DITHERENABLE,   TRUE );
  Device->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
  Device->SetRenderState( D3DRS_ZENABLE,        D3DZB_USEW  );
  Device->SetRenderState( D3DRS_ZFUNC,          D3DCMP_LESSEQUAL);
  Device->SetRenderState( D3DRS_AMBIENT,        0x00909090 );
  Device->SetRenderState( D3DRS_COLORVERTEX,    TRUE);
  Device->SetRenderState( D3DRS_LIGHTING,       TRUE );
  Device->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE , D3DMCS_MATERIAL);

  if(globals->wireframe == TRUE)
  {
    Device->SetRenderState( D3DRS_FILLMODE,        D3DFILL_WIREFRAME );
  }

  //Device->SetRenderState( D3DRS_SHADEMODE,        D3DSHADE_FLAT  );

  float FogStart, FogEnd, Density;
  FogStart = 5000;
  FogEnd = 25000;
  Density = 0.00001f;
  Device->SetRenderState( D3DRS_FOGENABLE, TRUE);
  Device->SetRenderState( D3DRS_FOGCOLOR,  skycolor);
  Device->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
  //Device->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_EXP2);
  //Device->SetRenderState( D3DRS_FOGDENSITY, *(DWORD *)(&Density));
  Device->SetRenderState( D3DRS_FOGSTART, *((DWORD*) (&FogStart)));
  Device->SetRenderState( D3DRS_FOGEND, *((DWORD*) (&FogEnd)));

  HFONT font = CreateFont(15,5,0,0, FW_LIGHT, 0,0,0, ANSI_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN,
                            "Arial");

  D3DXCreateFont(Device, font , &defaultfont);

  DeleteObject(font);

  Device->SetVertexShader(_NormalVertexType);

  return 0;
}

PolygonListObject * D3DGraphics::CreatePolygonListObject() 
{ 
	D3DPolygonListObject * pObject = new D3DPolygonListObject(this);
	return pObject; 
}


MaterialObject * D3DGraphics::CreateMaterialObject()
{
	D3DMaterialObject * pObject = new D3DMaterialObject(this);
	return pObject;
}

Camera * D3DGraphics::CreateCamera()
{
	D3DCamera * pObject = new D3DCamera(this);
	return pObject;
}

Texture2D * D3DGraphics::CreateTexture2D()
{
	D3DTexture2D * pObject = new D3DTexture2D(this);
	return pObject;
}


inline void D3DGraphics::SetVertexBuffer(void* VertexBuffer, short vertexbytecount)
{
  Device->SetStreamSource( 0, (IDirect3DVertexBuffer8 *)VertexBuffer, vertexbytecount );
}

inline void D3DGraphics::SetTexture(short stage, _Texture* Texture)
{
  Device->SetTexture(stage, (IDirect3DBaseTexture8 *)Texture);
}

_Material* D3DGraphics::CreateMaterial(_Color* Diffuse, _Color* Ambient, _Color* Emissive)
{

  D3DMATERIAL8 *Mat;

  Mat = new D3DMATERIAL8;

  ZeroMemory(Mat, sizeof(D3DMATERIAL8));

  Mat->Diffuse = (D3DCOLORVALUE)*(D3DCOLORVALUE *)Diffuse;
  Mat->Ambient = (D3DCOLORVALUE)*(D3DCOLORVALUE *)Ambient;
  Mat->Emissive = (D3DCOLORVALUE)*(D3DCOLORVALUE *)Emissive;

  return (_Material*)Mat;
}

inline void D3DGraphics::SetMaterial(_Material *Mat)
{
  Device->SetMaterial((D3DMATERIAL8 *)Mat );
}

inline void D3DGraphics::SetIndexBuffer(_IndexBuffer *Buffer)
{
  Device->SetIndices((IDirect3DIndexBuffer8 *)Buffer, 0);
}

inline void D3DGraphics::DrawTriangleListBuffer(short vertexcount, short indexcount)
{
  Device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, vertexcount, 0, indexcount );
}

_VertexBuffer* D3DGraphics::CreateVertexBuffer(void *buffer, unsigned long bufferbytecount, unsigned long vertexbytecount)
{

  IDirect3DVertexBuffer8 *Buffer;

  Device->CreateVertexBuffer(bufferbytecount, 0, _NormalVertexType, D3DPOOL_DEFAULT, &Buffer);

  VOID* pVertices;

  Buffer->Lock( 0, bufferbytecount, (BYTE**)&pVertices, 0 );

  memcpy( pVertices, buffer, bufferbytecount );

  Buffer->Unlock();

  return Buffer;
}

_IndexBuffer* D3DGraphics::CreateIndexBuffer(void *buffer, unsigned long bufferbytecount, unsigned long indexbytecount)
{

  IDirect3DIndexBuffer8 *Buff;

  Device->CreateIndexBuffer(bufferbytecount, 0, D3DFMT_INDEX16 , D3DPOOL_DEFAULT, &Buff);

  VOID* pVertices;

  Buff->Lock( 0, bufferbytecount, (BYTE**)&pVertices, 0 );

  memcpy( pVertices, buffer, bufferbytecount );

  Buff->Unlock();

  return Buff;
}

_Texture* D3DGraphics::CreateTexture(char *Filename)
{
  LPDIRECT3DTEXTURE8 Tex = 0;

  D3DXCreateTextureFromFileEx(Device, Filename, D3DX_DEFAULT, D3DX_DEFAULT,
                            globals->miplevels, 0, GetTextureDepth(), D3DPOOL_MANAGED, D3DX_FILTER_BOX | D3DX_FILTER_DITHER ,
                            D3DX_FILTER_BOX | D3DX_FILTER_DITHER , colorkey, 0, 0, &Tex);

  return (_Texture *)Tex;

}

_Texture* D3DGraphics::CreateTexture(short Width, short Height, short MipMap)
{

  LPDIRECT3DTEXTURE8 Tex;

  short levels;

  if(MipMap==TRUE)
  {
    levels = globals->miplevels;
  }
  else
  {
    levels = 1;
  }

  Device->CreateTexture(Width, Height, levels, 0, GetTextureDepth(), D3DPOOL_MANAGED, &Tex);

  return (_Texture*)Tex;
}

void D3DGraphics::SetLight(short lightnum, _Light *Light)
{

  Device->SetLight( lightnum, (D3DLIGHT8 *)Light );

  return;
}

void D3DGraphics::LightEnabled(short lightnum, short lighton)
{

  Device->LightEnable( lightnum, lighton);

  return;
}

_Light* D3DGraphics::CreateDirectionalLight(_Vector* Direction, _Color* Color)
{

  D3DLIGHT8 *light;

  light = new D3DLIGHT8;

  memset(light, 0, sizeof(D3DLIGHT8));

  light->Type = D3DLIGHT_DIRECTIONAL;
  light->Diffuse = (D3DCOLORVALUE)*(D3DCOLORVALUE *)Color;
  light->Direction = (D3DXVECTOR3)*(D3DXVECTOR3 *)Direction;

  return light;
}

void D3DGraphics::DrawTextF(char *buffer, short bufferbytecount, _Rect rect, short LeftCenterRight, _Color*color )
{

  D3DCOLORVALUE *Color = (D3DCOLORVALUE *)color ;

  switch(LeftCenterRight)
  {
  case 0:
    defaultfont->DrawText(buffer, bufferbytecount, (RECT *)&rect, DT_WORDBREAK | DT_LEFT, D3DCOLOR_COLORVALUE(Color->r, Color->g, Color->b, Color->a) );
    break;
  case 1:
    defaultfont->DrawText(buffer, bufferbytecount, (RECT *)&rect, DT_WORDBREAK | DT_CENTER, D3DCOLOR_COLORVALUE(Color->r, Color->g, Color->b, Color->a) );
    break;
  case 2:
    defaultfont->DrawText(buffer, bufferbytecount, (RECT *)&rect, DT_WORDBREAK | DT_RIGHT, D3DCOLOR_COLORVALUE(Color->r, Color->g, Color->b, Color->a) );
    break;
  }
}

D3DFORMAT D3DGraphics::GetTextureDepth()
{

  D3DFORMAT ret;

  switch(globals->texturedepth)
  {
    case 32:
      ret = D3DFMT_R8G8B8;
      break;
    default:
      ret = D3DFMT_R5G6B5;
      break;
  }

  return ret;
}

inline void D3DGraphics::BeginScene()
{
  Device->BeginScene();
}

inline void D3DGraphics::ClearSurface(void)
{
  Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, skycolor, 1.0f, 0);
}

inline void D3DGraphics::EndScene()
{
  Device->EndScene();
}

inline void D3DGraphics::Flip()
{
  Device->Present( NULL, NULL, NULL, NULL );
}

inline void D3DGraphics::SetTransformWorld(_Matrix *matrix)
{
  Device->SetTransform( D3DTS_WORLD, (D3DMATRIX *)matrix);
}

inline void D3DGraphics::SetTransformCamera(_Matrix *matrix)
{
  Device->SetTransform( D3DTS_VIEW, (D3DMATRIX *)matrix);
}

inline void D3DGraphics::SetTransformFOV(_Matrix *matrix)
{
  Device->SetTransform( D3DTS_PROJECTION, (D3DMATRIX *)matrix);
}

inline _Rect D3DGraphics::GetCoordsRect()
{
  _Rect rect;

  rect.x1 = 0;
  rect.y1 = 0;
  rect.x2 = globals->width;
  rect.y2 = globals->height;

  return rect;
}

inline void D3DGraphics::SetForeground(short on)
{
  Device->SetRenderState( D3DRS_ZWRITEENABLE,        on);
}

bool D3DGraphics::CanFlip()
{
  return true;
}

void D3DGraphics::DeleteVertexBuffer(_VertexBuffer *buff)
{
  ((IDirect3DVertexBuffer8 *)buff)->Release();
};

void D3DGraphics::DeleteIndexBuffer(_IndexBuffer *buff)
{
  ((IDirect3DIndexBuffer8 *)buff)->Release();
};

void D3DGraphics::DeleteTexture(_Texture *tex)
{
  _DEL(tex);
}

void D3DGraphics::DeleteMaterial(_Material *mat)
{
  _DEL(mat);
}

void D3DGraphics::DeleteLight(_Light *light)
{
  _DEL(light);
}


// these routines orginate from OpenGL
void D3DGraphics::SetIdentity() {}
void D3DGraphics::SetModelViewMatrix() {}
void D3DGraphics::SetProjectionMatrix() {}
void D3DGraphics::SetTextureMatrix() {}

void D3DGraphics::Translate(float x, float y, float z) {}
void D3DGraphics::Rotate(float angle, float x, float y, float z) {}
void D3DGraphics::Scale(float x, float y, float z) {}
void D3DGraphics::LoadMatrix() {}
void D3DGraphics::MultiMatrix() {}
void D3DGraphics::PushMatrix() {}
void D3DGraphics::PopMatrix() {}

void D3DGraphics::DrawPoint(float x, float y, float z) {}
void D3DGraphics::DrawPoint(_GraphicsVector * v) {}
void D3DGraphics::DrawPointArray(int vertexCount, _GraphicsVector *v) {}
void D3DGraphics::DrawLine(float x1, float y1, float z1,
						   float x2, float y2, float z2) {}
void D3DGraphics::DrawRect(float x1, float y1, float x2, float y2) {}
void D3DGraphics::DrawTriangle(float x1, float y1, float z1,
					float x2, float y2, float z2, 
					float x3, float y3, float z3) {}
void D3DGraphics::DrawTriangle(_GraphicsVector * v1, _GraphicsVector * v2,
							   _GraphicsVector * v3) {}
void D3DGraphics::DrawTriangleArray(int vertexCount, _GraphicsVector * v) {}
void D3DGraphics::DrawTriangleFan(int vertexCount, _GraphicsVector *v) {}
void D3DGraphics::DrawTriangleStrip(int vertexCount, _GraphicsVector *v) {}
void D3DGraphics::DrawTriangleArray(int vertexCount, _ShadedVector *v) {}
void D3DGraphics::DrawTriangleFan(int vertexCount, _ShadedVector *v) {}
void D3DGraphics::DrawTriangleStrip(int vertexCount, _ShadedVector *v) {}
void D3DGraphics::DrawQuad() {}
void D3DGraphics::DrawString(const char * str) {}

_Viewport * D3DGraphics::CreateViewport() {return NULL;}
_Viewport * D3DGraphics::CreateViewport(float left, float top, float width, float height) {return NULL;}
void D3DGraphics::ModifyViewport(float left, float top, float width, float height) {}
void D3DGraphics::SetViewport(_Viewport *view) {}
void D3DGraphics::SetViewport(float left, float top, float width, float height) {}
void D3DGraphics::SetOrtho(double left, double right, double bottom,
						   double top, double nearPos, double farPos) {}
void D3DGraphics::DeleteViewport(_Viewport *view) {}
void D3DGraphics::SetPerspective(double fov, double aspect, double nearPos, double farPos) {}

void D3DGraphics::BeginPrimitive(PrimitiveTypes) {}
void D3DGraphics::EndPrimitive() {}
void D3DGraphics::Vertex(float x, float y, float z) {}
void D3DGraphics::Normal(float x, float y, float z) {}

void D3DGraphics::SetColor(float r, float g, float b, float a) {}
void D3DGraphics::SetColor(float r, float g, float b) {}

void D3DGraphics::Flush() {}
void D3DGraphics::SetClearColor(float r, float g, float b, float a) {}

void D3DGraphics::CullFrontFace() {}
void D3DGraphics::CullBackFace() {}
void D3DGraphics::SetFrontFace(PolygonEnum direct) {}
void D3DGraphics::SetTextureMode(int flag) {}
void D3DGraphics::Look(double x, double y, double z,
		              double tar_x, double tar_y, double tar_z,
					  double up_x, double up_y, double up_z) {}



#endif