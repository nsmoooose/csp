///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   SkyBox.cpp
//   implementation of class CSkyBox
//
//	 using Code from the XTerminate Terrain Engine by Tim C. Schröder
//	 http://glvelocity.gamedev.net/
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	08/26/2001	original XTerminate code added and modified. - Stormbringer
//
//				to do: set the sun at the right place, below the terrain it
//				isn't of any use :-)					
//
///////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "SkyBox.h"

//#include "SISS/TerrainEngine/ResourceManager.h"
#include "ResourceManager.h"

//#define NO_GROUND

//////////////////////////////////////////////////////////////////////
// Construction / destrcution
//////////////////////////////////////////////////////////////////////

CSkyBox::CSkyBox()
{
	m_fSunHeight = 0.1f;
	m_fSunSize   = 0.35f;
}

CSkyBox::~CSkyBox()
{

}

bool CSkyBox::LoadTextures()
{
	////////////////////////////////////////////////////////////////////////
	// Load the six textures of the skybox
	////////////////////////////////////////////////////////////////////////

	CGLTexture *pTexture;

	printf("\nLoading sky textures...");

	// Load front texture
	ResManager.InsertResource(&m_hTextures[TEX_FRONT], new CGLTexture("../../SISS/TerrainData/front.bmp", GL_RGB));
	pTexture = static_cast<CGLTexture*>(ResManager.GetResource(m_hTextures[TEX_FRONT]));
	ResManager.Lock(m_hTextures[TEX_FRONT]);
	m_hTextures[TEX_FRONT] = pTexture->GetGLHandle();

	// Load back texture
	ResManager.InsertResource(&m_hTextures[TEX_BACK], new CGLTexture("../../SISS/TerrainData/back.bmp", GL_RGB));
	pTexture = static_cast<CGLTexture*>(ResManager.GetResource(m_hTextures[TEX_BACK]));
	ResManager.Lock(m_hTextures[TEX_BACK]);
	m_hTextures[TEX_BACK] = pTexture->GetGLHandle();

	// Load up texture
	ResManager.InsertResource(&m_hTextures[TEX_UP], new CGLTexture("../../SISS/TerrainData/up.bmp", GL_RGB));
	pTexture = static_cast<CGLTexture*>(ResManager.GetResource(m_hTextures[TEX_UP]));
	ResManager.Lock(m_hTextures[TEX_UP]);
	m_hTextures[TEX_UP] = pTexture->GetGLHandle();

#ifndef NO_GROUND
	// Load down texture
	ResManager.InsertResource(&m_hTextures[TEX_DOWN], new CGLTexture("../../SISS/TerrainData/down.bmp", GL_RGB));
	pTexture = static_cast<CGLTexture*>(ResManager.GetResource(m_hTextures[TEX_DOWN]));
	ResManager.Lock(m_hTextures[TEX_DOWN]);
	m_hTextures[TEX_DOWN] = pTexture->GetGLHandle();

#endif

	// Load left texture
	ResManager.InsertResource(&m_hTextures[TEX_LEFT], new CGLTexture("../../SISS/TerrainData/left.bmp", GL_RGB));
	pTexture = static_cast<CGLTexture*>(ResManager.GetResource(m_hTextures[TEX_LEFT]));
	ResManager.Lock(m_hTextures[TEX_LEFT]);
	m_hTextures[TEX_LEFT] = pTexture->GetGLHandle();

	// Load right texture
	ResManager.InsertResource(&m_hTextures[TEX_RIGHT], new CGLTexture("../../SISS/TerrainData/right.bmp", GL_RGB));
	pTexture = static_cast<CGLTexture*>(ResManager.GetResource(m_hTextures[TEX_RIGHT]));
	ResManager.Lock(m_hTextures[TEX_RIGHT]);
	m_hTextures[TEX_RIGHT] = pTexture->GetGLHandle();

	// Load sun base texture
	ResManager.InsertResource(&m_hSunBaseTexture, new CGLTexture("../../SISS/TerrainData/sunbase.tga", GL_RGB));
	pTexture = static_cast<CGLTexture*>(ResManager.GetResource(m_hSunBaseTexture));
	ResManager.Lock(m_hSunBaseTexture);
	m_hSunBaseTexture = pTexture->GetGLHandle();


	printf("finished\n");

	return true;
}

