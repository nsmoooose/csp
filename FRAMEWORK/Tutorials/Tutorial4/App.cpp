#include "App.h"

// ------ StandardApp Constructors/Deconstructors

App::App()
{
  p_Factory = 0;
  p_Framework = 0;

  p_3D = 0;
  p_Window3D = 0;
  p_WindowText = 0;
  p_Input = 0;
  p_GUI = 0;

}

App::~App()
{
  Uninitialize();
}

// ------ StandardApp Publics

short App::Initialize(_APP_INIT_STRUCT *AppInit)
{

  // declarations
  short             result = _A_OK;
  StandardColor     Color;

  // ------- wipe struct

  memcpy(&p_AppInit, AppInit, sizeof(p_AppInit));

  // ------- Create Objects

  p_Factory = new StandardFactory();

  p_Factory->Initialize();

  p_Factory->CreateFramework(&p_Framework);

  // ------ Initialize
  result = p_Framework->Initialize();

  if (result == _A_OK)
  {

    // Text Window
    p_Factory->CreateWindowText(&p_WindowText);
    
    result = p_WindowText->Initialize(this, p_Framework);
    if(result != _A_OK)
    {
        // return because we can't display error in WindowText.
        return result;
    }

    p_WindowText->PrintText("Initializing\n");

    // Window

    p_WindowText->PrintText("    Window.....");

    p_Factory->CreateWindow3D(&p_Window3D);

    p_Init = new _GRAPHICS_INIT_STRUCT;

    p_Init->Depth = 16;
    p_Init->Width = 800;
    p_Init->Height = 600;
    p_Init->Fullscreen = false;
    p_Init->HardwardTL = false;
    p_Init->VSync = false;
    p_Init->Wireframe = false;
    p_Init->NearZ = 1.0f;
    p_Init->FarZ =  1000.0f;
    p_Init->MipLevels = 1;

    result = p_Window3D->Initialize(this, p_Framework, &p_Init);

    if(result != _A_OK)
    {
       goto EARLY_EXIT;
    }
  
    p_WindowText->PrintText("OK\n");

    // Graphics
    p_WindowText->PrintText("    Graphics...");

    p_Factory->CreateGraphics(&p_3D);

    result = p_3D->Initialize(this, p_Init);
    if(result != _A_OK)
    {
      goto EARLY_EXIT;
    }

    p_WindowText->PrintText("OK\n");

    // input
    p_WindowText->PrintText("    Input......");

    p_Factory->CreateInput(&p_Input);

    result = p_Input->Initialize(p_Framework, p_Init->Handle, p_AppInit.Instance);
    if(result != _A_OK)
    {
      goto EARLY_EXIT;
    }

    p_WindowText->PrintText("OK\n");

    // GUI
    p_WindowText->PrintText("    GUI........");

    p_Factory->CreateGUI(&p_GUI);

    result = p_GUI->Initialize(this, p_Window3D, p_Framework, p_Input, p_3D);
    if (result != _A_OK)
    {
      goto EARLY_EXIT;
    }

    p_WindowText->PrintText("OK\n");

EARLY_EXIT:

    if(result != _A_OK)
    {
      char ach[256];
      sprintf(ach, "Failed! Error [%d]\n", result);

      p_WindowText->PrintText(ach);
      p_WindowText->Run();
    }
  }

  return result;
}

void App::Run()
{

  p_WindowText->PrintText("Run\n");

  // Show the 3D window
  p_WindowText->PrintText("    Show Window......");
  p_Window3D->Show();
  p_WindowText->PrintText("OK\n");

  // RUN
  p_WindowText->PrintText("    Run GUI..........");
  p_GUI->Run();
  p_WindowText->PrintText("OK\n");

  return;
}

void App::OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy)
{

  return;
}

void App::OnStartCycle()
{

}

void App::OnEndCycle()
{

}

void App::OnRender()
{

  return;
}

void App::OnNetwork()
{

  return;
}

void App::On2D(char *InputLine)
{

  return;
}

void App::OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy,
                _GUI *GUI)
{

  return;
}

void App::RequestCommands()
{

  return;
}

_APP_INIT_STRUCT* App::GetInit()
{
  return &p_AppInit;
}

void App::Uninitialize()
{

  if(p_Input != 0)
  {
    p_Input->Uninitialize();
    delete p_Input;
    p_Input = 0;
  }

  if(p_GUI != 0)
  {
    p_GUI->Uninitialize();

    delete p_GUI;
    p_GUI = 0;
  }

  if(p_WindowText != 0)
  {
    p_WindowText->Uninitialize();
    delete p_WindowText;
    p_WindowText = 0;
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

  if(p_Init != 0)
  {
    delete p_Init;
    p_Init = 0;
  }

  return;
}
