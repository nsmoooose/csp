///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Implementation of Class CTerrainData
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	09/25/2001	file created	-	Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#include "TerrainData.h"

CTerrainData::CTerrainData()
{
	m_pHeightmap = 0;
	m_pLoadedMesh = 0;
	m_paNormals = 0;
	m_paVertices = 0;
	m_paVertexColours = 0;
	m_iIterationLevel = 0;
	m_fFactorD = 1.0;
	m_aFLP[0].fFactorD = 1.0;
	m_BenchRes.iFPSmax = 0;
	m_BenchRes.iFPSmin = 0;
	m_bRenderMode = RENDER_MODE_TRIANGLES;
	m_faLightPos[0] = 6000.0f;
	m_faLightPos[1] = 6000.0f;
	m_faLightPos[2] = 8000.0f;
}

CTerrainData::~CTerrainData()
{
	if(m_pHeightmap) delete m_pHeightmap;
	if(m_pLoadedMesh) delete m_pLoadedMesh;
}

