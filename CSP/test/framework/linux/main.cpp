/* --------------------------------------------
Framework 1.032 - By Brandon Bohn
Please read the README.htm file in the README directory
before proceeding.

This file was adopted for the Linux-port of CSP.

$Id: main.cpp,v 1.1 2002/09/24 22:10:00 boddman Exp $

-----------------------------------------------*/
//#include <direct.h>
#include "stdio.h"
#include "app.h"

// -Globals
App                *mainapp;
_APP_INIT_STRUCT    init;

// -Check for license
#ifndef LICENSE
#error This program requires a license.
#endif




// --------------------------------
//  Start of Windows Specific
// --------------------------------
#ifdef WIN32

#include <windows.h>
#include "resource.h"

// ---- Call back for Dialog Box
LRESULT CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

  FactoryGraphics *GraphicsFactory;
  _GRAPHICS_DESCRIPTION *gd = 0;

  short number;
  short count;
  long index;

  switch(uMsg)
  {
    case WM_INITDIALOG:

      // Query Factory for graphical API's.
      GraphicsFactory = new FactoryGraphics;

      number = GraphicsFactory->GetCount();
      for(count = 0; count < number; count++)
      {
        GraphicsFactory->GetDescription(count, &gd);
        //if(gd->ID != _NOGRAPHICS)
        //{
          index = SendDlgItemMessage(hDlg, IDC_LISTGRAPHICSAPI, LB_ADDSTRING , 0, (long)gd->Name);
          SendDlgItemMessage(hDlg, IDC_LISTGRAPHICSAPI, LB_SETITEMDATA , index, (long)gd->ID);
          if(DEFAULT_PLATFORM == gd->ID)
          {
            SendDlgItemMessage(hDlg, IDC_LISTGRAPHICSAPI, LB_SETCURSEL, index, 0);
          }
        //}
      }

      _DEL(GraphicsFactory);

      // ---- Add resolutions to list.
      SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING , 0, (long)"640x480");
      SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING , 0, (long)"800x600");
      SendDlgItemMessage(hDlg, IDC_LIST1, LB_ADDSTRING , 0, (long)"1024x768");
      SendDlgItemMessage(hDlg, IDC_MIP5,  BM_SETCHECK,  BST_CHECKED, 0 );
    
      break;
    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case IDOK:
          index = SendDlgItemMessage(hDlg, IDC_LIST1, LB_GETCURSEL, 0, 0);
          if (index < 0)
          {
            MessageBox(hDlg, "No resolution selected", "Display Properties", MB_OK);
            break;
          }

          index = SendDlgItemMessage(hDlg, IDC_LISTGRAPHICSAPI, LB_GETCURSEL, 0, 0);
          if (index < 0)
          {
            MessageBox(hDlg, "No graphical API selected", "Display Properties", MB_OK);
            break;
          }

          // SetDisplayProperties
          // Set Resolution.
          index = SendDlgItemMessage(hDlg, IDC_LIST1, LB_GETCURSEL, 0, 0);
          switch(index)
          {
            case 0:
              init.display.width = 640;
              init.display.height = 480;
              break;
            case 1:
              init.display.width = 800;
              init.display.height = 600;
              break;
            case 2:
              init.display.width = 1024;
              init.display.height = 768;
              break;
          }

          // Set the graphical API
          index = SendDlgItemMessage(hDlg, IDC_LISTGRAPHICSAPI, LB_GETCURSEL, 0, 0);
          SendDlgItemMessage(hDlg, IDC_LISTGRAPHICSAPI, LB_GETITEMDATA, index, init.display.ID);

          // Set options.
          index = SendDlgItemMessage( hDlg, IDC_FULLSCREEN, BM_GETCHECK, 0, 0 );
          if (index ==  BST_CHECKED)
          {
            init.display.fullscreen = TRUE;
          }
          else
          {
            init.display.fullscreen = FALSE;
          }

          index = SendDlgItemMessage( hDlg, IDC_32BITCOLOR, BM_GETCHECK, 0, 0 );
          if (index ==  BST_CHECKED)
          {
            init.display.depth = 32;
          }
          else
          {
            init.display.depth = 16;
          }

          index = SendDlgItemMessage( hDlg, IDC_TEXTURE, BM_GETCHECK, 0, 0 );
          if (index ==  BST_CHECKED)
          {
            init.display.texturedepth = 32;
          }
          else
          {
            init.display.texturedepth = 16;
          }

          index = SendDlgItemMessage( hDlg, IDC_HARDWARETL, BM_GETCHECK, 0, 0 );
          if (index ==  BST_CHECKED)
          {
            init.display.hardwaretl = TRUE;
          }
          else
          {
            init.display.hardwaretl = FALSE;
          }

          index = SendDlgItemMessage( hDlg, IDC_VSYNC, BM_GETCHECK, 0, 0 );
          if (index ==  BST_CHECKED)
          {
            init.display.vsync = TRUE;
          }
          else
          {
            init.display.vsync = FALSE;
          }

          index = SendDlgItemMessage( hDlg, IDC_WIREFRAME, BM_GETCHECK, 0, 0 );
          if (index ==  BST_CHECKED)
          {
            init.display.wireframe = TRUE;
          }
          else
          {
            init.display.wireframe = FALSE;
          }

          index = SendDlgItemMessage( hDlg, IDC_MIPNONE, BM_GETCHECK, 0, 0 );
          if (index ==  BST_CHECKED)
          {
            init.display.miplevels = 1;
          }

          index = SendDlgItemMessage( hDlg, IDC_MIP5, BM_GETCHECK, 0, 0 );
          if (index ==  BST_CHECKED)
          {
            init.display.miplevels = 5;
          }

          index = SendDlgItemMessage( hDlg, IDC_MIPMAX, BM_GETCHECK, 0, 0 );
          if (index ==  BST_CHECKED)
          {
            init.display.miplevels = 0;
          }

          EndDialog(hDlg, IDOK);
          break;
        case IDCANCEL:
          EndDialog(hDlg, IDCANCEL);
          break;
      }
      break;
    default:
      return(FALSE);  
  }

  return(TRUE);
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

  // throw up dialog for display properties, have it fill out init.display struct
  if (DialogBox(this_inst, MAKEINTRESOURCE(IDD_DIALOG1), GetDesktopWindow(), (DLGPROC)DlgProc) != IDOK)
  {
    return 0;
  }

  // set app props
  init.app.name = "Combat Simulator Project";
  init.app.exename = "CSP";
  
  // create the app.
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









