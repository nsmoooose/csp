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

	void UpdateSettings();

	float GetDeltaXYZ()				{ return m_fDeltaXYZ; }
	float GetDeltaZ()				{ return m_fDeltaZ; }
	int	  GetQuadLength()			{ return m_iQuadLength; }
	float GetDetailLevel()			{ return m_fDetailLevel; }
	float GetRenderDistance()		{ return m_fRenderDistance; }
	float GetMinRenderDistance()	{ return m_fMinRenderDistance; }
	float GetMaxRenderDistance()	{ return m_fMaxRenderDistance; }

	void SetDeltaXYZ( float value )				{ m_fDeltaXYZ = value; }
	void SetDeltaZ( float value )				{ m_fDeltaZ = value; }
	void SetQuadLength( int value )				{ m_iQuadLength = value; }
	void SetDetailLevel( float value )			{ m_fDetailLevel = value; }
	void SetRenderDistance( float value )		{ m_fRenderDistance = value; }
	void SetMinRenderDistance( float value )	{ m_fMinRenderDistance = value; }
	void SetMaxRenderDistance( float value )	{ m_fMaxRenderDistance = value; }


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
