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
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library
#include <gl\glaux.h>											// Header File For The GLaux Library
#include <math.h>
#include "BaseCode.h"											// Header File For NeHeGL
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
		m_pCamera->m_fSpeed += .1f;
	}

	if (g_keys->keyDown ['Y'] == TRUE)
	{
		m_pCamera->m_fSpeed -= .1f;
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
		m_pTerrainData->m_bRenderMode = m_pTerrainData->m_bRenderMode = RENDER_MODE_SHADED;
		g_keys->keyDown ['S'] = FALSE;
	} 

	if (g_keys->keyDown ['W'] == TRUE)
	{
		m_pTerrainData->m_bRenderMode = m_pTerrainData->m_bRenderMode = RENDER_MODE_TRIANGLES;
		g_keys->keyDown ['W'] = FALSE;
	} 

	if (g_keys->keyDown ['S'] == TRUE)
	{
		m_pTerrainData->m_bRenderMode = m_pTerrainData->m_bRenderMode = RENDER_MODE_SQUARES;
		g_keys->keyDown ['S'] = FALSE;
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

	if (g_keys->keyDown [VK_F2] == TRUE)
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
//	case RENDER_MODE_SQUARES:
//		m_pTerrain->RenderSquares();
		break;
	case RENDER_MODE_TRIANGLES:
		m_pTerrain->RenderTriangles();
		break;
	case RENDER_MODE_SHADED:
		m_pTerrain->RenderShaded();
		break;
	}
}

