#ifndef __TYPESWINDOW3D_H__
#define __TYPESWINDOW3D_H__

#include "Framework.h"

class StandardApp;
class StandardFramework;

class StandardWindow3D
{

  private:

  public:

    virtual ~StandardWindow3D() {};

    // functions
    virtual short           Initialize(StandardApp *App, StandardFramework *Framework,_GRAPHICS_INIT_STRUCT **Init) = 0;
    virtual void            Uninitialize() = 0;

    virtual void            HandleSystemMessages() = 0;
    //virtual _HANDLE           GetWindowHandle() = 0;
    virtual bool            GetFocused() = 0;
    virtual void            Show() = 0;
    virtual _Point          GetMouse() = 0;
    virtual void            SetMouse(short x, short y) = 0;
    virtual void            SetWindowSize(short x, short y) = 0;

};

#endif
