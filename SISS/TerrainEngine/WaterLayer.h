///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   WaterLayer.h
//   declaration of class CWaterLayer
//
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	08/30/2001		file created - Stormbringer
//
///////////////////////////////////////////////////////////////////////////


#if !defined(WATERLAYER_H)
#define WATERLAYER_H

#include "GLTexture.h"

class CWaterLayer  
{
public:
	void Draw(float fPosX, float fPosY, float fDistance);
	bool LoadTextures();
	uint GetWaterQuadTexHandle();
	CWaterLayer();
	virtual ~CWaterLayer();

protected:
	uint m_hTextureWaterLayer;
	uint m_hTextureWaterQuad;
};

#endif // !defined(WATERLAYER_H)