#ifdef LINUX

int main(int _argc, char *_argv[])
{
  FactoryGraphics    *GraphicsFactory;
  short               dependencycount;
  _DEPENDENCY         dependency;
  FrameworkError      Error;
  _GRAPHICS_DESCRIPTION *gd = 0;

  short count, number;
  memset(&init, 0, sizeof(init));

  // set os specific struct, this struct will vary on platform.
  init.platform.cmdshow = _argc;
  init.platform.cmdline = *_argv;
  init.platform.icon = 0;

  // throw up dialog for display properties, have it fill out init.display struct
  /*
  if (DialogBox(this_inst, MAKEINTRESOURCE(IDD_DIALOG1), GetDesktopWindow(), (DLGPROC)DlgProc) != IDOK)
  {
    return 0;
  }
  */
  GraphicsFactory = new FactoryGraphics;
  
  number = GraphicsFactory->GetCount();
  fprintf(stderr,"Checking renderdev: ");
  for(count = 0; count < number; count++)
      {
        GraphicsFactory->GetDescription(count, &gd);
	fprintf(stderr, " \"%s\"",gd->Name);
      }
  fprintf(stderr, "\n");
  _DEL(GraphicsFactory);

  // set app props
  init.app.name = "Combat Simulator Project";
  init.app.exename = "CSP";
  
  // create the app.
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
    fprintf(stderr, "There was a problem setting up the App.\n%s",
	    Error.ConvertErrorToString(Error.sError));
    goto EARLY_EXIT;
  }
  fprintf(stderr, "Application created.\n");

  // Its all setup so create objects.
  Error.sError = mainapp->CreateObjects();
  if(Error.sError != _A_OK)
  {
    fprintf(stderr, "There was a problem creating objects in the App.\n%s",
	    Error.ConvertErrorToString(Error.sError));
    goto EARLY_EXIT;
  }
  fprintf(stderr, "Objects initialized.\n");

  // Its all setup so initialize the game.
  Error.sError = mainapp->Initialize();
  if(Error.sError != _A_OK)
  {
    fprintf(stderr,"There was a problem initializing the App:\n%s\n",
	    Error.ConvertErrorToString(Error.sError));
    goto EARLY_EXIT;
  }
  fprintf(stderr, "Application initialized.\n");

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
