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

#include "TerrainEngine.h"
#include "glextensions.h"

#include <assert.h>
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

CTerrainEngine::CTerrainEngine()
{
	m_pRenderer	= 0;
	m_pDataManager		= 0;
	
	OGLText.Init();

	if (glext::EnableMultiTexturing())
	{
		printf("Using multitexturing\n");
	} else
	{
		printf("Sorry, no multitexturing support detected. aborting ...");
		exit(-1);
	}
}

CTerrainEngine::~CTerrainEngine()
{
	if (m_pRenderer) delete m_pRenderer;
	if (m_pDataManager) delete m_pDataManager;
}

void CTerrainEngine::SetPosition(CVector vPos)
{
	assert(m_pDataManager && "terrain not loaded yet!");
	m_pDataManager->SetPosition(vPos);
}

CVector CTerrainEngine::GetPosition()
{
	assert(m_pDataManager && "terrain not loaded yet");
	return m_pDataManager->GetPosition();
}

void CTerrainEngine::SetRotation(float fRotHor, float fRotVer)
{
	assert(m_pRenderer && "terrain not loaded yet!");
	m_pDataManager->SetRotation(fRotHor, fRotVer);
}

void CTerrainEngine::SetDrawingStyle(unsigned char ucStyle)
{
	m_pDataManager->SetDrawingStyle(ucStyle);
}

float CTerrainEngine::GetClippingDistance()
{
	assert(m_pRenderer && "terrain not loaded yet!");
	return m_pRenderer->GetClippingDistance();
}

// don't use - just temporarily available!
unsigned int CTerrainEngine::GetNoRenderedTriangles()
{
	assert(m_pRenderer && "terrain not loaded yet!");
	return m_pRenderer->m_iTriangleCount;
}

void CTerrainEngine::LoadTerrain(char* cFilename)
{
	m_pDataManager = new CTerrainDataManager();
	m_pDataManager->LoadTerrainData(cFilename);
	m_pRenderer = new CRenderer(m_pDataManager->m_pData);
}

void CTerrainEngine::SetupSky()
{
	m_pSkybox = new CSkyBox();
	m_pSkybox->LoadTextures();
}

void CTerrainEngine::SetupWater()
{
	m_pWaterLayer = new CWaterLayer();
	m_pWaterLayer->LoadTextures();
	m_pDataManager->SetTextureWaterQuad(m_pWaterLayer->GetWaterQuadTexHandle());
}

void CTerrainEngine::Render()
{
	m_pSkybox->Draw(m_pDataManager->m_pData->m_fCameraRotVer, m_pDataManager->m_pData->m_fCameraRotHor, 60, 1.25);
	glPushMatrix();
	assert(m_pRenderer && "terrain not loaded yet!");
	glRotatef(m_pDataManager->m_pData->m_fCameraRotVer , -1, 0, 0);
	glRotatef(m_pDataManager->m_pData->m_fCameraRotHor, 0, 0, -1);
	glTranslatef(-m_pDataManager->m_pData->m_vCameraPos.x, -m_pDataManager->m_pData->m_vCameraPos.y, 
		-m_pDataManager->m_pData->m_vCameraPos.z);
//	m_pWaterLayer->Draw(m_pDataManager->m_pData->m_vCameraPos.x, m_pDataManager->m_pData->m_vCameraPos.y,
//						m_pDataManager->m_pData->m_fMaxQuadClippingDist/2);
	m_pRenderer->Render();
	glPopMatrix();
}

