#include "App.h"

// ------ StandardApp Constructors/Deconstructors

App::App()
{
  p_bPaused = false;
  p_Framework = 0;
  p_Model = 0;
  p_GUI = 0;
  p_3D = 0;
  p_Audio = 0;
  p_Camera = 0;

//  p_AppInit = 0;
}

App::~App()
{
  Uninitialize();
}

// ------ StandardApp Publics

short App::Initialize(_APP_INIT_STRUCT *AppInit)
{

  short result = _A_OK;

  memcpy(&p_AppInit, AppInit, sizeof(p_AppInit));

  p_Factory = new StandardFactory();

  p_Factory->Initialize();

  p_Factory->CreateFramework(&p_Framework);
  result = p_Framework->Initialize();

  if (result == _A_OK)
  {
    p_Factory->CreateWindowText(&p_WindowText); 

    p_WindowText->Initialize(this, p_Framework);

    p_WindowText->PrintText("Initializing...\n");

    p_Factory->CreateWindow3D(&p_Window3D);

    p_Init = new _GRAPHICS_INIT_STRUCT;

    p_Init->Depth = 32;
    p_Init->Width = 640;
    p_Init->Height = 480;
    p_Init->Fullscreen = false;
    p_Init->HardwardTL = false;
    p_Init->VSync = false;
    p_Init->Wireframe = false;
    p_Init->NearZ = 0.5f;
    p_Init->FarZ = 25000.0f;
	p_Init->MipLevels = 5;

    result = p_Window3D->Initialize(this, p_Framework, &p_Init);

    if(result == _A_OK)
    {
      p_Factory->CreateGraphics(&p_3D);
      p_Factory->CreateNetwork(&p_Network);
      p_Factory->CreateAudio(&p_Audio);
      p_Factory->CreateInput(&p_Input);
	  
	  p_Input->Initialize(p_Framework, p_Init->Handle, p_AppInit.Instance);
	  p_3D->Initialize(this, p_Init);
	  p_Camera = p_3D->CreateCamera();
	  p_Camera->SetFOVPerspectiveView(90, 640, 480, 0.5, 100); 
//	  p_Camera->SetOrthoView(640,480, -0.5, 10);
	  p_Camera->SetCameraIdentity();

      p_Viewport_leftLower = p_3D->CreateViewport();
      p_Viewport_leftLower->Initialize(0, 0, 320, 240);
	  p_Viewport_leftUpper = p_3D->CreateViewport();
	  p_Viewport_leftUpper->Initialize(0, 240, 320, 240);
      p_Viewport_rightLower = p_3D->CreateViewport();
      p_Viewport_rightLower->Initialize(320, 0, 320, 240);
	  p_Viewport_rightUpper = p_3D->CreateViewport();
	  p_Viewport_rightUpper->Initialize(320, 240, 320, 240);

	  p_Viewport_full = p_3D->CreateViewport();
	  p_Viewport_full->Initialize(0,0,640,460);

      p_Model = CreateCube(); 
	  p_TextureCinder = p_3D->CreateTexture();
	  p_TextureCinder->Initialize( "cinderblock_white.bmp", 1);
      p_TextureGrass = p_3D->CreateTexture();
	  p_TextureGrass->Initialize( "grass.bmp", 1);
      p_TextureSky = p_3D->CreateTexture();
	  p_TextureSky->Initialize( "sky.bmp", 1);
      p_TextureErde = p_3D->CreateTexture();
	  p_TextureErde->Initialize( "Texture_erde.bmp", 1);

	  p_Material = p_3D->CreateMaterial();

	  StandardColor Specular(0, 0, 0, 0);
      StandardColor Diffuse(255,0,0,255);
      StandardColor Ambient(255, 0, 0, 255);
      StandardColor Emissive(255, 0, 0, 255);

	  p_Material->Initialize(&Emissive,&Specular,&Diffuse,&Ambient,0);

	  StandardColor black(0,0,0,0);
	  p_3D->SetBackgroundColor(&black);

    }



  }

  xpos1 = 3.0; ypos1 = 3.0;
  xpos2 = 3.0; ypos2 = -3.0;
  xpos3 = -3.0; ypos3 = -3.0;
  xpos4 = -3.0; ypos4 = 3.0;

  p_WindowText->Uninitialize();
  p_Window3D->Show();

  return result;
}

void App::Run()
{
  p_Factory->CreateGUI(&p_GUI);
  p_GUI->Initialize(this, p_Window3D, p_Framework, p_Input, p_3D);

  p_GUI->Run();
  p_GUI->Uninitialize();

  return;
}