void CSkyBox::Draw(float fRotVer, float fRotHor, unsigned int iFOV, float fAspect)
{
	////////////////////////////////////////////////////////////////////////
	// Draw the skybox
	////////////////////////////////////////////////////////////////////////

	static int iDisplayList = -1;
	float fLowerLeft[3];
	float fLowerRight[3];
	float fUpperLeft[3];
	float fUpperRight[3];

	glPushMatrix();

	// Rotate the skybox to fit the players view
	glRotatef(fRotVer , -1, 0, 0);
	glRotatef(fRotHor, 0, 0, -1);

	// Has the display list already been generated ?
	if (iDisplayList != -1)
	{
		// Just call the list and exit
		glCallList(iDisplayList);
		glPopMatrix();
		return;
	}

	// This is the first time we render the skybox. Create a display
	// list and render the scene into the list while passing it to OpenGL.
	iDisplayList = glGenLists(1);
	glNewList(iDisplayList, GL_COMPILE_AND_EXECUTE);

	glPushAttrib(GL_FOG_BIT | GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT |
		         GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT);

	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	
	// Back
	glBindTexture(GL_TEXTURE_2D, m_hTextures[TEX_BACK]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(10.0f, 10.0f, -10.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(10.0f, 10.0f, 10.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10.0f, 10.0f, 10.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10.0f, 10.0f, -10.0f);
	glEnd();

	// Front
	glBindTexture(GL_TEXTURE_2D, m_hTextures[TEX_FRONT]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-10.0f, -10.0f, -10.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-10.0f, -10.0f, 10.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(10.0f, -10.0f, 10.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(10.0f, -10.0f, -10.0f);
	glEnd();

	// Up
	glBindTexture(GL_TEXTURE_2D, m_hTextures[TEX_UP]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(10.0f, 10.0f, 10.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(10.0f, -10.0f, 10.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10.0f, -10.0f, 10.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10.0f, 10.0f, 10.0f);
	glEnd();

	// Down
#ifndef NO_GROUND
	glBindTexture(GL_TEXTURE_2D, m_hTextures[TEX_DOWN]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(10.0f, -10.0f, -10.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(10.0f, 10.0f, -10.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10.0f, 10.0f, -10.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10.0f, -10.0f, -10.0f);
	glEnd();

#endif

	// Right
	glBindTexture(GL_TEXTURE_2D, m_hTextures[TEX_RIGHT]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(10.0f, -10.0f, -10.0f); 
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(10.0f, -10.0f, 10.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(10.0f, 10.0f, 10.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(10.0f, 10.0f, -10.0f);
	glEnd();

	// Left
	glBindTexture(GL_TEXTURE_2D, m_hTextures[TEX_LEFT]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBegin(GL_QUADS);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-10.0f, 10.0f, -10.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-10.0f, 10.0f, 10.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10.0f, -10.0f, 10.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10.0f, -10.0f, -10.0f);
	glEnd();

	// Project the sun's quad into a 2D plane that is centered in the screen. Size depends on
	// m_fSunSize. fAspect is used to scale the to Project2D passed coordinates so that
	// the crosshair is always rectangular, regardless of the screen's aspect.
	Project2D(-m_fSunSize, -m_fSunSize * fAspect + m_fSunHeight, SUN_DISTANCE, iFOV, fAspect, fLowerLeft);
	Project2D(m_fSunSize,  -m_fSunSize * fAspect + m_fSunHeight, SUN_DISTANCE, iFOV, fAspect, fLowerRight);
	Project2D(-m_fSunSize, m_fSunSize  * fAspect + m_fSunHeight, SUN_DISTANCE, iFOV, fAspect, fUpperLeft);
	Project2D(m_fSunSize,  m_fSunSize  * fAspect + m_fSunHeight, SUN_DISTANCE, iFOV, fAspect, fUpperRight);

	// Transparency
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Sun base texture, transparent
	glBindTexture(GL_TEXTURE_2D, m_hSunBaseTexture);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

	// Draw sun quad
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3fv(fLowerLeft);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3fv(fLowerRight);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3fv(fUpperRight);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3fv(fUpperLeft);
	glEnd();	

	glPopAttrib();

	glEndList();

	glPopMatrix();
}

void CSkyBox::Project2D(const float fU, const float fV, const float fPlane,
						const unsigned int iFOV, float fAspect, float fVertex3DOut[3])
{
	////////////////////////////////////////////////////////////////////////
	// Project a point in 2D space. The point in 2D space is normalized. 
	// Lower-left corner is -1,-1 and upper-right is +1,+1
	////////////////////////////////////////////////////////////////////////

	fVertex3DOut[2]= fPlane;
	fVertex3DOut[1] = - tanf(iFOV * 0.0087266f) * fVertex3DOut[2];
	fVertex3DOut[0]= fVertex3DOut[1] * fAspect * fU;
	fVertex3DOut[1] *= fV;
}

