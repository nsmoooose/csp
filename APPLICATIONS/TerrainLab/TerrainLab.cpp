///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab  - CSP - http://csp.homeip.net
//
//   coded by Stormbringer and		zzed
//   email:   storm_bringer@gmx.li	zzed@gmx.li
//
///////////////////////////////////////////////////////////////////////////
//
//   TerrainLab Main file based upon Jeff Molofee's Revised OpenGL Basecode
//	 found at http://nehe.gamedev.net  
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	09/25/2001	file created	-	Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#include <windows.h>											// Header File For Windows
//#include <gl\gl.h>												// Header File For The OpenGL32 Library
//#include <gl\glu.h>												// Header File For The GLu32 Library
//#include <gl\glaux.h>											// Header File For The GLaux Library
#include "BaseCode.h"											// Header File For NeHeGL
#include <math.h>
#include "TerrainData.h"
#include "Terrain.h"
#include "Camera.h"
#include "DlgStartup.h"
#include "DlgProcess.h"
#include "DlgExtract.h"
#include "Fractal.h"
#include "SwapInterval.h"

#include "OGLText.h"

#pragma comment( lib, "opengl32.lib" )							// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )								// Search For GLu32.lib While Linking
#pragma comment( lib, "glaux.lib" )								// Search For GLaux.lib While Linking

#ifndef CDS_FULLSCREEN											// CDS_FULLSCREEN Is Not Defined By Some
#define CDS_FULLSCREEN 4										// Compilers. By Defining It This Way,
#endif															// We Can Avoid Errors

extern CTerrainData *m_pTerrainData;							// Global class containing all terrain parameters
extern CTerrain		*m_pTerrain;								// The main Terrain class
extern CCamera	    *m_pCamera;							
extern CDlgStartup	 m_cDlgStartup;
extern CDlgProcess   m_cDlgProcess;
extern CDlgExtract   m_cDlgExtract;
extern CFractal		 Fractal;
extern CSwapInterval cSwapInterval;

GL_Window*	g_window;
Keys*		g_keys;

bool multitextureSupported=false;					// Flag Indicating Whether Multitexturing Is Supported
bool useMultitexture=true;							// Use It If It Is Supported?
short texturesAvailable=UNKNOWN;						// This is true when textures are available on hard disk

// User Defined Variables
float		angle;												// Used To Rotate The Triangles
int			iLastMouseposX, iLastMouseposY;					// Used for storing mouse motion
int			iRetVal;
int			iMode;											// Fractal? Quad Map?

BOOL Initialize (GL_Window* window, Keys* keys)					// Any GL Init Code & User Initialiazation Goes Here
{
	g_window	= window;
	g_keys		= keys;

	iLastMouseposX = iLastMouseposY = 0;

	// Start Of User Initialization
	multitextureSupported=initMultitexture();
	
	glClearColor (0.8f, 0.85f, 0.9f, 1.0f);						// Black Background
	glClearDepth (1.0f);										// Depth Buffer Setup
	glShadeModel (GL_SMOOTH);									// Select Smooth Shading
	glDepthFunc (GL_LEQUAL);									// The Type Of Depth Testing (Less Or Equal)
	glEnable (GL_DEPTH_TEST);									// Enable Depth Testing
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Set Perspective Calculations To Most Accurate
	OGLText.Init();

	return TRUE;												// Return TRUE (Initialization Successful)
}

void Deinitialize (void)										// Any User DeInitialization Goes Here
{
	if(m_pCamera) delete m_pCamera;
	if(m_pTerrain) delete m_pTerrain;		
	return;
}

