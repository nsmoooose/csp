///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   WaterLayer.cpp
//   implementation of class CWaterLayer
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	08/30/2001	file created - Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "WaterLayer.h"
//#include "SISS/TerrainEngine/ResourceManager.h"
#include "ResourceManager.h"

//////////////////////////////////////////////////////////////////////
// Construction / destrcution
//////////////////////////////////////////////////////////////////////

CWaterLayer::CWaterLayer()
{
}

CWaterLayer::~CWaterLayer()
{

}

bool CWaterLayer::LoadTextures()
{
	////////////////////////////////////////////////////////////////////////
	// Load the texture of the water layer and of the water quad
	////////////////////////////////////////////////////////////////////////

	CGLTexture *pTexture;

	printf("\nLoading water textures...");

	// Load quad texture
	ResManager.InsertResource(&m_hTextureWaterQuad, new CGLTexture("../../SISS/TerrainData/f4water.bmp", GL_RGB));
	pTexture = static_cast<CGLTexture*>(ResManager.GetResource(m_hTextureWaterQuad));
	ResManager.Lock(m_hTextureWaterQuad);
	m_hTextureWaterQuad = pTexture->GetGLHandle(); 

	// there's no water layer texture yet

	printf("finished\n");

	return true;
}

void CWaterLayer::Draw(float fPosX, float fPosY, float fDistance)
{
	////////////////////////////////////////////////////////////////////////
	// Draw the water layer (never called at the moment)
	////////////////////////////////////////////////////////////////////////

	glPushMatrix();
	glTranslatef(fPosX, fPosY, 0.0f);
	glPushAttrib(GL_FOG_BIT | GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT |
		         GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_hTextureWaterLayer);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(fDistance, -fDistance, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(fDistance, fDistance, 0.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-fDistance, fDistance, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-fDistance, -fDistance, 0.0f);
	glEnd();

	glPopAttrib();
	glPopMatrix();
}

uint CWaterLayer::GetWaterQuadTexHandle()
{
	if(m_hTextureWaterQuad)
		return m_hTextureWaterQuad;
	else
		return false;
}

