#include "WinWindow3D.h"

WinWindow3D::WinWindow3D()
{
  p_bFocused = true;
  p_Init = 0;
  p_App = 0;
}

WinWindow3D::~WinWindow3D()
{
  Uninitialize();
}

// ------------------------------------------------------------
// WindowProc
// Check to see what OS system messages have come through.
// ------------------------------------------------------------
LRESULT CALLBACK WinWindow3D::WindowProc(HWND win, UINT msg, WPARAM wparam, LPARAM lparam)
{

  WinWindow3D *mysub = (WinWindow3D *)GetWindowLong(win,   GWL_USERDATA);

  LRESULT result;

  switch(msg)
  {
    case WM_DESTROY:
      if(mysub)
      {
        mysub->p_Framework->TriggerShutdown();
      }
      PostQuitMessage( 0 );

      return 0;


    case WM_ACTIVATE:
      float fActive;
      fActive = LOWORD(wparam);           // activation flag
      if (mysub)
      {
        if (fActive == WA_INACTIVE)
        {
          //ShowCursor(TRUE);
          mysub->p_bFocused = false;
        }
        else
        {
          //ShowCursor(FALSE);
          mysub->p_bFocused = true;
        }
      }
      break;
    case WM_MOUSEMOVE:
      if(mysub != 0)
      {
        mysub->SetMouse(LOWORD(lparam), HIWORD(lparam));
      }
      break;
    case WM_SIZE:
      if(mysub != 0)
      {
        mysub->SetWindowSize(LOWORD(lparam),HIWORD(lparam)); 
      }
      break;
    default:
      result = DefWindowProc(win, msg, wparam, lparam);
      break;
  }
  return result;
}


short WinWindow3D::Initialize(StandardApp *App, StandardFramework *Framework, _GRAPHICS_INIT_STRUCT **Init)
{

  p_Init = Init;
  p_App = App;
  p_AppInit = p_App->GetAppInit();
  p_Framework = Framework;

  // ----------

  WNDCLASS wc;

  // Make Windows
  memset(&wc, 0, sizeof(wc));

  wc.style = CS_HREDRAW | CS_VREDRAW /* | CS_OWNDC */;
  wc.lpfnWndProc = WindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = sizeof(DWORD);
  wc.hInstance = (HINSTANCE)p_AppInit->Instance;
  wc.hIcon = (HICON)p_AppInit->Icon;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszClassName = p_AppInit->ShortName;
  wc.lpszMenuName = "AppMenu";

  if (RegisterClass(&wc) == 0)
  {
    return _WINDOW_REGISTER_FAILED;
  }

  RECT rect;

  rect.left = 0;
  rect.top = 0;
  rect.right = (*Init)->Width;
  rect.bottom = (*Init)->Height;

  if((*p_Init)->Fullscreen == false)
  {

      AdjustWindowRect(&rect, CS_HREDRAW | CS_VREDRAW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
					                WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME, FALSE);

      p_hWindow = CreateWindow(p_AppInit->ShortName,            
                      p_AppInit->LongName,  
                      //WS_POPUPWINDOW,
                      CS_HREDRAW | CS_VREDRAW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
					            WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME,                   
                      0,           
                      0,             
                      (int)(rect.right - rect.left) + 2,                       
                      (int)(rect.bottom - rect.top) + 21,  
                      // NOTE:
                        // THE + values in the 2 lines above
                        // will CHANGE depending on users
                        // display properties for titlebar size.
                        // So if they have something
                        // non standard, the window will look funny.
                      NULL,                      
                      NULL,                      
                      (HINSTANCE)p_AppInit->Instance,                  
                      NULL);
  }
  else
  {

      AdjustWindowRect(&rect, WS_POPUP, FALSE);

      p_hWindow = CreateWindow(p_AppInit->ShortName,            
                      p_AppInit->LongName,  
                      WS_POPUP,
                      //WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME,                   
                      0,           
                      0,             
                      (int)(rect.right - rect.left),                       
                      (int)(rect.bottom - rect.top),  
                      NULL,                      
                      NULL,                      
                      (HINSTANCE)p_AppInit->Instance,                  
                      NULL);
  }

  if (p_hWindow == 0)
  {
    return _WINDOW_CREATION_FAILED;
  }

  SetWindowLong(p_hWindow, GWL_USERDATA, (long)this);

  GetClientRect(p_hWindow, &rect);

  (*Init)->Handle = p_hWindow;

  return 0;
}

void WinWindow3D::Uninitialize()
{

  if (p_hWindow)
  {
	DestroyWindow(p_hWindow);
	p_hWindow = 0;
  }

  UnregisterClass(p_AppInit->ShortName, (HINSTANCE)p_AppInit->Instance);

  return;
}

void WinWindow3D::HandleSystemMessages()
{

  MSG     msg;
  BOOL bGot = FALSE;

  while(1)
  {
    bGot = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

    if (bGot == TRUE)
    {
      // DISPATCH (call the win proc to handle messages)
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {
      break;
    }
  }

  return;
}

bool  WinWindow3D::GetFocused()
{
  return p_bFocused;
}

void WinWindow3D::Show()
{
  // Show window and paint it.
  ShowWindow(p_hWindow, (int)p_AppInit->Show);
  UpdateWindow(p_hWindow);

  Sleep(500);
}


_Point WinWindow3D::GetMouse()
{
  _Point pt;

  pt.x = p_MouseX / (float)(*p_Init)->Width;
  pt.y = p_MouseY / (float)(*p_Init)->Height;

  return pt;
}

void WinWindow3D::SetMouse(short x, short y)
{
  p_MouseX = x;
  p_MouseY = y;
}

void WinWindow3D::SetWindowSize(short x, short y)
{
  if((*p_Init)->Fullscreen == false)
  {
    (*p_Init)->Width = x;
    (*p_Init)->Height = y;
  }
}