void Update (DWORD milliseconds)								// Perform Motion Updates Here
{
	ProcessFractKeys(milliseconds);

	if (g_keys->keyDown [VK_ESCAPE] == TRUE)					// Is ESC Being Pressed?
	{
		TerminateApplication (g_window);						// Terminate The Program
	}

	if (g_keys->keyDown [VK_LEFT] == TRUE)
	{
		m_pCamera->m_fRotX += .5;
	}

	if (g_keys->keyDown [VK_RIGHT] == TRUE)
	{
		m_pCamera->m_fRotX -= .5;
	}

	if (g_keys->keyDown [VK_DOWN] == TRUE)
	{
		m_pCamera->m_fRotY += .5;
	}

	if (g_keys->keyDown [VK_UP] == TRUE)
	{
		m_pCamera->m_fRotY -= .5;
	}

	if (g_keys->keyDown ['A'] == TRUE)
	{
		m_pCamera->m_fSpeed += 5.0f;
	}

	if (g_keys->keyDown ['Y'] == TRUE)
	{
		m_pCamera->m_fSpeed -= 5.0f;
	} 

	if (g_keys->keyDown ['Q'] == TRUE)
	{
		m_pCamera->m_fSpeed = 0;
	} 

	if (g_keys->keyDown ['V'] == TRUE)
	{
		m_pTerrainData->m_bActivateVSync = 1 - m_pTerrainData->m_bActivateVSync;
		cSwapInterval.SetInterval(m_pTerrainData->m_bActivateVSync);
	} 

	if (g_keys->keyDown ['S'] == TRUE)
	{
		m_pTerrainData->m_bRenderMode = RENDER_MODE_SHADED;
		g_keys->keyDown ['S'] = FALSE;
	} 

	if (g_keys->keyDown ['W'] == TRUE)
	{
		m_pTerrainData->m_bRenderMode = RENDER_MODE_TRIANGLES;
		g_keys->keyDown ['W'] = FALSE;
	} 

	if (g_keys->keyDown ['C'] == TRUE)
	{
		m_pTerrainData->m_bShadingMode = SHADING_MODE_ECO_COLOR;
		g_keys->keyDown ['C'] = FALSE;
	} 

	if (g_keys->keyDown ['L'] == TRUE)
	{
		m_pTerrainData->m_bShadingMode = SHADING_MODE_SLOPE;
		g_keys->keyDown ['L'] = FALSE;
	} 

	if (g_keys->keyDown ['R'] == TRUE)
	{
		m_pTerrainData->m_bShadingMode = SHADING_MODE_REL_ELEVATION;
		g_keys->keyDown ['R'] = FALSE;
	} 

	if (g_keys->keyDown ['T'] == TRUE)
	{
		m_pTerrainData->m_bRenderMode = RENDER_MODE_TEXTURED;
		g_keys->keyDown ['T'] = FALSE;
	} 

	if (g_keys->keyDown ['E'] == TRUE)
	{
		if(m_pTerrainData->m_bEcosystemMode == ECOSYSTEM_MODE_SHARP)
			m_pTerrainData->m_bEcosystemMode = ECOSYSTEM_MODE_SMOOTH;
		else
			m_pTerrainData->m_bEcosystemMode = ECOSYSTEM_MODE_SHARP;

		g_keys->keyDown ['E'] = FALSE;
	} 

	if (g_keys->keyDown [VK_F1] == TRUE)
	{
		// Only function provided here is the loading of a mesh
		iRetVal = m_cDlgStartup.QueryParameters();
		if(m_pTerrainData->m_bStartupAction == DLG_LOAD_NEW)
		{
			if(iMode == MODE_FRACTAL)
				m_pTerrain->LoadRAWData();
		}
		else if(m_pTerrainData->m_bStartupAction == DLG_LOAD_DEFAULT)
		{
			m_pTerrain->LoadDefaultMesh();
		}

		g_keys->keyDown [VK_F1] = FALSE;
	}

/*	if (g_keys->keyDown [VK_F2] == TRUE)
	{
		// Call the dialog. no return values.
		iRetVal = m_cDlgProcess.QueryParameters();
		g_keys->keyDown [VK_F2] = FALSE;
	}

	if (g_keys->keyDown [VK_F3] == TRUE)
	{
		// Call the dialog. no return values.
		iRetVal = m_cDlgExtract.QueryParameters();
		g_keys->keyDown [VK_F3] = FALSE;
	}
*/

	// update the camera position 
	CalculateFPS(milliseconds);
	UpdateCamera();
}

void UpdateCamera(void)
{
	float vecxy = (float)cos(m_pCamera->m_fRotY/180*3.14);

	if (m_pCamera->m_fRotX < 0) m_pCamera->m_fRotX += 360;
	else if (m_pCamera->m_fRotX > 360) m_pCamera->m_fRotX -= 360;
	if (m_pCamera->m_fRotY <0 ) m_pCamera->m_fRotY += 360;
	else if (m_pCamera->m_fRotY) m_pCamera->m_fRotY -= 360;

	m_pCamera->m_vRotation[2] = (float)sin(m_pCamera->m_fRotY/180*3.14);
	m_pCamera->m_vRotation[1] = (float)cos(m_pCamera->m_fRotX/180*3.14)*vecxy;
	m_pCamera->m_vRotation[0] = (float)-sin(m_pCamera->m_fRotX/180*3.14)*vecxy;

	m_pCamera->m_fPosX += m_pCamera->m_vRotation[0]*m_pCamera->m_fSpeed;
	m_pCamera->m_fPosY += m_pCamera->m_vRotation[1]*m_pCamera->m_fSpeed;
	m_pCamera->m_fPosZ += m_pCamera->m_vRotation[2]*m_pCamera->m_fSpeed;
}

