///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   definition of class EngineManager - it defines the interface between
//   the terrain engine itself and the input/output devices
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	6/13/2001	moved interpretation of SCN-CFG file to CSCNDataLoader - zzed
//	8/07/2001	added support for the DevIL Image Library - Stormbringer
//
//
//
///////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include "stdafx.h"
#endif

#include "EngineManager.h"

#include "FrameCounter.h"
#include "../../SISS/TerrainEngine/Tools.h"
#include "SISS/TerrainEngine/ResourceManager.h"
#include "SISS/TerrainEngine/GLTexture.h"

#include <math.h>

#ifndef TRUE
#define TRUE 1
#endif

static char *sConfigData;
EngineManager *eManager;
CFrameCounter FrameCounter;

EngineManager::EngineManager()
{
        printf("EngineManager Constructor\n");
//	m_mousestate = GLUT_UP;	
	m_vPos.x = 2;
	m_vPos.y = 2;
	m_vPos.z = 3.0f;
	m_rotx = 0;
	m_roty = 0;
	m_fSpeed = 0;
	m_lastmouseposx = 0;
	m_lastmouseposy = 0;
	m_vRotation[0] = 0;
	m_vRotation[1] = 0;
	m_vRotation[2] = 0;
	m_ucDrawingStyle = TE_DS_SMOOTH;
	m_iWindowHeight = 480;
	m_iWindowWidth = 640;
	m_fSplashAngle = 0;
	m_fSplashRotSpeed = 1;
	m_tLastFrame = 0;
}

EngineManager::~EngineManager()
{
	ResManager.Unlock(m_hSplashTex);
	delete m_pTerrainEngine;
}

/*void EngineManager::ReadInitialValues()
{
	char sStringBuffer[1024];

	CTools::GetConfigData("m_fCameraXPos", sConfigData, sStringBuffer); 
	m_posx = atof(sStringBuffer);
	
	CTools::GetConfigData("m_fCameraYPos", sConfigData, sStringBuffer); 
	m_posy = atof(sStringBuffer);
	
	CTools::GetConfigData("m_fCameraZPos", sConfigData, sStringBuffer); 
	m_posz = atof(sStringBuffer);

	CTools::GetConfigData("m_fCameraSpeed", sConfigData, sStringBuffer); 
	m_fSpeed = atof(sStringBuffer);

	CTools::GetConfigData("m_ucDrawingStyle", sConfigData, sStringBuffer); 
	m_ucDrawingStyle = (unsigned char)atoi(sStringBuffer);

	CTools::GetConfigData("m_ucMapType", sConfigData, sStringBuffer); 
	m_ucMapType = (unsigned char)atoi(sStringBuffer);
}*/

void EngineManager::StartEngine(int argc, char **argv)
{
        printf("EngineManager::StartEngine\n");

	eManager = this;

	/*iFile = _open(argv[1], _O_BINARY);			// open the config-file given by the command line
	if(iFile == -1)
	{	
		printf("unable to open '%s'!\n", (char *)argv[1]);
		char text[100];
		gets(text);
		exit(1);
	}
	iFileLength = _filelength(iFile);
	sConfigData = new char[iFileLength+1];
	iSuccess = _read(iFile, sConfigData, (iFileLength*sizeof(char)));
	_close(iFile);*/

	InitGLUT(argc, argv);
	InitGL();

	// Initialize IL, ILU, ILUT
	ilInit();
	iluInit();
	ilutInit();

	// Initialize ILUT with OpenGL support.
	ilutRenderer(ILUT_OPENGL);

	//m_pTerrainData = new CTerrainData("../../SISS/TerrainData/terrain.raw", false);
	//m_renderer = new CMipMapRenderer(m_pTerrainData);
	m_pTerrainEngine = new CTerrainEngine();

	printf("\nUse mouse to rotate camera\n");
	printf("Use 'a' and 'y' keys to adjust camera speed\n");
	printf("Use '1' and '2' keys to adjust splash rotation speed\n");

        printf("Loading Terrain\n");

	m_pTerrainEngine->LoadTerrain(argv[1]);

        printf("Setting Up Sky\n");
	m_pTerrainEngine->SetupSky();

        printf("Setting up Water\n");
	m_pTerrainEngine->SetupWater();
	m_pTerrainEngine->SetDrawingStyle(m_ucDrawingStyle);
	m_vPos = m_pTerrainEngine->GetPosition();

	m_iShowStats = TRUE;

	// Load splash-texture
	ResManager.InsertResource(&m_hSplashTex, new CGLTexture("../../SISS/TerrainData/CSP_Splash.tga", GL_RGB));
	CGLTexture *pTexture = static_cast<CGLTexture*>(ResManager.GetResource(m_hSplashTex));
	ResManager.Lock(m_hSplashTex);
	m_hGLSplashTex = pTexture->GetGLHandle();

//	glutMainLoop();
	delete[] sConfigData;
}

void EngineManager::OnReshapeWindow(int width, int height)
{
	eManager->m_iWindowHeight = height;
	eManager->m_iWindowWidth = width;

	glViewport(0, 0, width, height);			
}

void EngineManager::InitGLUT(int argc, char **argv)
{
	printf("Initializing GLUT ...\n");
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
//	glutInitWindowSize(640, 480);
//	glutInitWindowPosition(320, 240);
//	glutCreateWindow("Simulation");
	//glutFullScreen();
//	glutKeyboardFunc(EngineManager::OnKeyboard);
//	glutIdleFunc(EngineManager::OnIdle);
//	glutDisplayFunc(EngineManager::DrawWindow);
//	glutMouseFunc(EngineManager::OnMouse);
//	glutMotionFunc(EngineManager::OnMouseMotion);
//	glutReshapeFunc(EngineManager::OnReshapeWindow);
}

