///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of class CTerrainData
//   encapsulates all data for displaying the terrain (except textures)
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	06/14/2001	implemented CVector - zzed
//	09/03/2001  added water values - stormbringer
//	10/10/2001	moved quadlist to CRenderer
//
//
///////////////////////////////////////////////////////////////////////////

/**
 * Represents data for the terrain
 */

#if !defined(TERRAINDATA_H)
#define TERRAINDATA_H

#include "MipMapQuad.h"
#include "Vector.h"
#include "../../Test/TerrainTest/OGLText.h"
#include "ResourceManager.h"
#include "TextureZoom.h"

#include <stdio.h>


const unsigned char TE_DS_WIREFRAME	= 1;	// DrawingStyle for Terrainengine: wireframe-mode
const unsigned char TE_DS_SMOOTH	= 2;	// smooth-shading
const unsigned char TE_DS_TEXTURED	= 3;	// textured

const unsigned char TE_MT_SCN		= 1;
const unsigned char TE_MT_RAW		= 2;

class CTerrainData
{
public:
	CTerrainData();
	~CTerrainData();

	uint					m_uiTerrainQuadWidth;		// width of the whole terrain in quads
	uint					m_uiTerrainQuadHeight;		// height of the whole terrain in quads
	uint					m_uiTerrainPointWidth;		// width of the whole terrain in points 
	uint					m_uiTerrainPointHeight;		// height of the whole terrain in points
	uint					m_uiNoMipmaps;
	float					m_fDeltaZ;
	CVector					m_vCameraPos;
	float					m_fCameraRotHor;
	float					m_fCameraRotVer;
	uint					m_uiMaxNoTextures;
	float					m_fAverageHeight;
	uint					m_uiQuadWidth;
	float					m_fDetailLevel;
	float					m_fMinQuadClippingDist;	
	float					m_fMaxQuadClippingDist;
	float					m_fRatioQuadClippingDist;
	float					m_fQuadClippingDist;
	float					m_fMinHeight;
	float					m_fMaxHeight;
	uchar					m_ucDrawingStyle;
	CQuad::CQuadRenderer	*m_pQuadRenderer;
	CTextureZoom			*m_pLowTex;
	CTextureZoom			*m_pHighTex;
	uchar					m_ucTexQuadSpan;
	uint					m_hTextureWaterQuad;
	float					m_fWaterLevel;
	uint					m_uiSectorWidth;			// width of a sector (in quads!)
	FILE					*m_hTerrainFile;
	uint					m_uiFrameCount;				// frame-counter (needed for threads)
};

#endif 
