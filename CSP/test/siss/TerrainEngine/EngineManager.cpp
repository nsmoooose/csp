///////////////////////////////////////////////////////////////////////////
//
//   test of a terrain-engine based on GeoMipMapping
//
//   coded by zzed and Stormbringer
//   email:   zzed@gmx.li    storm_bringer@gmx.li 
//
//   7.2.2001
//
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EngineManager.h"
#include "glTexFont.h"
#include "FrameCounter.h"
//#include "Texture.h"

#include <math.h>


EngineManager *eManager;
CFrameCounter FrameCounter;
//CTexture	  m_Texture;	


EngineManager::EngineManager()
{
	m_mousestate = GLUT_UP;	
	m_posx = 2;
	m_posy = 2;
	m_posz = 3.0f;
	m_rotx = 0;
	m_roty = 0;
	m_fSpeed = 0;
	m_lastmouseposx = 0;
	m_lastmouseposy = 0;
	m_vRotation[0] = 0;
	m_vRotation[1] = 0;
	m_vRotation[2] = 0;
	m_bLines = true;
}

EngineManager::~EngineManager()
{
	delete m_pTerrainData;
	delete m_renderer;
}


void EngineManager::StartEngine(int argc, char **argv)
{
	eManager = this;
	InitGLUT(argc, argv);
	InitGL();

	m_pTerrainData = new CTerrainData("terrain2.raw", false);
	m_renderer = new CMipMapRenderer(m_pTerrainData);
	m_iShowStats = TRUE;
	
	if(!fontLoad("font.tga"))
	{
		printf("unable to load font!\n");
		char text[100];
		gets(text);
		exit(1);
	}
	// Load bitmap and initialize texture
	/*if(!m_Texture.LoadBMP("Texture_erde.bmp"))
	{
		printf("unable to load texture!\n");
		char text[100];
		gets(text);
		exit(1);
	}*/

	glutMainLoop();
}

void EngineManager::InitGLUT(int argc, char **argv)
{
	printf("Initializing GLUT ...\n");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(320, 240);
	glutCreateWindow("Simulation");
	glutKeyboardFunc(EngineManager::OnKeyboard);
	glutIdleFunc(EngineManager::OnIdle);
	glutDisplayFunc(EngineManager::DrawWindow);
	glutMouseFunc(EngineManager::OnMouse);
	glutMotionFunc(EngineManager::OnMouseMotion);
}

void EngineManager::InitGL()
{
	glClearColor(0.0, 0.0, 1.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);

}

void EngineManager::OnKeyboard(unsigned char key, int, int)
{
	switch (key)
	{
	case 'a': 
		{
			eManager->m_fSpeed += 0.3;
			break;
		}
	case 'y': 
		{
			eManager->m_fSpeed -= 0.3;
			break;
		}
	case 's':
		{
			eManager->m_iShowStats = 1-eManager->m_iShowStats;
			break;
		}
	case 'x':
		{
			eManager->m_bLines = !eManager->m_bLines;
			break;
		}
	case 'q':
		{
			delete eManager;
			exit(0);
		}
	}
}

void EngineManager::OnIdle()
{
	eManager->m_posx += eManager->m_vRotation[0]*eManager->m_fSpeed;;
	eManager->m_posy += eManager->m_vRotation[1]*eManager->m_fSpeed;;
	eManager->m_posz += eManager->m_vRotation[2]*eManager->m_fSpeed;;
	glutPostRedisplay();
}

void EngineManager::DrawWindow()
{
	FrameCounter.StartFrame();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	eManager->m_renderer->SetPosition(eManager->m_posx, eManager->m_posy, eManager->m_posz);
	eManager->m_renderer->SetRotation(eManager->m_rotx, eManager->m_roty);
	gluPerspective(60, 1, 1, eManager->m_renderer->GetClippingDistance());    // be careful! near and far aren't allowed to have too many powers of ten between them!
	gluLookAt(0, 0, 0, 0, 1, 0, 0, 0, 1);
	glRotatef(eManager->m_roty, -1, 0, 0);
	glRotatef(eManager->m_rotx, 0, 0, -1);
	glTranslatef(-eManager->m_posx, -eManager->m_posy, -eManager->m_posz);
	/*m_Texture.Use();
	if (eManager->m_bLines) 
		glBegin(GL_LINES);
	else 
		glBegin(GL_TRIANGLES);*/
	eManager->m_renderer->Render(eManager->m_bLines);
	/*eManager->m_renderer->Render(eManager->m_posx, eManager->m_posy, eManager->m_posz,
		eManager->m_rotx, eManager->m_roty, eManager->m_bLines);*/
	//glEnd();
	FrameCounter.EndFrame();

	if(eManager->m_iShowStats)
	{
		fontDrawString(0, 32, "Current Speed %.1f", eManager->m_fSpeed);
		fontDrawString(0, 48, "FPS: %.2f",FrameCounter.GetFrameCount()); 
		fontDrawString(0, 64, "X: %.2f, Y: %.2f, Z: %.2f", eManager->m_posx, eManager->m_posy, eManager->m_posz);
		fontDrawString(0, 16, "Current Rotation Specs: x: %.2f, y: %.2f", eManager->m_rotx, eManager->m_roty);
		fontDrawString(0, 0, "NoTriangles: %d", eManager->m_renderer->m_iTriangleCount);
	}

	glutSwapBuffers();
}

void EngineManager::OnMouse(int, int state, int x, int y)
{
	eManager->m_mousestate = state;
	eManager->m_lastmouseposx = x;
	eManager->m_lastmouseposy = y;
}

void EngineManager::OnMouseMotion(int x, int y)
{
	eManager->m_rotx -= x-eManager->m_lastmouseposx;
	eManager->m_roty += y-eManager->m_lastmouseposy;
	eManager->m_lastmouseposx = x;
	eManager->m_lastmouseposy = y;
	if (eManager->m_rotx<0) eManager->m_rotx += 360;
	else if (eManager->m_rotx>360) eManager->m_rotx -=360;
	if (eManager->m_roty<0) eManager->m_roty += 360;
	else if (eManager->m_roty>360) eManager->m_roty -= 360;

	float vecxy = cos(eManager->m_roty/180*3.14);
	eManager->m_vRotation[2] = sin(eManager->m_roty/180*3.14)/100;
	eManager->m_vRotation[1] = cos(eManager->m_rotx/180*3.14)*vecxy/100;
	eManager->m_vRotation[0] = -sin(eManager->m_rotx/180*3.14)*vecxy/100;
}