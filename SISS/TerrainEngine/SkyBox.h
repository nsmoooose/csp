///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   SkyBox.h
//   declaration of class CSkyBox
//
//	 using Code from the XTerminate Terrain Engine by Tim C. Schröder
//	 http://glvelocity.gamedev.net/
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	08/26/2001	original XTerminate code added and modified. - Stormbringer
//
//				to do: set the sun at the right place, below the terrain it
//				isn't of any use :-)					
//
///////////////////////////////////////////////////////////////////////////


#if !defined(SKYBOX_H)
#define SKYBOX_H

// Sun placement attributes
#define SUN_DISTANCE -10.0f

// Define this if there is no need to draw a ground
// #define NO_GROUND

// Array indexes for the six skybox textures
#define TEX_FRONT	0
#define TEX_BACK	1
#define TEX_UP		2
#define TEX_DOWN	3
#define TEX_LEFT	4
#define TEX_RIGHT	5

#include "GLTexture.h"

class CSkyBox  
{
public:
	void Draw(float fRotVer, float fRotHor, unsigned int iFOV, float fAspect);
	bool LoadTextures();
	void SetSunHeight(float fHeight) { m_fSunHeight = fHeight; };
	void SetSunSize(float fSize) { m_fSunSize = fSize; };
	CSkyBox();
	virtual ~CSkyBox();

protected:
	float m_fSunHeight;
	float m_fSunSize;
	uint m_hTextures[6];
	uint m_hSunBaseTexture;

	void Project2D(const float fU, const float fV, const float fPlane,
				   const unsigned int iFOV, float fAspect, float fVertex3DOut[3]);
};

#endif // !defined(SKYBOX_H)
