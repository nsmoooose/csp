///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   Header of Class CFractal
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	09/25/2001	file created		-	Stormbringer
//	09/27/2001  smooth step added, roughness in DiamondSquare
//				removed				-	Stormbringer
//
///////////////////////////////////////////////////////////////////////////

#if !defined(FRACTAL_H)
#define FRACTAL_H

#include <math.h>
#include "BaseCode.h"								// Header File For NeHeGL
#include "TerrainData.h"
#include "Terrain.h"
#include "DlgStartup.h"

extern CTerrainData *m_pTerrainData;				// Global class containing all terrain parameters
extern CTerrain		*m_pTerrain;					// The main Terrain class
extern CDlgStartup	 m_cDlgStartup;

class CFractal
{
public:

	CFractal();
	~CFractal();
	
	void DiamondSquare(float fFactorD);
	void SmoothStep(float fSmoothFactor);

	float *m_pFractMap;
};

extern CFractal	Fractal;

#endif