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

#if !defined(TERRAINDATAMANAGER_H)
#define TERRAINDATAMANAGER_H


#include "TerrainData.h"
#include "TerrainDataLoader.h"
#include "TextureManager.h"


class CTerrainDataManager
{
public:
	CTerrainData			*m_pData;
	CTerrainDataLoader		*m_pDataLoader;
	//CTextureManager	*m_pTextureManager;

	CTerrainDataManager();
	~CTerrainDataManager();
	int LoadTerrainData(char *cFilename);
	void SetPosition(CVector vPos);
	CVector GetPosition();
	void SetRotation(float fRotHor, float fRotVer);
	void SetDrawingStyle(unsigned char ucDrawingStyle);
	void SetTextureWaterQuad(unsigned int hTextureWaterQuad);
};

#endif
