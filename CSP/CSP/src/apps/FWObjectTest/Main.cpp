/* --------------------------------------------
Framework 1.032 - By Brandon Bohn
Please read the README.htm file in the README directory
before proceeding.
-----------------------------------------------*/
#ifdef WIN32

#include <windows.h>
#include "resource.h"

#endif 

#include <direct.h>
#include "stdio.h"
#include "AppBase.h"
#include "App.h"
#include "ConfigDlg.h"
#include "StatesDlg.h"

// -Globals
AppBase                *mainapp;
_APP_INIT_STRUCT    init;

// -Check for license
#ifndef LICENSE
#error This program requires a license.
#endif

// --------------------------------
//  Start of Windows Specific
// --------------------------------

#ifdef WIN32

void main()
{
	WinMain((HINSTANCE)GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOW);
}


// --- Windows Enters Here
int PASCAL WinMain (HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{

  short               dependencycount;
  _DEPENDENCY         dependency;
  FrameworkError      Error;

  short count;
  memset(&init, 0, sizeof(init));

  #ifdef _DEBUG
    // UGH. reset the directory for debug mode (only)
    // since MSVC 6.0++ resets the directory to its directory
    // in debug, we have to manually reset the directory
    // back to the right directory when in debug mode.
    _chdir("e:\\projects\\c\\csp\\");
  #endif

  // set os specific struct, this struct will vary on platform.
  init.platform.this_inst = this_inst;
  init.platform.prev_inst = prev_inst;
  init.platform.cmdline = cmdline;
  init.platform.cmdshow = cmdshow;
  init.platform.icon = 0;

  ConfigDlg dlg(this_inst, GetDesktopWindow(), &init);
  if (dlg.DoModal() != IDOK)
  {
	return 0;
  }

  StatesDlg stateDlg(this_inst, GetDesktopWindow());
  stateDlg.Show();

  // set app props
  init.app.name = "Combat Simulator Project";
  init.app.exename = "CSP";
  
  // create the app.
//  mainapp = new AppBase();
  mainapp = new App();
  // set the init struct.
  mainapp->SetSetup(&init);
  // ask object for dependency list
  dependencycount = mainapp->GetDependencyCount();
  if(dependencycount > 0)
  {
    // check which depedency we need.
    // NOTE: App has no dependency's so it should never hit this.
    for(count = 0; count < dependencycount; count++)
    {
      mainapp->GetDependencyDescription(count, &dependency);
      switch(dependency.ID)
      {
        case _PLATFORM_DEPENDENCY:
          //mainapp->SetDependencyData(dependency.ID, platform);  // example
          break;
      }
    }
  }

  // Make sure everything is ok!
  Error.sError = mainapp->CheckStatus();
  if(Error.sError != _A_OK)
  {
    sprintf(Error.String, "There was a problem setting up the App.\n%s", Error.ConvertErrorToString(Error.sError));
    MessageBox(GetDesktopWindow(), Error.String, init.app.name, MB_OK | MB_ICONSTOP);
    goto EARLY_EXIT;
  }

  // Its all setup so create objects.
  Error.sError = mainapp->CreateObjects();
  if(Error.sError != _A_OK)
  {
    sprintf(Error.String, "There was a problem creating objects in the App.\n%s", Error.ConvertErrorToString(Error.sError));
    MessageBox(GetDesktopWindow(), Error.String, init.app.name, MB_OK | MB_ICONSTOP);
    goto EARLY_EXIT;
  }

  // Its all setup so initialize the game.
  Error.sError = mainapp->Initialize();
  if(Error.sError != _A_OK)
  {
    sprintf(Error.String, "There was a problem initializing the App.\n%s", Error.ConvertErrorToString(Error.sError));
    MessageBox(GetDesktopWindow(), Error.String, init.app.name, MB_OK | MB_ICONSTOP);
    goto EARLY_EXIT;
  }

  // Run the game.
  mainapp->Run();

  // Game was quit or we had an error
EARLY_EXIT:

  // Safe cleanup.
  mainapp->Uninitialize();

  // Kill the object.
  _DEL(mainapp);

  return 0;

}

#endif
// --------------------------------
//  END! of Windows Specific
// --------------------------------