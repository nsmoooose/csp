#ifdef WIN32

#include "WindowsPlatform.h"

// ------------------------------------------
//  Constructor / Deconstructor
// ------------------------------------------
WindowsPlatform::WindowsPlatform()
{

  VersionNumber = 0;

  timeBeginPeriod(1);

  shutdown = CreateEvent(NULL, TRUE, FALSE, NULL);

  globals = 0;

}

WindowsPlatform::~WindowsPlatform()
{

  DeleteAppWindow();

  timeEndPeriod(1);

  _DEL(VersionNumber);

  CloseHandle(shutdown);

}


// -------------------------------------------
// STANDARD FRAMEWORK
// -------------------------------------------
void  WindowsPlatform::SetSetup(void *setup)
{
  globals = (_STANDARD_PLATFORM *)setup;
}

short WindowsPlatform::GetDependencyCount()
{
  return 0;
}

void  WindowsPlatform::GetDependencyDescription(short Index, _DEPENDENCY *Dependency)
{
  memset(Dependency, 0, sizeof(_DEPENDENCY));
}

short WindowsPlatform::SetDependencyData(short Index, void* Data)
{
  switch(Index)
  {
    case 0:
      return _NO_DEPENDENCY_TO_SET;
      break;
    default:
      return _NO_DEPENDENCY_TO_SET;
      break;
  }
  return 0;
}

void  WindowsPlatform::GetSetup(void*setup)
{
  setup = globals;
}

short WindowsPlatform::CheckStatus()
{
  if(globals == 0)
  {
    return _SETUP_NOT_SET;
  }
  return 0;
}

// ----------------------------------------
// GetPlatformVersion
// Gets the version number of the platform (windows)
// -----------------------------------------
char* WindowsPlatform::GetPlatformVersion()
{

  char temp[50];

  // get platform name and version

  OSVERSIONINFO info;
  DWORD result;

  if(VersionNumber == 0)
  {
    memset(&info, 0, sizeof(info));

    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    result = GetVersionEx(&info);
    if(result != 0)
    {
      memset(temp, 0, sizeof(VersionNumber));
      // UGH. just hope this sprintf isn't bigger than 50 characters. ;)
      sprintf(temp, "%d.%d", info.dwMajorVersion, info.dwMinorVersion);
      VersionNumber = new char[strlen(temp) + 1];
      memset(VersionNumber, 0, strlen(temp) + 1);
      strcpy(VersionNumber, temp);
    }
  }

  return VersionNumber;
}

// ------------------------------------------
// CreateAppWindow
// Creates the initial window
// -------------------------------------------
short WindowsPlatform::CreateAppWindow()
{
  
  WNDCLASS wc;

  // Make Windows
  memset(&wc, 0, sizeof(wc));

  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = sizeof(DWORD);
  wc.hInstance = (HINSTANCE)globals->this_inst;
  wc.hIcon = (HICON)globals->icon;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszClassName = globals->exename;
  wc.lpszMenuName = "AppMenu";

  if (RegisterClass(&wc) == 0)
  {
    return _WINDOW_REGISTER_FAILED;
  }

  globals->display.window = CreateWindow(globals->exename,            
                      globals->displayname,              
                      WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME,                   
                      0,           
                      0,             
                      (int)globals->display.width,                       
                      (int)globals->display.height,                  
                      NULL,                      
                      NULL,                      
                      (HINSTANCE)globals->this_inst,                  
                      NULL);

  if (globals->display.window == 0)
  {
    return _WINDOW_CREATION_FAILED;
  }

  // give it time to paint, etc.
  Sleep(500);

  RECT rect;

  GetClientRect((HWND)globals->display.window, &rect);

  globals->display.clientwidth = (short)rect.right;
  globals->display.clientheight = (short)rect.bottom;

  SetWindowLong((HWND)globals->display.window, GWL_USERDATA, (long)this);

  // Show window and paint it.
  ShowWindow((HWND)globals->display.window, globals->cmdshow);
  UpdateWindow((HWND)globals->display.window);

  return 0;
};

