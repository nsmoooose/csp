#include "App.h"

// ------ StandardApp Constructors/Deconstructors

// Must Create a Application Global Object.
App myApp;

App::App()
{
}

App::~App()
{
  Uninitialize();
}

// ------ StandardApp Publics

short App::Initialize(_APP_INIT_STRUCT *AppInit,
                      _FRAMEWORK_INIT_STRUCT *FrameworkInit)
{

  // declarations
  short             result = _A_OK;

  // modify AppInit and FrameworkInit to choose customize application.

  // Must call BaseApp::Initialize to initialize the app and framework
  result = BaseApp::Initialize(AppInit, FrameworkInit);

  // Do application specific initialization


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

void App::On2D(char *InputLine)
{
  
  return;
}

void App::OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy,
                _GUI *GUI)
{

  return;
}


void App::Uninitialize()
{

  // cleanup application specific data.

  return;
}
