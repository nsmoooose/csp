#include "App.h"

// ------ StandardApp Constructors/Deconstructors

App myApp;

App::App()
{

}

App::~App()
{
  Uninitialize();
}

// ------ StandardApp Publics

short App::Initialize(_APP_INIT_STRUCT *AppInit, _FRAMEWORK_INIT_STRUCT *FrameworkInit)
{

  // declarations
  short             result = _A_OK;
  char buffer[256];

  FrameworkInit->TextWindowFlag = true;

  result = BaseApp::Initialize(AppInit, FrameworkInit);
  GetWindowText()->PrintText("Hello World\n");
  GetWindowText()->PrintText("<Press Return>\n");
  GetWindowText()->ReadText(buffer);
  
  return result;
}


void App::OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy)
{

  return;
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
  
  if(stricmp(InputLine, "demo") == 0)
  {
      GetWindowText()->PrintText("\nThis is a demo!\n\n");
  }

  return;
}

void App::OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy,
                _GUI *GUI)
{

  return;
}

void App::OnStartCycle()
{

    BaseApp::OnStartCycle();
}

void App::OnEndCycle()
{

    BaseApp::OnEndCycle();
}



void App::Uninitialize()
{


  return;
}
