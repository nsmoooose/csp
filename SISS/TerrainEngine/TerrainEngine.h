///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   <file-description>
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(TERRAINENGINE_H)
#define TERRAINENGINE_H

#include "TerrainDataManager.h"
#include "MipMapRenderer.h"
#include "SkyBox.h"
#include "WaterLayer.h"

class CTerrainEngine 
{
public:
	CTerrainEngine();
	~CTerrainEngine();
	void LoadTerrain(char* cFilename);
	void SetupSky();
	void SetupWater();
	void SetPosition(CVector vPos);
	CVector GetPosition();
	void SetRotation(float fRotHor, float fRotVer);
	void SetDrawingStyle(unsigned char ucStyle);
	float GetClippingDistance();
	unsigned int GetNoRenderedTriangles();  // don't use - just temporarily available
	void Render();
private:
	CTerrainDataManager	*m_pDataManager;
	CRenderer			*m_pRenderer;
	CSkyBox				*m_pSkybox;
	CWaterLayer			*m_pWaterLayer;
};

#endif
