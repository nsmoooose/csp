///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   definition of CTextureZoom
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

#include "TextureZoom.h"

#include <assert.h>


CTextureZoom::CTextureZoom()
{
}

CTextureZoom::CTextureZoom(char* cFilename, bool bCache) 
	: CTexture(cFilename)
{
	m_bCache = bCache;
}

CTextureZoom::CTextureZoom(ushort usWidth, ushort usHeight, uchar usPixeldepth, uchar *pData, bool bCache) 
	: CTexture(usWidth, usHeight, usPixeldepth, pData)
{
	m_bCache = bCache;
}

CTextureZoom::~CTextureZoom()
{
	Destroy();
}

void CTextureZoom::Destroy()
{
	m_vZoomedTex.clear();
	m_uiSize = 0;
	if (m_pData) delete[] m_pData;
}

uint CTextureZoom::GetSize()
{
	return m_uiSize+CTexture::GetSize();
}

uchar* CTextureZoom::GetZoomedTexture(uint uiWidth, uint uiHeight)
{
	// texture already loaded?
	if (m_pData==0) 
		if (!CTexture::Create()) return 0;	// ups, error while loading texture!
	// is it the original size?
	if (m_usWidth==uiWidth && m_usHeight==uiHeight) 
		return m_pData;
	if (m_bCache)
	{
		// search if we already have calculated an images with this width and height
		VecZoomedTex::const_iterator iter;
		for (iter = m_vZoomedTex.begin(); iter!=m_vZoomedTex.end(); ++iter)
		{
			if ((*iter)->width==uiWidth && (*iter)->height==uiHeight)
			{
				// yeah, there it is
				return (*iter)->data;
			}
		}
	}
	// ok, texture with desired size not found in buffer, we must create it then
	uchar *ctex = CreateZoomedTexture(uiWidth, uiHeight);
	if (m_bCache)
	{
		ZoomedTex *tex = new ZoomedTex(ctex, uiWidth, uiHeight);
		m_vZoomedTex.insert(m_vZoomedTex.begin(), tex);
		m_uiSize += uiWidth*uiHeight*m_ucPixeldepth/8;
	}

	return ctex;
}

uchar* CTextureZoom::CreateZoomedTexture(uint uiWidth, uint uiHeight)
{
	assert((m_ucPixeldepth==8 || m_ucPixeldepth==16 || m_ucPixeldepth==24) && "CTextureZoom only processes 8, 16, or 24 textures");

	uchar ucNoBPP = m_ucPixeldepth/8;   // no bytes per pixel
	uchar *pZoomedTex = new uchar[uiWidth*uiHeight*ucNoBPP];
	float fDSizing[2]    = { 1.0f/uiWidth, 1.0f/uiHeight };
	float fSectorSize[2] = { (fDSizing[0]<1.0f/m_usWidth ? 1.0f/m_usWidth : fDSizing[0]), 
						     (fDSizing[1]<1.0f/m_usHeight ? 1.0f/m_usHeight : fDSizing[1]) };
	//float fSectorSize[2] = { 1.0f/m_usWidth, 1.0f/m_usHeight };

	for (uint y=0; y<uiHeight; ++y)
	{
		for (uint x=0; x<uiWidth; ++x)
		{
			float fSectorPos[2] = { fDSizing[0]*x-fDSizing[0]/2, fDSizing[1]*y-fDSizing[1]/2 };
			GetPixelColor(&pZoomedTex[(y*uiWidth+x)*ucNoBPP], fSectorPos, fSectorSize, ucNoBPP);
		}
	}
	return pZoomedTex;
}

void CTextureZoom::GetPixelColor(uchar ucRGB[], float fRelPos[2], float fRelSize[2], uchar ucNoBPP)
{
	uint uiSrcPos[2]	= { (fRelPos[0]<0 ? fRelPos[0]=0 : fRelPos[0])*m_usWidth, (fRelPos[1]<0 ? fRelPos[1]=0 : fRelPos[1])*m_usHeight };
	uint uiSrcSize[2]	= { (fRelPos[0]+fRelSize[0])*m_usWidth-uiSrcPos[0]+1, (fRelPos[1]+fRelSize[1])*m_usWidth-uiSrcPos[1]+1 };

	float fRelSizing[2]	= { 1.0f/m_usWidth, 1.0f/m_usHeight };
	float fWeight		= 0.0f;
	float fRGB[3]		= { 0.0f, 0.0f, 0.0f };
	
	// iterate through all pixels enclosed by our section
	for (uint sy = uiSrcPos[1]; sy<uiSrcPos[1]+uiSrcSize[1] && sy<m_usHeight; ++sy)
	{
		for (uint sx= uiSrcPos[0]; sx<uiSrcPos[0]+uiSrcSize[0] && sx<m_usWidth; ++sx)
		{
			// pointer to values in texture-array
			uchar *pData = &m_pData[(sy*m_usWidth+sx)*ucNoBPP];
			// is pixel on section's border?
			if (sx==uiSrcPos[0] || sx==uiSrcPos[0]+uiSrcSize[0]-1 || sy==uiSrcPos[1] || sy==uiSrcPos[1]+uiSrcSize[1]-1)
			{	// yes
				// relative coordinates of pixel's upper left corner
				float fRelPixPos[2] = { (float)sx/m_usWidth, (float)sy/m_usHeight };
				float fTempWeight = 1.0f;
				// pixel on left border?
				if (sx==uiSrcPos[0]) fTempWeight *= fRelSizing[0]-(fRelPos[0]-fRelPixPos[0]);
				// right border
				if (sx==uiSrcPos[0]+uiSrcSize[0]-1) fTempWeight *= fRelPos[0]+fRelSize[0]-fRelPixPos[0];
				// upper border
				if (sy==uiSrcPos[1]) fTempWeight *= fRelSizing[1]-(fRelPos[1]-fRelPixPos[1]);
				// lower border
				if (sy==uiSrcPos[1]+uiSrcSize[1]-1) fTempWeight *= fRelPos[1]+fRelSize[1]-fRelPixPos[1];
				
				for (uint i=0; i<ucNoBPP; ++i)
					fRGB[i] += (float)(pData[i])*fTempWeight;
				fWeight += fTempWeight;
			} else
			{	// pixel lies completely inside section
				for (uint i=0; i<ucNoBPP; ++i)
					fRGB[i] += (float)(pData[i]);
				fWeight += 1;
			}
		}
	}
	// ok, all color-values are inside fRGB, now we calculate the "average" and return it
	for (uint i=0; i<ucNoBPP; ++i)
		ucRGB[i] = (uchar)(fRGB[i]/fWeight);
}
