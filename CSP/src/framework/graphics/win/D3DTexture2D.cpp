#ifdef D3D

#include "D3D.h"
#include "D3DTexture2D.h"


D3DTexture2D::D3DTexture2D(StandardGraphics * pDisplay) :
	Texture2D(pDisplay)
{
	m_Tex = 0;
	m_pD3Display = (D3DGraphics*)pDisplay;
}

D3DTexture2D::~D3DTexture2D()
{

}

void D3DTexture2D::Create(const char * filename, int filetype)
{
  D3DXCreateTextureFromFileEx(m_pD3Display->Device, filename, 
							D3DX_DEFAULT, D3DX_DEFAULT,
                            m_pD3Display->globals->miplevels, 0, m_pD3Display->GetTextureDepth(), 
							D3DPOOL_MANAGED, D3DX_FILTER_BOX | D3DX_FILTER_DITHER ,
                            D3DX_FILTER_BOX | D3DX_FILTER_DITHER , m_pD3Display->colorkey, 0, 0, &m_Tex);

}

void D3DTexture2D::Destroy()
{

}

void D3DTexture2D::Apply()
{
  m_pD3Display->Device->SetTexture(0, (IDirect3DBaseTexture8 *)m_Tex);

}

#endif