void App::Uninitialize()
{



  if(p_Model != 0)
  {
    p_Model->Uninitialize();

    delete p_Model;
    p_Model = 0;
  }

  if (p_Material != 0)
  {
	p_Material->Uninitialize();
	delete p_Material;
	p_Material=0;
  }

  if (p_TextureCinder!=0)
  {
	p_TextureCinder->Uninitialize();
	delete p_TextureCinder;
	p_TextureCinder=0;
  }

  if (p_Viewport_leftLower != 0)
  {
	  p_Viewport_leftLower->Uninitialize();
	  delete p_Viewport_leftLower;
	  p_Viewport_leftLower=0;
  }

  if (p_Viewport_leftUpper != 0)
  {
	p_Viewport_leftUpper->Uninitialize();
	delete p_Viewport_leftUpper;
	p_Viewport_leftUpper=0;
  }
  
  if (p_Viewport_rightLower != 0)
  {
	p_Viewport_rightLower->Uninitialize();
	delete p_Viewport_rightLower;
	p_Viewport_rightLower=0;
  }
  
  if (p_Viewport_rightUpper != 0)
  {
	p_Viewport_rightUpper->Uninitialize();
	delete p_Viewport_rightUpper;
	p_Viewport_rightUpper=0;
  }
  
  if (p_Viewport_full != 0)
  {
	p_Viewport_full->Uninitialize();
	delete p_Viewport_full;
	p_Viewport_full=0;
  }
 

//  if(p_Viewport != 0)
//  {
//    p_Viewport->Uninitialize();
//
//    delete p_Viewport;
//    p_Viewport = 0;
//  }

  if(p_Framework != 0)
  {
    p_Framework->Uninitialize();

    delete p_Framework;
    p_Framework = 0;
  }

  return;
}


void App::OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy)
{
  return;
}


void App::OnRender()
{

  unsigned long stim, etim, rtim;

  stim = p_Framework->GetTime();
  unsigned long TPF = 0;

  _Rect rect;
  StandardColor color(255,255,255,255);

  rect.x1 = 0;
  rect.y1 = 0;
  rect.x2 = 1;
  rect.y2 = 1;

  
  char ach[128];

//  p_Viewport->BeginScene();
//  p_Viewport->Clear();
//  p_3D->SetTexture(0);
//  p_Viewport->DrawTextF(ach, rect, 1, &color);
//  p_Viewport->EndScene();

//  p_3D->SetViewport();

  xpos1 += 0.001; xpos2 -= 0.001; ypos3 += 0.001; //ypos4 -= 0.001;

  
  p_Camera->SetPosition(0,0,-5,0,0,0,0,1,0);
  p_3D->SetCamera(p_Camera);

  p_3D->BeginScene();
  p_3D->Clear();

  StandardMatrix4 * pCameraMat = p_Camera->GetCameraMatrix();
  StandardMatrix4 * pProjMat = p_Camera->GetProjectionMatrix();

  
//  p_3D->SetViewport(p_Viewport_leftUpper);
  p_Model->SetPos(xpos1,ypos1,0);
  p_3D->SetTexture(p_TextureCinder);
  TPF += p_3D->DrawPolygonMesh(p_Model);

//  p_3D->SetViewport(p_Viewport_leftLower);
  p_Model->SetPos(xpos2,ypos2,0);
  p_3D->SetTexture(p_TextureGrass);
  TPF += p_3D->DrawPolygonMesh(p_Model);

//  p_3D->SetViewport(p_Viewport_rightUpper);
  p_Model->SetPos(xpos3,ypos3,0);
  p_3D->SetTexture(p_TextureSky);
  TPF += p_3D->DrawPolygonMesh(p_Model);

//  p_3D->SetViewport(p_Viewport_rightLower);
  p_Model->SetPos(xpos4,ypos4,0);
  p_3D->SetTexture(p_TextureErde);
  TPF += p_3D->DrawPolygonMesh(p_Model);

//  p_3D->DrawPolygonMesh(p_Terrain);

//  p_3D->SetTexture(p_Viewport->GetSurface());
  //p_3D->SetViewport(p_Viewport_full);


  // LEFT
  sprintf(ach, "TPF - %4.4d", TPF);
  p_3D->DrawTextF(ach, rect, 0, 0, &color);

  // MIDDLE
  sprintf(ach, "FPS - %4.4f", p_fFPS);
  p_3D->DrawTextF(ach, rect, 1, 0, &color);

  // RIGHT SIDE
  _Point pt;

  pt = p_Window3D->GetMouse();

  sprintf(ach, "MouseXY - %4.4f %4.4f", pt.x, pt.y);
  p_3D->DrawTextF(ach, rect, 2, 0, &color);


  p_3D->EndScene();
  p_3D->Flip();


  return;
}

