#include "stdafx.h"
#include <windows.h>
#include <GL\gl.h>
#include <GL\glu.h>

#include "Log.h"
#include "EngineStatus.h"
#include "glTexFont.h"
#include "FrameCounter.h" 
#include "glcode.h"
#include "glTexFont.h"

CLog				Log("log.txt");											// The Status Log
CEngineStatus		EngineStatus;
CFrameCounter		FrameCounter;
CTerrainData		*m_pTerrainData;
CMipMapRenderer		*m_renderer;


//////////////////////////////////////////////////////////////////////////////
// Initalize The OpenGL System
//////////////////////////////////////////////////////////////////////////////
int InitGL(GLvoid)															// All Setup For OpenGL Goes Here
{																			
	// TODO: Modify This Section To Suite Your Needs For Your OpenGL Scene
	//////////////////////////////////////////////////////////////////////////////
	glShadeModel(GL_SMOOTH);												// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.2f, 0.5f);									// Black Background
	glClearDepth(1.0f);														// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);												// Enables Depth Testing
//	glEnable(GL_TEXTURE_2D);
//	glDepthFunc(GL_LEQUAL);													// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);						// Really Nice Perspective Calculations
	
	glMatrixMode(GL_MODELVIEW);

	fontLoad("font.tga");

	return TRUE;															// Initialization Went OK
}

void KillGL()
{
	if(m_pTerrainData)														// clean up first, important on re-initialisation
		delete(m_pTerrainData);
//	if(m_renderer)
//		delete(m_renderer);
}

int LoadScene()
{
//	if(m_renderer)
//		delete(m_renderer);
	if(m_pTerrainData)														// clean up first, important on re-initialisation
		delete(m_pTerrainData);

	m_pTerrainData = new CTerrainData(EngineStatus.m_sMapFileName);
	m_renderer = new CMipMapRenderer(m_pTerrainData);
	EngineStatus.m_iShowStats = TRUE;
	return TRUE;
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)							// Resize And Initialize The GL Window
{																			
	glViewport(0, 0, width, height);
	EngineStatus.m_fAspectRatio = (float)width/height;
	EngineStatus.m_iViewportWidth = (int)width;
	EngineStatus.m_iViewportHeight = (int)height;
}

int DrawGLScene(GLvoid)														// Here's Where We Do All The Drawing
{																			
	// TODO: Modify This Section; This Is Where You Do All Of Your Drawing
	//////////////////////////////////////////////////////////////////////////////
	FrameCounter.StartFrame();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluPerspective(45, EngineStatus.m_fAspectRatio, 1, m_renderer->GetMaxRenderDistance());  
	gluLookAt(0, 0, 0, 0, 1, 0, 0, 0, 1);
	glRotatef(EngineStatus.m_roty, -1, 0, 0);
	glRotatef(EngineStatus.m_rotx, 0, 0, -1);
	glTranslatef(-EngineStatus.m_posx, -EngineStatus.m_posy, -EngineStatus.m_posz);
//	if(EngineStatus.m_bTextures)	
//		m_Texture.Use();
	/*if (eManager->m_bLines) 
		glBegin(GL_LINES);
	else 
		glBegin(GL_TRIANGLES);*/
	m_renderer->Render(EngineStatus.m_posx, EngineStatus.m_posy, EngineStatus.m_posz,
	   				   EngineStatus.m_rotx, EngineStatus.m_roty, EngineStatus.m_bLines);
	//glEnd();

	FrameCounter.EndFrame();

	if(EngineStatus.m_iShowStats)
	{
		fontDrawString(0, 32, "Current Speed %.1f", EngineStatus.m_fSpeed);
		fontDrawString(0, 48, "FPS: %d",FrameCounter.GetFrameCount()); 
		fontDrawString(0, 64, "X: %.2f, Y: %.2f, Z: %.2f", EngineStatus.m_posx, EngineStatus.m_posy, EngineStatus.m_posz);
		fontDrawString(0, 16, "Current Rotation Specs: x: %.2f, y: %.2f", EngineStatus.m_rotx, EngineStatus.m_roty);
	}

	glFlush();

	return TRUE;
}

void SetCamera()
{
	float vecxy = (float)cos(EngineStatus.m_roty/180*3.14);

	if (EngineStatus.m_rotx<0) EngineStatus.m_rotx += 360;
	else if (EngineStatus.m_rotx>360) EngineStatus.m_rotx -=360;
	if (EngineStatus.m_roty<0) EngineStatus.m_roty += 360;
	else if (EngineStatus.m_roty>360) EngineStatus.m_roty -= 360;

	EngineStatus.m_vRotation[2] = (float)sin(EngineStatus.m_roty/180*3.14)/100;
	EngineStatus.m_vRotation[1] = (float)cos(EngineStatus.m_rotx/180*3.14)*vecxy/100;
	EngineStatus.m_vRotation[0] = (float)-sin(EngineStatus.m_rotx/180*3.14)*vecxy/100;

	EngineStatus.m_posx += EngineStatus.m_vRotation[0]*EngineStatus.m_fSpeed;
	EngineStatus.m_posy += EngineStatus.m_vRotation[1]*EngineStatus.m_fSpeed;
	EngineStatus.m_posz += EngineStatus.m_vRotation[2]*EngineStatus.m_fSpeed;
}