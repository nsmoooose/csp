
#include "TerrainApp.h"

//EngineManager *eManager;
static char *sConfigData;

TerrainApp theApp;

TerrainApp::TerrainApp() : BaseApp()
{
	CSP_LOG(CSP_APP, CSP_TRACE, "TerrainApp::TerrainApp()");

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

TerrainApp::~TerrainApp()
{
	CSP_LOG(CSP_APP, CSP_TRACE, "TerrainApp::~TerrainApp()");

	ResManager.Unlock(m_hSplashTex);
	delete m_pTerrainEngine;

}


short TerrainApp::Initialize(_APP_INIT_STRUCT *AppInit)
{
	CSP_LOG(CSP_APP, CSP_TRACE, "TerrainApp::Initialize()");
	CSP_LOG(CSP_APP, CSP_INFO, "Start Engine....");
	
    // Initialize must call the base initialize
	short rtnval = BaseApp::Initialize(AppInit);

//	eManager = this;

	// Initialize IL, ILU, ILUT
	ilInit();
	iluInit();
	ilutInit();

	// Initialize ILUT with OpenGL support.
	ilutRenderer(ILUT_OPENGL);

	//m_pTerrainData = new CTerrainData("../../SISS/TerrainData/terrain.raw", false);
	//m_renderer = new CMipMapRenderer(m_pTerrainData);
	m_pTerrainEngine = new CTerrainEngine();

	CSP_LOG(CSP_APP, CSP_INFO, "Use mouse to rotate camera");
	CSP_LOG(CSP_APP, CSP_INFO, "Use 'a' and 'y' keys to adjust camera speed\n");
	CSP_LOG(CSP_APP, CSP_INFO, "Use '1' and '2' keys to adjust splash rotation speed");
	CSP_LOG(CSP_APP, CSP_INFO, "Loading Terrain");


//	m_pTerrainEngine->LoadTerrain(argv[1]);
	m_pTerrainEngine->LoadTerrain( AppInit->CommandLine );

	CSP_LOG(CSP_APP, CSP_INFO, "Setting Up Sky");
	
	m_pTerrainEngine->SetupSky();

	CSP_LOG(CSP_APP, CSP_INFO, "Setting up Water");

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


  return rtnval;
}

void TerrainApp::Uninitialize()
{
	CSP_LOG(CSP_APP, CSP_TRACE, "TerrainApp::Uninitialize()");

	BaseApp::Uninitialize();

	// do TerrainApp Uninitialize here
}

void TerrainApp::OnRender()
{

	CSP_LOG(CSP_APP, CSP_BULK, "TerrainApp::OnRender()");

    p_3D->BeginScene();
    p_3D->Clear();

	glViewport(0, 0, 640, 480);
	glClearColor(0.0, 0.0, 1.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, m_iWindowWidth/m_iWindowHeight, 1.0f, 5000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();



	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	m_pTerrainEngine->SetPosition(m_vPos);
	m_pTerrainEngine->SetRotation(m_rotx, m_roty);
	gluLookAt(0, 0, 0, 0, 1, 0, 0, 0, 1);
	
	//m_Texture.Use();
	m_pTerrainEngine->SetDrawingStyle(m_ucDrawingStyle);
	m_pTerrainEngine->Render();
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
	glBindTexture(GL_TEXTURE_2D, m_hGLSplashTex);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTranslatef(0.55f, 0.8f, 0.0f);
	glScalef(0.15f, 0.15f, 0.15f);
	glRotatef(3, 1, 0, 0);
	glRotatef((m_fSplashAngle += m_fSplashRotSpeed), 0, 2, 0);
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
	if (m_fSplashAngle>360) m_fSplashAngle -= 360;

	glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	if(m_iShowStats)
	{
		/*glLoadIdentity();
		glTranslatef(-0.9, 0, 0);
		glScalef(0.03, 0.04, 0.04);*/
		//gpOGLText->Print(0, 0, "fuck, warum läuft die Scheisse nicht?");
		OGLText.Print(0, 0, "Current Speed %.1f", m_fSpeed);
		OGLText.Print(0, 1, "FPS: %.2f",p_fFPS); 
		OGLText.Print(0, 2, "X: %.2f, Y: %.2f, Z: %.2f", m_vPos.x, m_vPos.y, m_vPos.z);
		OGLText.Print(0, 3, "Current Rotation Specs: x: %.2f, y: %.2f", m_rotx, m_roty);
		OGLText.Print(0, 4, "NoTriangles: %d", m_pTerrainEngine->GetNoRenderedTriangles());
	}

  p_3D->EndScene();
  p_3D->Flip();


  return;
}

void TerrainApp::OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy)
{
 //---
  CSP_LOG(CSP_APP, CSP_TRACE, "TerrainApp::OnMovement()");

	if (key['a'])
		m_fSpeed += 0.3;

	if (key[30])
		m_fSpeed += 0.3;

	if (key['y'])
		m_fSpeed -= 0.3;

	if (key['s'])
		m_iShowStats = 1-m_iShowStats;
//	case 'x':
//		{
//			switch (eManager->m_ucDrawingStyle)
//			{
//				case TE_DS_TEXTURED:
//					eManager->m_ucDrawingStyle = TE_DS_SMOOTH;
//					break;
//				case TE_DS_SMOOTH:
//					eManager->m_ucDrawingStyle = TE_DS_WIREFRAME;
//					break;
//				case TE_DS_WIREFRAME:
//					default:
//					eManager->m_ucDrawingStyle = TE_DS_TEXTURED;
//			}
//			break;
//		}
	if (key['1'])
		m_fSplashRotSpeed -= 0.1;

	if (key['2'])
		m_fSplashRotSpeed += 0.1;
	
	if (key['q'])
	{
//		delete eManager;
		exit(0);
	}


  return;
}

void TerrainApp::OnNetwork()
{
  CSP_LOG(CSP_APP, CSP_TRACE, "TerrainApp::OnMovement()");

  return;
}

void TerrainApp::On2D(char *InputLine)
{

  CSP_LOG(CSP_APP, CSP_TRACE, "TerrainApp::On2D()");

  //  if (stricmp(InputLine, "config") == 0)
//  {
//    p_Framework->GetTextWindow()->PrintText("Not implemented.\n");
//  }
  
  return;
}

void TerrainApp::OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy, _GUI * GUI)
{
  CSP_LOG(CSP_APP, CSP_TRACE, "TerrainApp::OnGUI()");
  if (key->key == 'a')
  {

  }
  if (key->key == p_Input->MapKey('0'))
  {

  }
  return;
}

