#ifndef __D3DTEXTURE2D_H_
#define __D3DTEXTURE2D_H_

#include "graphics/typesgraphics.h"

class D3DTexture2D : public Texture2D
{
public:
	D3DTexture2D(StandardGraphics * pDisplay);
	virtual ~D3DTexture2D();
	virtual void Create(const char * filename, int filetype);


	virtual void Destroy();
	virtual void Apply();

protected:
	LPDIRECT3DTEXTURE8 m_Tex;
	D3DGraphics * m_pD3Display;
};








#endif