// ------------------------------------------------------------
// WindowProc
// Check to see what OS system messages have come through.
// ------------------------------------------------------------
LRESULT CALLBACK WindowsPlatform::WindowProc(HWND win, UINT msg, WPARAM wparam, LPARAM lparam)
{

  WindowsPlatform *mysub = (WindowsPlatform *)GetWindowLong(win,   GWL_USERDATA);

  LRESULT result;

  switch(msg)
  {
    case WM_DESTROY:
      PostQuitMessage( 0 );
      if(mysub)
      {
        mysub->quit = TRUE;
      }
      return 0;


    case WM_ACTIVATE:
      float fActive;
      fActive = LOWORD(wparam);           // activation flag
      if (mysub)
      {
        if (fActive == WA_INACTIVE)
        {
          mysub->regainedfocus = FALSE;
        }
        else
        {
          mysub->regainedfocus = TRUE;
        }
      }
      break;

	case WM_SIZE:
		{
			int fwSizeType = wparam;      // resizing flag 
			mysub->resize = TRUE;         // set resize flag;
			int nWidth = LOWORD(lparam);  // width of client area 
			int nHeight = HIWORD(lparam); // height of client area 
		}
		break;

    default:
      result = DefWindowProc(win, msg, wparam, lparam);
      break;
  }
  return result;
}

// ------------------------------------------
// DestroyAppWindow
// Destroy the initial window
// -------------------------------------------
void WindowsPlatform::DeleteAppWindow()
{

  DestroyWindow((HWND)globals->display.window);

  UnregisterClass(globals->exename, (HINSTANCE)globals->this_inst);

  return;
}

// ------------------------------------------
// GetWindowHandle
// Destroy the initial window
// -------------------------------------------
void* WindowsPlatform::GetWindowHandle()
{
  return globals->display.window;
}

// ------------------------------------------
// GetShutdown
// Returns the handle for the shutdown event.
// -------------------------------------------
inline void* WindowsPlatform::GetShutdown()
{
  return shutdown;
}

// ------------------------------------------
// TriggerShutdown
// The APP needs to tell the subsystems when
// its shutting down. This is the event to do it.
// -------------------------------------------
void WindowsPlatform::TriggerShutdown()
{
  SetEvent(shutdown);
  return;
}

// ------------------------------------------
// HandleSystemMessages
// Windows sends messages and the queue grows
// so we must handle the system messages to remove them.
// Since the WindowsProc does nothing, the cue is flushed
// with minimal overhead.
// -------------------------------------------
void WindowsPlatform::HandleSystemMessages()
{

  MSG     msg;
  bool bGot = FALSE;

  bGot = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

  if (bGot == TRUE)
  {
    // DISPATCH (call the win proc to handle messages)
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return;
}

// ------------------------------------------
// GetTime
// This returns the time for FPS calculations
// ------------------------------------------
inline unsigned long WindowsPlatform::GetTime()
{
  return timeGetTime();
}

// ------------------------------------------
// GetRegainedFocus
// This checks to see if the application got focus
// again.
// -------------------------------------------
inline short WindowsPlatform::GetRegainedFocus()
{
  return regainedfocus;
}

// ------------------------------------------
// GetQuit
// This checks to see if the application got a system quit message.
// -------------------------------------------
inline short WindowsPlatform::GetQuit()
{
  return quit;
}

// ------------------------------------------
// Nap
// Wait n milliseconds
// -------------------------------------------
inline void WindowsPlatform::Nap(short milliseconds)
{
  Sleep(milliseconds);
}

// -------
// Create the 3D Device
// -------
short WindowsPlatform::CreateRenderingDevice(StandardGraphics **display)
{

  short                 dependencycount;
  short                 count;
  _DEPENDENCY           dependency;

  FactoryGraphics *GraphicsFactory = 0;

  // -----
  // GRAPHICAL API
  // -----
  GraphicsFactory = new FactoryGraphics;
  
  Error.sError = GraphicsFactory->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  // we already know which API they want from startup dialog.
  GraphicsFactory->Create(globals->display.ID, display);

  _DEL(GraphicsFactory);

  // ask object for dependency list
  dependencycount = (*display)->GetDependencyCount();
  if(dependencycount > 0)
  {
    // check which depedency we need.
    for(count = 0; count < dependencycount; count++)
    {
      (*display)->GetDependencyDescription(count, &dependency);
      switch(dependency.ID)
      {
        case _PLATFORM_DEPENDENCY:
          (*display)->SetDependencyData(dependency.ID, this);
          break;
      }
    }
  }

  (*display)->SetSetup(&globals->display);

  Error.sError = (*display)->CheckStatus();
  if(Error.sError != _A_OK)
  {
    return Error.sError;
  }

  return _A_OK;

}

void WindowsPlatform::DeleteRenderingDevice(StandardGraphics *display)
{
  _DEL(display);
}

void WindowsPlatform::Flip()
{

}

#endif