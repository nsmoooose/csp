
#include "Framework.h"

//#ifdef WIN32
//#include <windows.h>
//#endif

//#include "App.h"


int main(int argc, char *argv[])
{

  ofstream logfile("LOG.OUT");
  csplog().setLogLevels( CSP_ALL, CSP_DEBUG );
  csplog().set_output (logfile);

  CSP_LOG( CSP_APP , CSP_INFO,  "Starting TerrainFrameworkDemo..." ); 

  _APP_INIT_STRUCT    AppInit;
  _FRAMEWORK_INIT_STRUCT FrameworkInit;

  short               result;

  AppInit.PreviousInstance = false;
  AppInit.Instance = 0;
  AppInit.Icon = 0;
  AppInit.Show = 0;
  AppInit.LongName = "Framework Sample Program";
  AppInit.ShortName = "Framework11";

  FrameworkInit.GraphicsFlag = false;
  FrameworkInit.InputFlag = false;
  FrameworkInit.NetworkingFlag = false;
  FrameworkInit.TextWindowFlag = false;
  FrameworkInit.GUIFlag = false;
  FrameworkInit.LoggingFlag = false;
  FrameworkInit.AudioFlag = false;


  result = BaseApp::s_App->Initialize(&AppInit, &FrameworkInit);
 
  if(result == _A_OK)
  {
	BaseApp::s_App->Run();
  }

  BaseApp::s_App->Uninitialize();

  csplog().set_output (cerr);
  logfile.close();

  return 0;
}

#ifdef WIN32
int PASCAL WinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{
  ofstream logfile("LOG.OUT");
  csplog().setLogLevels( CSP_ALL, CSP_DEBUG );
  csplog().set_output (logfile);

  CSP_LOG( CSP_APP , CSP_INFO,  "Starting TerrainFrameworkDemo..." ); 

  _APP_INIT_STRUCT    AppInit;
  _FRAMEWORK_INIT_STRUCT FrameworkInit;
  short               result;

  // fill out init struct
  AppInit.CommandLine = cmdline;
  if(prev_inst != 0)
  {
    AppInit.PreviousInstance = true;
  }
  else
  {
    AppInit.PreviousInstance = false;
  }
  AppInit.Instance = this_inst;
  AppInit.Icon = 0;
  AppInit.Show = (void *)cmdshow;
  AppInit.LongName = "Framework Sample Program";
  AppInit.ShortName = "Framework11";

  FrameworkInit.GraphicsFlag = false;
  FrameworkInit.InputFlag = false;
  FrameworkInit.NetworkingFlag = false;
  FrameworkInit.TextWindowFlag = false;
  FrameworkInit.GUIFlag = false;
  FrameworkInit.LoggingFlag = false;
  FrameworkInit.AudioFlag = false;

  // run the app

  result = BaseApp::s_App->Initialize(&AppInit, &FrameworkInit);
  if(result == _A_OK)
  {
    BaseApp::s_App->Run();	
  }
  BaseApp::s_App->Uninitialize();

  csplog().set_output (cerr);
  logfile.close();

  return 0;
}


#endif

