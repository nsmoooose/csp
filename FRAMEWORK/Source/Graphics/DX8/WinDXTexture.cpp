#ifdef D3D

#include "WinDXTexture.h"

WinDXTexture::WinDXTexture(StandardGraphics *Graphics)
{

  p_Graphics = Graphics;
  p_Device = (IDirect3DDevice8 *)p_Graphics->GetDevice();
  p_Init = p_Graphics->GetInit();

	p_Tex = 0;

}

WinDXTexture::~WinDXTexture()
{
  Uninitialize();
}

short WinDXTexture::Initialize(const char *filename, int filetype)
{ 
  return Initialize(filename, p_Init->MipLevels, 0);
}

short WinDXTexture::Initialize(const char *filename)
{
  return Initialize(filename, p_Init->MipLevels, 0);
}

short WinDXTexture::Initialize(const char *filename, _MIPMAP Levels, StandardColor *ColorKey)
{

  // ------


  D3DCOLOR rgb;

  if(ColorKey != 0)
  {
    rgb = D3DCOLOR_RGBA(ColorKey->r, ColorKey->g, ColorKey->b, ColorKey->a);
  }
  else
  {
    rgb = 0;
  }

  HRESULT result;
    
  result = D3DXCreateTextureFromFileEx(p_Device, filename, 
							                         D3DX_DEFAULT, D3DX_DEFAULT,
                                       Levels, 0, D3DFMT_UNKNOWN, 
							                         D3DPOOL_MANAGED, 
                                       D3DX_DEFAULT,
                                       D3DX_DEFAULT,
                                       rgb, 0, 0, &p_Tex);
  return _A_OK;
}

inline void WinDXTexture::Apply()
{
  p_Device->SetTexture(0, p_Tex);
}

void WinDXTexture::Uninitialize()
{
  if(p_Tex != 0)
  {
    p_Tex->Release();
    p_Tex = 0;
  }
}

#endif
