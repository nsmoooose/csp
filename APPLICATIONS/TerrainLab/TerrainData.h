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
	float  *m_pHeightmap;							// pointer will be set by WinMain
	float  *m_paNormals;
	float  *m_paVertices;
	float  *m_paVertexColours;
	float	m_fDeltaZ;
	BOOL	m_bStartupAction;
	BOOL	m_bMapType;
	BOOL	m_bActivateVSync;
	BOOL	m_bRenderMode;
	float	m_fFactorD;
	float	m_fIterations;
	int		m_iIterationLevel;
	float  *m_fDefaultMesh;
	float  *m_pLoadedMesh;
	float	m_faLightPos[3];
	FRACT_LEVEL_PARAMETER	m_aFLP[20];				// 20 levels should be enough
	BENCH_RESULT			m_BenchRes;				// for the max/min/average framerate
};


#endif