/*void DrawFractalMesh(void)
{
	short x, y, gd;
	int tricount = 0, vertcount = 0;

	float mat_specular[] = { 0, 0, 0, 0 };
	float mat_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float white_light[] = { 1.0, 1.0, 1.0, 1.0 };
	float col_ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };

	gd = m_pTerrainData->m_iMeshGridDistance;

	if(m_pTerrainData->m_bLight)
	{
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);

		glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
		glLightfv(GL_LIGHT0, GL_AMBIENT, col_ambient);

		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
		glLoadIdentity ();											// Reset The Modelview Matrix
		gluLookAt(0, 0, 0, 0, 1, 0, 0, 0, 1);

		glRotatef(m_pCamera->m_fRotY, -1.0f, 0.0f, 0.0f);
		glRotatef(m_pCamera->m_fRotX, 0.0f, 0.0f, -1.0f); 
		glTranslatef (-m_pCamera->m_fPosX, -m_pCamera->m_fPosY, -m_pCamera->m_fPosZ);

		// this is used to show the position of the light source
		glDisable(GL_LIGHTING);
		glPointSize(1000.0f);
		glBegin(GL_POINTS);
		glColor3f(1.0, 1.0, 0.0);
		glVertex3fv(m_pTerrainData->m_faLightPos);
		glEnd();

		glEnable(GL_LIGHTING);
		glLightfv(GL_LIGHT0, GL_POSITION, m_pTerrainData->m_faLightPos);

		for(y=0; y<m_pTerrainData->m_iMeshWidth-1;y++)
			for(x=0; x<m_pTerrainData->m_iMeshWidth-1; x++)
			{
				glBegin(GL_TRIANGLES);
				glNormal3fv(m_pTerrainData->m_paNormals[y*m_pTerrainData->m_iMeshWidth+x]);			
				glVertex3fv(m_pTerrainData->m_paVertices[y*m_pTerrainData->m_iMeshWidth+x]);		// left lower 
				glNormal3fv(m_pTerrainData->m_paNormals[y*m_pTerrainData->m_iMeshWidth+x+1]);			
				glVertex3fv(m_pTerrainData->m_paVertices[y*m_pTerrainData->m_iMeshWidth+x+1]);		// right lower
				glNormal3fv(m_pTerrainData->m_paNormals[(y+1)*m_pTerrainData->m_iMeshWidth+x]);			
				glVertex3fv(m_pTerrainData->m_paVertices[(y+1)*m_pTerrainData->m_iMeshWidth+x]);    // left upper
				glEnd();
	
				glBegin(GL_TRIANGLES);
				glNormal3fv(m_pTerrainData->m_paNormals[y*m_pTerrainData->m_iMeshWidth+x+1]);
				glVertex3fv(m_pTerrainData->m_paVertices[y*m_pTerrainData->m_iMeshWidth+x+1]);		// right lower 
				glNormal3fv(m_pTerrainData->m_paNormals[(y+1)*m_pTerrainData->m_iMeshWidth+x+1]);
				glVertex3fv(m_pTerrainData->m_paVertices[(y+1)*m_pTerrainData->m_iMeshWidth+x+1]);  // right upper
				glNormal3fv(m_pTerrainData->m_paNormals[(y+1)*m_pTerrainData->m_iMeshWidth+x]);
				glVertex3fv(m_pTerrainData->m_paVertices[(y+1)*m_pTerrainData->m_iMeshWidth+x]);	// left upper
				glEnd();

				tricount+=2;
				vertcount+=6; 

			}
			
		glDisable(GL_LIGHTING);
		glColor3f(0.0, 0.0, 0.0);
		OGLText.Print(0, 0, "Iteration: %i", m_pTerrainData->m_iIterationLevel);
		OGLText.Print(0, 1, "FactorD:	%f", m_pTerrainData->m_fFactorD);
		OGLText.Print(0, 2, "FPS:		%i, Vertices: %i, Triangles: %i", m_pTerrainData->m_BenchRes.iFPSnow, vertcount, tricount);
		OGLText.Print(0, 3, "Sun X:		%i, Sun Y:	  %i, Sun Z		%i", 
					  (int)m_pTerrainData->m_faLightPos[0], (int)m_pTerrainData->m_faLightPos[1],
					  (int)m_pTerrainData->m_faLightPos[2]);
	}

	// render without gllighting
	else
	{
		glDisable(GL_LIGHTING);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
		glLoadIdentity ();											// Reset The Modelview Matrix
		gluLookAt(0, 0, 0, 0, 1, 0, 0, 0, 1);
	
		glColor3i(255, 255, 255);
		glRotatef(m_pCamera->m_fRotY, -1.0f, 0.0f, 0.0f);
		glRotatef(m_pCamera->m_fRotX, 0.0f, 0.0f, -1.0f); 
		glTranslatef (-m_pCamera->m_fPosX, -m_pCamera->m_fPosY, -m_pCamera->m_fPosZ);

		for(y=0; y<m_pTerrainData->m_iMeshWidth-1;y++)
			for(x=0; x<m_pTerrainData->m_iMeshWidth-1; x++)
			{
				glBegin(GL_LINE_STRIP);
				glVertex3fv(m_pTerrainData->m_paVertices[y*m_pTerrainData->m_iMeshWidth+x]);		// left lower 
				glVertex3fv(m_pTerrainData->m_paVertices[y*m_pTerrainData->m_iMeshWidth+x+1]);		// right lower
				glVertex3fv(m_pTerrainData->m_paVertices[(y+1)*m_pTerrainData->m_iMeshWidth+x+1]);    // left upper
				glVertex3fv(m_pTerrainData->m_paVertices[(y+1)*m_pTerrainData->m_iMeshWidth+x]);    // left upper
				glEnd();

				tricount+=1;
				vertcount+=4; 
			}

		glColor3i(0, 0, 0);
		OGLText.Print(0, 0, "Iteration: %i", m_pTerrainData->m_iIterationLevel);
		OGLText.Print(0, 1, "FactorD:	%f", m_pTerrainData->m_fFactorD);
		OGLText.Print(0, 2, "FPS:		%i, Vertices: %i, Triangles: %i", m_pTerrainData->m_BenchRes.iFPSnow, vertcount, tricount);
	}


	glFlush ();													// Flush The GL Rendering Pipeline
}
*/
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