void App::OnNetwork()
{

  return;
}

void App::On2D(char *InputLine)
{

//  if (stricmp(InputLine, "config") == 0)
//  {
//    p_WindowText->PrintText("Not implemented.\n");
//  }
  
  return;
}

void App::OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy,_GUI * gui)
{

  return;
}

//void App::OnInput(_KEY_STRUCT *key, _MOUSE_STRUCT *mouse, _JOYSTICK_STRUCT *joy)
//{
//
//  return;
//}

void App::RequestCommands()
{
  p_WindowText->PrintText(
      "config           - This will launch the config screen.\n");

  return;
}

_APP_INIT_STRUCT* App::GetInit()
{
  return &p_AppInit;
}

bool App::GetPaused()
{
  return p_bPaused;
}


void App::OnStartCycle()
{
  stim = p_Framework->GetTime();
}

void App::OnEndCycle()
{
  // Calculate FPS
  etim = p_Framework->GetTime();

  if(etim == stim)  etim++;
  rtim = etim - stim;
  if(rtim < 5)
  {
    p_Framework->Nap(5 - rtim);
    rtim = 5;
  }
  p_fFPS = (short)(1000 / rtim);
  if(p_fFPS == 0) p_fFPS = 1;
	p_fMultiplier = (float)(float)30 / (float)p_fFPS;

}








///////////////////////////////////////////////////////////////////////

StandardPolygonMesh * App::CreateCube()
{
   StandardPolygonMesh * pCube = p_3D->CreatePolygonMesh();
   StandardNTVertex cubeVert;

	pCube->BeginMesh(FW_TRIANGLES);

   // bottom face
   // triangle 1
   cubeVert.Initialize(-1.0f,-1.0f,-1.0f,-0.33f,-0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,-1.0f,1.0f,-0.33f,-0.33f,0.33f,1.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,-1.0f,1.0f,0.33f,-0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // triangle 2
   cubeVert.Initialize(-1.0f,-1.0f,-1.0f,-0.33f,-.033f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,-1.0f,1.0f,0.33f,-0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,-1.0f,-1.0f,0.33f,-.033f,-0.33f,0.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // top face
   // triangle 3
   cubeVert.Initialize(-1.0f,1.0f,-1.0f,-0.33f,0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,1.0f,1.0f,-0.33f,0.33f,0.33f,1.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,1.0f,0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // triangle 4
   cubeVert.Initialize(-1.0f,1.0f,-1.0f,-0.33f,0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,1.0f,0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,-1.0f,0.33f,0.33f,-0.33f,0.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // left face
   // triangle 5
   cubeVert.Initialize(-1.0f,-1.0f,-1.0f,-0.33f,-0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,1.0f,-1.0f,-0.33f,0.33f,0.33f,1.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,1.0f,1.0f,-0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // triangle 6
   cubeVert.Initialize(-1.0f,-1.0f,-1.0f,-0.33f,0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,1.0f,1.0f,0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,-1.0f,1.0f,0.33f,0.33f,-0.33f,0.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // right face
   // triangle 7
   cubeVert.Initialize(1.0f,-1.0f,-1.0f,-0.33f,-0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,-1.0f,-0.33f,0.33f,0.33f,1.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,1.0f,-0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // triangle 8
   cubeVert.Initialize(1.0f,-1.0f,-1.0f,-0.33f,0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,1.0f,0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,-1.0f,1.0f,0.33f,0.33f,-0.33f,0.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // back face
   // triangle 9
   cubeVert.Initialize(-1.0f,-1.0f,-1.0f,-0.33f,-0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,-1.0f,-1.0f,-0.33f,0.33f,0.33f,1.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,-1.0f,-0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // triangle 10
   cubeVert.Initialize(-1.0f,-1.0f,-1.0f,-0.33f,0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,-1.0f,0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,1.0f,-1.0f,0.33f,0.33f,-0.33f,0.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // front face
   // triangle 11
   cubeVert.Initialize(-1.0f,-1.0f,1.0f,-0.33f,-0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,-1.0f,1.0f,-0.33f,0.33f,0.33f,1.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,1.0f,-0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   // triangle 12
   cubeVert.Initialize(-1.0f,-1.0f,1.0f,-0.33f,0.33f,-0.33f,0.0f,0.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(1.0f,1.0f,1.0f,0.33f,0.33f,0.33f,1.0f,1.0f);
   pCube->AddVertex(cubeVert);

   cubeVert.Initialize(-1.0f,1.0f,1.0f,0.33f,0.33f,-0.33f,0.0f,1.0f);
   pCube->AddVertex(cubeVert);

   pCube->EndMesh();


   return pCube;
}


