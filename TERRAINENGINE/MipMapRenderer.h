///////////////////////////////////////////////////////////////////////////
//
//   test of a terrain-engine based on GeoMipMapping
//
//   coded by zzed and Stormbringer
//   email:   zzed@gmx.li    storm_bringer@gmx.li 
//
//   7.2.2001
//
///////////////////////////////////////////////////////////////////////////

#if !defined(MIPMAPRENDERER_H)
#define MIPMAPRENDERER_H

#include "MipMapQuad.h"
#include "TerrainData.h"	
#include "Frustum.h"


class CMipMapRenderer  
{
public:
	float m_fDeltaZ;
	bool m_temp;
	void Render(float x, float y, float z, float rotx, float roty, bool bLines);
	CMipMapRenderer(CTerrainData *pTerrainData);
	virtual ~CMipMapRenderer();

private:
	void PreprocessTerrain();
	float m_fAverageTerrainHeight;
	float m_fRenderDistance;
	float m_fMinRenderDistance;
	float m_fMaxRenderDistance;
	float m_fDetailLevel;
	float m_fDeltaXYZ;
	int m_iQuadLength;
	int m_iQuadHeight;
	int m_iQuadWidth;
	CMipMapQuad **m_MipmapQuads;
	void InitQuads();
	CTerrainData *m_pTerrainData;
	CFrustum m_cFrustum;
};

#endif 
