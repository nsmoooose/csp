///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Header of Class CTerrainData
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	09/25/2001	file created	-	Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(TERRAINDATA_H)
#define TERRAINDATA_H

#include "BaseCode.h"
#include "Texture.h"

typedef struct
{
	float fFactorD;
} FRACT_LEVEL_PARAMETER;

typedef struct
{
	int iFPSnow;
	int iFPSaverage;
	int iFPSmax;
	int iFPSmin;
} BENCH_RESULT;

typedef struct
{
	float fRed;
	float fGreen;
	float fBlue;
} COLOR;

class CTerrainData
{
public:
	CTerrainData();
	~CTerrainData();

	char	m_sMeshFileName[MAX_STR_LENGTH];
	int		m_iMeshWidth;
	int		m_iMeshWidthOld;
	int		m_iMeshGridDistance;
	int		m_iMeshGridDistanceBasis;
	int		m_iNumEcosystems;
	float  *m_pHeightmap;							// pointer will be set by WinMain
	float  *m_paNormals;
	float  *m_paVertices;
	float  *m_paVertexColoursSmoothEdges;
	float  *m_paVertexColoursSharpEdges;
	float  *m_paRelativeElevationGrid;
	float  *m_paRelElevVertexColours;
	float  *m_paSlopeVertexColours;
	float  *m_paSlope;
	float  *m_paTextureCoords;
	float  **m_paCoverageFactorsSmooth;
	float  **m_paCoverageFactorsSharp;
	int	   *m_pEcosystems;
	float  *m_pcColorArrayForCoverageFactorsSharp;
	float  *m_pcColorArrayForCoverageFactorsSmooth;	
	float  *m_paTextureForCoverageFactorsSharp;
	float  *m_paTextureForCoverageFactorsSmooth;
	float  *m_paAlphaMapTexture5;
	float	m_fMaxAlt;
	float	m_fMinAlt;
	float	m_fDeltaZ;
	BOOL	m_bStartupAction;
	BOOL	m_bMapType;
	BOOL	m_bActivateVSync;
	BOOL	m_bRenderMode;
	BOOL	m_bShadingMode;
	BOOL	m_bEcosystemMode;
	float	m_fFactorD;
	float	m_fIterations;
	int		m_iIterationLevel;
	float  *m_fDefaultMesh;
	float  *m_pLoadedMesh;
	float	m_faLightPos[3];
	float	m_fScalingFactor;
	CTexture *m_paTextures;
	CTexture *m_paCoverageTexture;
//	CLightmap *m_pLightmap;
	FRACT_LEVEL_PARAMETER	m_aFLP[20];				// 20 levels should be enough
	BENCH_RESULT			m_BenchRes;				// for the max/min/average framerate
};


#endif

