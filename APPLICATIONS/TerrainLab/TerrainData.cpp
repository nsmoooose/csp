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
	m_pHeightmap = NULL;
	m_pLoadedMesh = NULL;
	m_paNormals = NULL;
	m_paVertices = NULL;
	m_paVertexColoursSmoothEdges = NULL;
	m_paVertexColoursSharpEdges = NULL;
	m_paRelativeElevationGrid = NULL;
	m_paSlopeVertexColours = NULL;
	m_paRelElevVertexColours = NULL;
	m_paTextureCoords = NULL;
	m_paSlope = NULL;
	m_pEcosystems = NULL;
	m_paTextures = NULL;
	m_paCoverageTexture = NULL;
	m_pcColorArrayForCoverageFactorsSharp = NULL;
	m_pcColorArrayForCoverageFactorsSmooth = NULL;
	m_paCoverageFactorsSmooth = NULL;
	m_paCoverageFactorsSharp = NULL;
	m_paTextureForCoverageFactorsSmooth = NULL;
	m_paTextureForCoverageFactorsSharp = NULL;
	m_paAlphaMapTexture5 = NULL;
//	m_pLightmap = NULL;
	m_iIterationLevel = 0;
	m_fFactorD = 1.0;
	m_aFLP[0].fFactorD = 1.0;
	m_BenchRes.iFPSmax = 0;
	m_BenchRes.iFPSmin = 0;
	m_bRenderMode = RENDER_MODE_TRIANGLES;
	m_bShadingMode = SHADING_MODE_ECO_COLOR;
	m_bEcosystemMode = ECOSYSTEM_MODE_SMOOTH;
	m_faLightPos[0] = -3000.0f;
	m_faLightPos[1] = 5000.0f;
	m_faLightPos[2] = 5000.0f;
	m_fMinAlt = 0.0f;
}

CTerrainData::~CTerrainData()
{
	if(m_paCoverageFactorsSmooth)
	{
		for(int i=0;i<m_iNumEcosystems;i++)
		{
			if(m_paCoverageFactorsSmooth[i])
				delete[] m_paCoverageFactorsSmooth[i];
		}
		delete[] m_paCoverageFactorsSmooth;
	}

	if(m_paCoverageFactorsSharp)
	{
		for(int i=0;i<m_iNumEcosystems;i++)
		{
			if(m_paCoverageFactorsSharp[i])
				delete[] m_paCoverageFactorsSharp[i];
		}
		delete[] m_paCoverageFactorsSharp;
	}

	if(m_pHeightmap) delete [] m_pHeightmap;
	if(m_pLoadedMesh) delete []m_pLoadedMesh;
	if(m_paNormals)	delete [] m_paNormals;
	if(m_paVertices) delete m_paVertices;
	if(m_paVertexColoursSmoothEdges) delete [] m_paVertexColoursSmoothEdges; 
	if(m_paVertexColoursSharpEdges) delete [] m_paVertexColoursSharpEdges; 
	if(m_paRelElevVertexColours) delete [] m_paRelElevVertexColours;
	if(m_paSlopeVertexColours) delete [] m_paSlopeVertexColours;
	if(m_paSlope) delete [] m_paSlope;
	if(m_pcColorArrayForCoverageFactorsSharp) delete [] m_pcColorArrayForCoverageFactorsSharp;
	if(m_pcColorArrayForCoverageFactorsSmooth) delete [] m_pcColorArrayForCoverageFactorsSmooth;
	if(m_paTextureForCoverageFactorsSmooth) delete [] m_paTextureForCoverageFactorsSmooth;
	if(m_paTextureForCoverageFactorsSharp) delete [] m_paTextureForCoverageFactorsSharp;
	if(m_paAlphaMapTexture5) delete [] m_paAlphaMapTexture5;
//	if(m_pLightmap) delete [] m_pLightmap;
}

