#ifndef __SDLWINDOW3D_H__
#define __SDLWINDOW3D_H__

#include "Framework.h"

class StandardWindow3D;

class SDLWindow3D : public StandardWindow3D
{

private:

  // functions
  //  static LRESULT CALLBACK WindowProc(HWND win, UINT msg, WPARAM wparam, LPARAM lparam);

  // members
  StandardApp       *p_App;
  _APP_INIT_STRUCT  *p_AppInit;
  StandardFramework *p_Framework;
  
  bool               p_bFocused;
  short              p_MouseX;
  short              p_MouseY;
  _GRAPHICS_INIT_STRUCT ** p_Init;

public:

  SDLWindow3D();
  ~SDLWindow3D();

  // functions
  short           Initialize(StandardApp *App, StandardFramework *Framework,_GRAPHICS_INIT_STRUCT **Init);
  void            Uninitialize();

  void            HandleSystemMessages();
  //_HANDLE         GetWindowHandle();
  bool            GetFocused();
  void            Show();
  _Point          GetMouse();
  void            SetMouse(short x, short y);
  void            SetWindowSize(short x, short y);
};


#endif