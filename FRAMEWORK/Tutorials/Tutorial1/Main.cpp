#include <windows.h>
#include "App.h"

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
  Init.LongName = "Tutorial1";
  Init.ShortName = "Tut1";

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

