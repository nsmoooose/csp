#ifdef D3D

#include "WinDXViewport.h"

WinDXViewport::WinDXViewport()
{
  p_Viewport = 0;
  p_ZBuffer = 0;
  p_Graphics = 0;
  p_Device = 0;
  p_Init = 0;
  p_Font = 0;

}

WinDXViewport::~WinDXViewport()
{
  Uninitialize();
}

short WinDXViewport::Initialize(short left, short bottom, unsigned short Width, unsigned short Height)
{

  return 0;
}


short WinDXViewport::Initialize(StandardGraphics *Graphics,
                                unsigned short Width, unsigned short Height)
{

  p_Graphics = Graphics;

  p_Device = (IDirect3DDevice8 *)p_Graphics->GetDevice();
  p_Init = p_Graphics->GetInit();
  p_Font = (ID3DXFont *)p_Graphics->GetFont();

  // -----

  p_usWidth = Width;
  p_usHeight = Height;

  HRESULT result;

  D3DFORMAT *format;
  D3DFORMAT *format2;

  format = (D3DFORMAT *)p_Graphics->GetSurfaceFormat();
  format2 = (D3DFORMAT *)p_Graphics->GetZSurfaceFormat();

  result = p_Device->CreateTexture(Width,Height, 
                                  1, D3DUSAGE_RENDERTARGET,
                                  *format, D3DPOOL_DEFAULT,
                                  &p_Viewport);

  result = p_Device->CreateDepthStencilSurface(Width,Height, *format2,
                                D3DMULTISAMPLE_NONE, &p_ZBuffer);

  p_Viewport->AddRef();
  p_ZBuffer->AddRef();

  return 0;
}

void WinDXViewport::Uninitialize()
{
  if(p_Viewport != 0)
  {
    p_Viewport->Release();
    p_Viewport->Release();
    p_Viewport = 0;
  }
  if(p_ZBuffer != 0)
  {
    p_ZBuffer->Release();
    p_ZBuffer->Release();
    p_ZBuffer = 0;
  }

  return;
}

void WinDXViewport::SetViewport()
{

  IDirect3DSurface8 *surface;

  p_Viewport->GetSurfaceLevel(0, &surface);

  p_Device->SetRenderTarget(surface, p_ZBuffer);

  return;
}

void* WinDXViewport::GetSurface()
{
  return (void *)p_Viewport;
}

void WinDXViewport::BeginScene()
{
  p_Device->BeginScene();
}

void WinDXViewport::EndScene()
{
  p_Device->EndScene();
}

void WinDXViewport::Flip()
{
  p_Device->Present( NULL, NULL, NULL, NULL );
}

void WinDXViewport::Clear()
{
  p_Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0,0,128,0), 1.0f, 0);
}

void WinDXViewport::DrawTextF(char *buffer, _Rect rect, short LeftCenterRight, StandardColor*color )
{

  D3DCOLOR ColorRGBA;
  
  ColorRGBA = D3DCOLOR_RGBA(color->r, color->g, color->b, color->a);

  RECT rect2;

  rect2.top = (long)(rect.y1 * p_usHeight);
  rect2.left = (long)(rect.x1 * p_usWidth);
  rect2.right = (long)(rect.x2 * p_usWidth);
  rect2.bottom = (long)(rect.y2 * p_usHeight);

  switch(LeftCenterRight)
  {
  case 0:
    p_Font->DrawText(buffer, strlen(buffer), &rect2, DT_WORDBREAK | DT_LEFT, ColorRGBA );
    break;
  case 1:
    p_Font->DrawText(buffer, strlen(buffer), &rect2, DT_WORDBREAK | DT_CENTER, ColorRGBA);
    break;
  case 2:
    p_Font->DrawText(buffer, strlen(buffer), &rect2, DT_WORDBREAK | DT_RIGHT, ColorRGBA);
    break;
  }
}

StandardViewport* WinDXViewport::CreateViewport()
{

  StandardViewport *viewport;

  viewport = new WinDXViewport();

  return viewport;
}

#endif
