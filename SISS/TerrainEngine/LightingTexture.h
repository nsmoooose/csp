///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of CLightingTexture
//   calculates and implements a quad's lighting texture
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//	8/14/2001	created file - zzed
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(LIGHTINGTEXTURE_H)
#define LIGHTINGTEXTURE_H

#pragma warning( disable : 4786 )	// disable warning about truncated characters in debug-info

#include "TextureZoom.h"


class CQuad;

class CLightingTexture : public CTextureZoom
{
public:
	CLightingTexture(CQuad *pQuad);
	virtual bool Create();
private:
	CQuad	*m_pQuad;

	void CalcLightingTexture();
	void RectNormal(float afNormal[3], uint uiXPos, uint uiYPos);
	void TriangleNormal(float afNormal[3], float afVec1[3], float afVec2[3], float afVec3[3]);
};


#endif // LIGHTINGTEXTURE_H
