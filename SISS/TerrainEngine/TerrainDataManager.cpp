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

#include "TerrainDataManager.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "SCNDataLoader.h"
#include "Config.h"


CTerrainDataManager::CTerrainDataManager()
{
	m_pData = new CTerrainData();
	m_pDataLoader = 0;
//	m_pTextureManager = 0;
}

CTerrainDataManager::~CTerrainDataManager()
{
	delete m_pData;
	if (m_pDataLoader) delete m_pDataLoader;
//	if (m_pTextureManager) delete m_pTextureManager;
}

int CTerrainDataManager::LoadTerrainData(char *cFilename)
{
	char cFileExtension[4];

	printf("CTerrainDataManager::LoadTerrainData - %s\n", cFilename);

	// get the extension of the filename and convert it to lower case
	strcpy(cFileExtension, cFilename+strlen(cFilename)-3);
#ifdef WIN32
	strlwr(cFileExtension);
#endif
	if (strcmp(cFileExtension, "cfg")==0)
	{
		// config file found!
		Config.ReadConfigFile(cFilename);

		CSCNDataLoader* pSCNLoader = new CSCNDataLoader();
		m_pDataLoader = reinterpret_cast<CTerrainDataLoader*>(pSCNLoader);
	} else if (strcmp(cFileExtension, "raw")==0)
	{
		// simple RAW file found ... have to guess whether 8 or 16 bit are used
		//CRAWDataLoader* pRAWLoader = new CRAWDataLoader();
		//m_pDataLoader = reinterpret_cast<CTerrainDataLoader*>(pRAWLoader);
		assert(0 && "RAW-filetype not supported anymore!");
	} else
	{
		printf("unknown file format given at command line!");
		exit(-1);
	}
	m_pData->m_fDeltaZ = Config.GetFloat("m_fDeltaZ");

	int iRes = m_pDataLoader->LoadData(m_pData, cFilename);
	//m_pTextureManager = new CTextureManager(m_pData, m_pDataLoader);

	return iRes;
}

void CTerrainDataManager::SetPosition(CVector vPos)
{
	m_pData->m_vCameraPos = vPos;
}

CVector CTerrainDataManager::GetPosition()
{
	return m_pData->m_vCameraPos;
}

void CTerrainDataManager::SetRotation(float fRotHor, float fRotVer)
{
	m_pData->m_fCameraRotHor = fRotHor;
	m_pData->m_fCameraRotVer = fRotVer;
}

void CTerrainDataManager::SetDrawingStyle(unsigned char ucDrawingStyle)
{
	//m_pData->m_ucDrawingStyle = ucDrawingStyle;
	if (m_pData->m_pQuadRenderer) delete m_pData->m_pQuadRenderer;
	m_pData->m_ucDrawingStyle = ucDrawingStyle;
	switch (ucDrawingStyle)
	{
		case TE_DS_TEXTURED:
			m_pData->m_pQuadRenderer = reinterpret_cast<CQuad::CQuadRenderer*>(new CQuad::CQuadMTexRenderer());
			break;
		case TE_DS_SMOOTH:
			m_pData->m_pQuadRenderer = reinterpret_cast<CQuad::CQuadRenderer*>(new CQuad::CQuadSmoothRenderer());
			break;
		case TE_DS_WIREFRAME:
		default:
			m_pData->m_pQuadRenderer = reinterpret_cast<CQuad::CQuadRenderer*>(new CQuad::CQuadWireframeRenderer());
	}
}

void CTerrainDataManager::SetTextureWaterQuad(unsigned int hTextureWaterQuad)
{
	m_pData->m_hTextureWaterQuad = hTextureWaterQuad;
}
