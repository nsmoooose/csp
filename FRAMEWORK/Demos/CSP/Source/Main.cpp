#ifdef WIN32
#include <windows.h>
#endif

#include "App.h"

#ifdef WIN32
int PASCAL WinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{

  _APP_INIT_STRUCT    Init;
  App                 *MyApp = 0;
  short               result;

  // fill out init struct
  Init.CommandLine = cmdline;
  if(prev_inst != 0)
  {
    Init.PreviousInstance = true;
  }
  else
  {
    Init.PreviousInstance = false;
  }
  Init.Instance = this_inst;
  Init.Icon = 0;
  Init.Show = (void *)cmdshow;
  Init.LongName = "CSP Demo 1";
  Init.ShortName = "CSP";

  // run the app
  MyApp = new App();

  result = MyApp->Initialize(&Init);
  if(result == _A_OK)
  {
    MyApp->Run();
  }
  else
  {
    if(result != _EARLY_EXIT)
    {
      char ach[256];
      sprintf(ach, "Error [%d] during App::Initialize()", result);
      MessageBox(GetDesktopWindow(), ach, Init.LongName, MB_OK);
    }
  }
  MyApp->Uninitialize();

  // delete variables.

  delete MyApp;
  MyApp = 0;

  return 0;
}
#else

int main(int argc, char *argv[])
{

 _APP_INIT_STRUCT    Init;
  App                 *MyApp = 0;
  short               result;

  Init.PreviousInstance = false;
  Init.Instance = 0;
  Init.Icon = 0;
  Init.Show = 0;
  Init.LongName = "CSP Demo 1";
  Init.ShortName = "CSP";


  // run the app
  MyApp = new App();

  result = MyApp->Initialize(&Init);
  if(result == _A_OK)
  {
    MyApp->Run();
  }
  MyApp->Uninitialize();

  // delete variables.

  delete MyApp;
  MyApp = 0;

  return 0;
}


#endif
