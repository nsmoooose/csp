///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of CTextureZoom
//   derived from CTexture, delivers different sizes of the same texture
//   NOTE: class is only able to zoom texture with 8, 16 or 24 bits!
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//	8/5/2001	implemented basic functionality - zzed
//  8/5/2001	algos should run now - zzed
//  8/6/2001	corrected algo (made mistakes when magnifying small images)
//              - zzed
//  8/22/2001	added support for 8 and 16 bit textures - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(TEXTUREZOOM_H)
#define TEXTUREZOOM_H

#include "Texture.h"

#include <vector>


using namespace std;


class CTextureZoom : public CTexture
{
private:
	class ZoomedTex		
	{
	public:
		ZoomedTex(uchar *pData, uint uiWidth, uint uiHeight) { data = pData; width = uiWidth; height = uiHeight; }
		virtual ~ZoomedTex() { delete[] data; }
		uint	width;
		uint	height;
		uchar	*data;
	};
	typedef vector<ZoomedTex*>	VecZoomedTex;

public:
	CTextureZoom();
	virtual ~CTextureZoom();
	CTextureZoom(char* cFilename, bool bCache = true);
	CTextureZoom(ushort usWidth, ushort usHeight, uchar usPixeldepth, uchar *pData, bool bCache = true);

	uchar* GetZoomedTexture(uint uiWidth, uint uiHeight);
	void Destroy();
	uint GetSize();

protected:
	bool			m_bCache;

private:
	VecZoomedTex	m_vZoomedTex;
	uint			m_uiSize;

	void GetPixelColor(uchar ucRGB[], float fRelPos[2], float fRelSize[2], uchar ucNoBPP);
	uchar* CreateZoomedTexture(uint uiWidth, uint uiHeight);
};


#endif // TEXTUREZOOM_H
