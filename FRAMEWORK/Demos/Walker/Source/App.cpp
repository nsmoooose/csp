#include "App.h"

// ------ StandardApp Constructors/Deconstructors

App::App()
{

  fprintf(stdout, "App::App()\n");

  p_bPaused = false;
  p_Framework = 0;

  p_Model = 0;
  p_Framework = 0;
  p_GUI = 0;
  p_3D = 0;
  p_Audio = 0;
  p_Camera = 0;
  p_TextureGrass = 0;
  p_Material = 0;

  p_Init = 0;

}

App::~App()
{
  Uninitialize();
}

// ------ StandardApp Publics

short App::Initialize(_APP_INIT_STRUCT *AppInit)
{

  fprintf(stdout, "App::Initialize\n");

  short result = _A_OK;

  memcpy(&p_AppInit, AppInit, sizeof(p_AppInit));

  // ------- Create Objects

  p_Factory = new StandardFactory();

  p_Factory->Initialize();

  p_Factory->CreateFramework(&p_Framework);

  // ------ Lets Rock
  result = p_Framework->Initialize();

  if (result == _A_OK)
  {

    p_Factory->CreateWindowText(&p_WindowText);
    
    p_WindowText->Initialize(this, p_Framework);

    p_WindowText->PrintText("Initializing...\n");
    
    p_Factory->CreateWindow3D(&p_Window3D);

    p_Init = new _GRAPHICS_INIT_STRUCT;

    p_Init->Depth = 16;
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

      p_Audio->Initialize(this, p_Init->Handle);
      p_Input->Initialize(p_Framework, p_Init->Handle, p_AppInit.Instance);
      p_3D->Initialize(this, p_Init);

	    p_Camera  = p_3D->CreateCamera();

	    p_Camera->SetFOVPerspectiveView(90, 640, 480, 0.5, 10000); 
      p_Camera->SetCameraIdentity();

      p_TextureGrass = p_3D->CreateTexture();
      p_TextureGrass->Initialize( "grass.bmp", 1);

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

  p_WindowText->Uninitialize();

  p_Window3D->Show();

  return result;
}

void App::Run()
{

  fprintf(stdout, "App::Run()\n");
   
  p_Factory->CreateGUI(&p_GUI);

  p_GUI->Initialize(this, p_Window3D, p_Framework, p_Input, p_3D);

  p_GUI->Run();

  p_GUI->Uninitialize();

  return;
}

void App::Uninitialize()
{

  if(p_Init != 0)
  {
    delete p_Init;
    p_Init = 0;
  }

  if(p_GUI != 0)
  {
    p_GUI->Uninitialize();

    delete p_GUI;
    p_GUI = 0;
  }

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
 
  if(p_TextureGrass != 0)
  {
    p_TextureGrass->Uninitialize();

    delete p_TextureGrass;
    p_TextureGrass = 0;
  }

  if(p_3D != 0)
  {
    p_3D->Uninitialize();

    delete p_3D;
    p_3D = 0;
  }

  if(p_Window3D != 0)
  {
    p_Window3D->Uninitialize();

    delete p_Window3D;
    p_Window3D = 0;
  }

  if(p_Framework != 0)
  {
    p_Framework->Uninitialize();

    delete p_Framework;
    p_Framework = 0;
  }

  if(p_Factory != 0)
  {
    p_Factory->Uninitialize();

    delete p_Factory;
    p_Factory = 0;
  }

  return;
}

void App::OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy)
{
  fprintf(stdout, "App::OnMovement()\n");
  return;
}

void App::OnStartCycle()
{
  fprintf(stdout, "App::OnStartCycle()\n");
  stim = p_Framework->GetTime();
}

void App::OnEndCycle()
{

  fprintf(stdout, "App::OnEndCycle()\n");
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

void App::OnRender()
{

  fprintf(stdout, "App::OnRenderer()\n");

  unsigned long TPF = 0;

  _Rect rect;
  StandardColor color;

  color.r = 255;
  color.g = 255;
  color.b = 255;
  color.a = 255;

  rect.x1 = 0;
  rect.y1 = 0;
  rect.x2 = 1;
  rect.y2 = 1;

  char ach[128];


  p_3D->BeginScene();
  p_3D->Clear();

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

  // END
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

  //if (stricmp(InputLine, "config") == 0)
  //{
  //  p_WindowText->PrintText("Not implemented.\n");
  //}
  
  return;
}

void App::OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy,_GUI * gui)
{
  return;
}

void App::RequestCommands()
{
  //p_WindowText->PrintText("config           - This will launch the config screen.\n");

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