void EngineManager::InitGL()
{
	glClearColor(0.0, 0.0, 1.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, m_iWindowWidth/m_iWindowHeight, 1.0f, 5000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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
			switch (eManager->m_ucDrawingStyle)
			{
				case TE_DS_TEXTURED:
					eManager->m_ucDrawingStyle = TE_DS_SMOOTH;
					break;
				case TE_DS_SMOOTH:
					eManager->m_ucDrawingStyle = TE_DS_WIREFRAME;
					break;
				case TE_DS_WIREFRAME:
					default:
					eManager->m_ucDrawingStyle = TE_DS_TEXTURED;
			}
			break;
		}
	case '1':
		{
			eManager->m_fSplashRotSpeed -= 0.1;
			break;
		}
	case '2':
		{
			eManager->m_fSplashRotSpeed += 0.1;
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
	if (eManager->m_tLastFrame!=0)
	{
#ifdef WIN32
		float diff = (float)(timeGetTime()-eManager->m_tLastFrame)/300;
#else
		struct timeval time_val;
		gettimeofday(&time_val, NULL);
		float diff = (float)(time_val.tv_usec-eManager->m_tLastFrame)/300;
#endif
		eManager->m_vPos.x += eManager->m_vRotation[0]*eManager->m_fSpeed*diff;
		eManager->m_vPos.y += eManager->m_vRotation[1]*eManager->m_fSpeed*diff;
		eManager->m_vPos.z += eManager->m_vRotation[2]*eManager->m_fSpeed*diff;
	}
#ifdef WIN32
	eManager->m_tLastFrame = timeGetTime();
#else
	struct timeval time_val;
	gettimeofday(&time_val, NULL);
	eManager->m_tLastFrame = time_val.tv_usec;

#endif
//	glutPostRedisplay();
}

void EngineManager::DrawWindow()
{
	FrameCounter.StartFrame();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	eManager->m_pTerrainEngine->SetPosition(eManager->m_vPos);
	eManager->m_pTerrainEngine->SetRotation(eManager->m_rotx, eManager->m_roty);
	gluLookAt(0, 0, 0, 0, 1, 0, 0, 0, 1);
	
	//m_Texture.Use();
	eManager->m_pTerrainEngine->SetDrawingStyle(eManager->m_ucDrawingStyle);
	eManager->m_pTerrainEngine->Render();
	/*eManager->m_renderer->Render(eManager->m_posx, eManager->m_posy, eManager->m_posz,
		eManager->m_rotx, eManager->m_roty, eManager->m_bLines);*/
	//glEnd();
//	FrameCounter.EndFrame();

	// show our nice splash-texture .. ;)

	// we NEED orthogonal view
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	
	glPushAttrib(GL_ENABLE_BIT);
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, eManager->m_hGLSplashTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTranslatef(0.55f, 0.8f, 0.0f);
	glScalef(0.15f, 0.15f, 0.15f);
	glRotatef(3, 1, 0, 0);
	glRotatef((eManager->m_fSplashAngle += eManager->m_fSplashRotSpeed), 0, 2, 0);
	glTranslatef(-2, -0.5, 0);
	glBegin(GL_QUADS);
		glColor4f(1, 1, 1, 0.5);
		glTexCoord2f(0, 0);
		glVertex3f(0, 0, 0);
		glTexCoord2f(1, 0);;
		glVertex3f(4.0, 0.0, 0.0);
		glTexCoord2f(1, 1);;
		glVertex3f(4.0, 1.0, 0.0);
		glTexCoord2f(0, 1);;
		glVertex3f(0.0, 1.0, 0.0);
	glEnd();
	if (eManager->m_fSplashAngle>360) eManager->m_fSplashAngle -= 360;

	glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	if(eManager->m_iShowStats)
	{
		/*glLoadIdentity();
		glTranslatef(-0.9, 0, 0);
		glScalef(0.03, 0.04, 0.04);*/
		//gpOGLText->Print(0, 0, "fuck, warum läuft die Scheisse nicht?");
		OGLText.Print(0, 0, "Current Speed %.1f", eManager->m_fSpeed);
		OGLText.Print(0, 1, "FPS: %.2f",FrameCounter.GetFrameCount()); 
		OGLText.Print(0, 2, "X: %.2f, Y: %.2f, Z: %.2f", eManager->m_vPos.x, eManager->m_vPos.y, eManager->m_vPos.z);
		OGLText.Print(0, 3, "Current Rotation Specs: x: %.2f, y: %.2f", eManager->m_rotx, eManager->m_roty);
		OGLText.Print(0, 4, "NoTriangles: %d", eManager->m_pTerrainEngine->GetNoRenderedTriangles());
	}

//	glutSwapBuffers();
	
	FrameCounter.EndFrame();
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
	eManager->m_vRotation[2] = sin(eManager->m_roty/180*3.14);
	eManager->m_vRotation[1] = cos(eManager->m_rotx/180*3.14)*vecxy;
	eManager->m_vRotation[0] = -sin(eManager->m_rotx/180*3.14)*vecxy;
//	printf("Rotation: (%.2f, %.2f, %.2f); source: (%.2f, %.2f)\n", eManager->m_vRotation[0], eManager->m_vRotation[1], eManager->m_vRotation[2],
//		eManager->m_rotx, eManager->m_roty);
}