void Draw (void)
{
	switch(m_pTerrainData->m_bRenderMode)
	{
	case RENDER_MODE_TRIANGLES:
		m_pTerrain->RenderTriangles();
		break;
	case RENDER_MODE_SHADED:
		m_pTerrain->RenderShaded();
		break;
	case RENDER_MODE_TEXTURED:
		m_pTerrain->RenderTextured();
		break;
	}
}

void CalculateFPS(DWORD milliseconds)
{
	if(milliseconds > 0)
	{
		m_pTerrainData->m_BenchRes.iFPSnow = 1000/milliseconds;
		if(m_pTerrainData->m_BenchRes.iFPSnow > m_pTerrainData->m_BenchRes.iFPSmax)
			m_pTerrainData->m_BenchRes.iFPSmax = m_pTerrainData->m_BenchRes.iFPSnow;
		else if(m_pTerrainData->m_BenchRes.iFPSnow < m_pTerrainData->m_BenchRes.iFPSmin)
			m_pTerrainData->m_BenchRes.iFPSmin = m_pTerrainData->m_BenchRes.iFPSnow;
	}
	return;
}

void ProcessFractKeys(DWORD milliseconds)
{
	int iCount;

	if (g_keys->keyDown [VK_ADD] == TRUE)
	{
		m_pTerrain->Iterate(m_pTerrainData->m_fFactorD);
		// the fractal parameters for each step are stored in m_aFLP
		m_pTerrainData->m_aFLP[m_pTerrainData->m_iIterationLevel].fFactorD = m_pTerrainData->m_fFactorD;
		m_pTerrainData->m_aFLP[m_pTerrainData->m_iIterationLevel+1].fFactorD = 
			m_pTerrainData->m_fFactorD * (float)pow(2, -1);
		
		m_pTerrainData->m_iIterationLevel++;
		m_pTerrainData->m_fFactorD = m_pTerrainData->m_aFLP[m_pTerrainData->m_iIterationLevel].fFactorD;

		g_keys->keyDown [VK_ADD] = FALSE;
	} 

	if (g_keys->keyDown [VK_SUBTRACT] == TRUE && m_pTerrainData->m_iIterationLevel > 0)
	{
		// We don't store the heightmaps, but calculate them from the beginning on again
		if(m_pTerrainData->m_bStartupAction == DLG_LOAD_NEW)
		{
			m_pTerrain->LoadRAWData();
		}
		else if(m_pTerrainData->m_bStartupAction == DLG_LOAD_DEFAULT)
		{
			m_pTerrain->LoadDefaultMesh();
		}
	
		for(iCount = 0; iCount < m_pTerrainData->m_iIterationLevel-1;iCount++)
		{
			m_pTerrain->Iterate(m_pTerrainData->m_aFLP[iCount].fFactorD);
		}
		m_pTerrainData->m_iIterationLevel--;
		m_pTerrainData->m_fFactorD = m_pTerrainData->m_aFLP[iCount].fFactorD;

		g_keys->keyDown [VK_SUBTRACT] = FALSE;
	} 

	if (g_keys->keyDown [VK_MULTIPLY] == TRUE && m_pTerrainData->m_iIterationLevel > 0)
	{
		// Do the smooth step
		Fractal.SmoothStep(1.0f);
		m_pTerrain->BuildVertexArray(m_pTerrainData->m_iMeshWidth);
		g_keys->keyDown [VK_MULTIPLY] = FALSE;
	}

	if (g_keys->keyDown [VK_DIVIDE] == TRUE && m_pTerrainData->m_iIterationLevel > 0)
	{
		// Do the smooth step
		Fractal.SmoothStepAlternative(0.1f);
		m_pTerrain->BuildVertexArray(m_pTerrainData->m_iMeshWidth);
		g_keys->keyDown [VK_DIVIDE] = FALSE;
	}


	if (g_keys->keyDown [VK_NUMPAD8] == TRUE)
	{
		m_pTerrainData->m_fFactorD += 0.01f;
	}

	if (g_keys->keyDown [VK_NUMPAD2] == TRUE)
	{
		m_pTerrainData->m_fFactorD -= 0.01f;
		if(m_pTerrainData->m_fFactorD < 0.0f)
		   m_pTerrainData->m_fFactorD = 0.0f;
	}
}
