#include "App.h"

// ------ StandardApp Constructors/Deconstructors

// Must create a global Application object.
App myApp;

App::App()
{

}

App::~App()
{
  Uninitialize();
}

// ------ StandardApp Publics

short App::Initialize(_APP_INIT_STRUCT *AppInit, _FRAMEWORK_INIT_STRUCT * FrameworkInit)
{

  // declarations
  short             result = _A_OK;
  StandardColor     Color;

  
   AppInit->Width = 800;
   AppInit->Height = 600;
   AppInit->Fullscreen = false;

   FrameworkInit->GraphicsFlag = true;
   FrameworkInit->GUIFlag = true;
   FrameworkInit->InputFlag = true;
   FrameworkInit->LoggingFlag = true;

   BaseApp::Initialize(AppInit, FrameworkInit);
  
  
  return result;
}

void App::Run()
{

  p_WindowText->PrintText("Run\n");

  // Show the 3D window
  p_WindowText->PrintText("    Show Window......");
  p_Window3D->Show();
  p_WindowText->PrintText("OK\n");

  // wait before exiting.
  p_Framework->Nap(3000);

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

void App::OnConsoleText(char *InputLine)
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


  return;
}
