#include "Framework.h"

#ifdef WIN32
//#include "stdafx.h"
#endif

BaseApp * BaseApp::s_App;
unsigned int g_totaltim;

// ------ StandardApp Constructors/Deconstructors

BaseApp::BaseApp()
{

  CSP_LOG( CSP_APP, CSP_TRACE, "BaseApp::BaseApp() - ctr");

  BaseApp::s_App = this;	
  
  p_bPaused = false;
  p_Framework = 0;
  p_3D = 0;
  mouse_x = 0;
  mouse_y = 0;

  p_Factory = 0;
  p_WindowText = 0;
  p_Window3D = 0;
  p_GUI = 0;
  p_Audio = 0;

  p_AppInit = NULL;
  p_GraphicsInit = NULL;
  p_FrameworkInit = NULL;

  g_totaltim = 0;

}

BaseApp::~BaseApp()
{

  CSP_LOG(CSP_APP, CSP_TRACE, "BaseApp::~BaseApp()");

  Uninitialize();
}

// ------ StandardBaseApp Publics

short BaseApp::Initialize(_APP_INIT_STRUCT *AppInit, 
                          _FRAMEWORK_INIT_STRUCT * FrameworkInit)
{

  CSP_LOG( CSP_APP, CSP_TRACE,  "BaseApp::Initialize()" ); 

  short result = _A_OK;

  p_AppInit = new _APP_INIT_STRUCT(AppInit);
  memcpy(p_AppInit, AppInit, sizeof(p_AppInit));

  p_FrameworkInit = new _FRAMEWORK_INIT_STRUCT;
  memcpy(p_FrameworkInit, FrameworkInit, sizeof(p_FrameworkInit));

  p_Factory = new StandardFactory();

  p_Factory->Initialize();

  p_Factory->CreateFramework(&p_Framework);

  result = p_Framework->Initialize();

  if (result == _A_OK)
  {
    if (FrameworkInit->TextWindowFlag)
    {
        p_Factory->CreateWindowText(&p_WindowText); 
        p_WindowText->Initialize(this, p_Framework);
        p_WindowText->PrintText("Initializing...\n");
    }

    if (FrameworkInit->GraphicsFlag)
    {
        p_Factory->CreateWindow3D(&p_Window3D);

        p_GraphicsInit = new _GRAPHICS_INIT_STRUCT;

        p_GraphicsInit->Depth = 32;
        p_GraphicsInit->ZBuffDepth = 24;
        p_GraphicsInit->Width = 800;
        p_GraphicsInit->Height = 600;
        p_GraphicsInit->Fullscreen = false;
        p_GraphicsInit->HardwardTL = true;
        p_GraphicsInit->VSync = false;
        p_GraphicsInit->Wireframe = false;
        p_GraphicsInit->NearZ = 0.5f;
        p_GraphicsInit->FarZ = 25000.0f;
	    p_GraphicsInit->MipLevels = 5;

        result = p_Window3D->Initialize(this, p_Framework, &p_GraphicsInit);

        if(result == _A_OK)
        {
          p_Factory->CreateGraphics(&p_3D);
          p_Factory->CreateInput(&p_Input);
	      
          p_Input->Initialize(p_Framework, p_GraphicsInit->Handle, p_AppInit->Instance);
          p_3D->Initialize(this, p_GraphicsInit);


        }
    }

//    m_Framework->m_TextWindow->Uninitialize();

  }

  if (FrameworkInit->GUIFlag)
  {
      p_Factory->CreateGUI(&p_GUI);
      p_GUI->Initialize(this, p_Window3D, p_Framework, p_Input, p_3D);
  }

  if (FrameworkInit->NetworkingFlag)
  {
      p_Factory->CreateNetwork(&p_Network);
      p_Network->Initialize();
  }

//  p_WindowText->Uninitialize();

  if (p_Window3D)
    p_Window3D->Show();

  return result;
}


void BaseApp::Run()
{

  CSP_LOG( CSP_APP, CSP_TRACE, "BaseApp::Run()");

  if (p_GUI)
    p_GUI->Run();

  return;
}

void BaseApp::Uninitialize()
{

  CSP_LOG( CSP_APP, CSP_TRACE, "BaseApp::Uninitialize()" );

  if (p_GUI)   
    p_GUI->Uninitialize();


  if(p_Framework != 0)
  {
    p_Framework->Uninitialize();

    delete p_Framework;
    p_Framework = 0;
  }

  return;
}



void BaseApp::OnStartCycle()
{

  CSP_LOG( CSP_APP, CSP_BULK, "BaseApp::OnStartCycle()");

  stim = p_Framework->GetTime();
}

void BaseApp::OnEndCycle()
{

  CSP_LOG( CSP_APP, CSP_BULK, "BaseApp::OnEndCycle()");

  // Calculate FPS
  unsigned int etim_prev = etim;
  etim = p_Framework->GetTime();
  g_totaltim += (etim - etim_prev);

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


void BaseApp::RequestCommands()
{
  CSP_LOG(CSP_APP, CSP_BULK, "BaseApp::RequestCommands()");

//  p_Framework->GetTextWindow()->PrintText(
//      "config           - This will launch the config screen.\n");

  return;
}


void BaseApp::OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy,
                            _GUI *GUI)
{

}

