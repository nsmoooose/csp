#ifndef STANDARDSCREEN_H_
#define STANDARDSCREEN_H_

#include "Framework.h"

class StandardScreen
{
  private:

    StandardGraphics        *p_3D;
    _DEVICE                 *p_Device;
    _GRAPHICS_INIT_STRUCT   *p_Init;

    unsigned char            p_WindowCount;
    StandardWindow          *p_Window[50];
    StandardWindow          *p_FocusedWindow;


    StandardColor            p_BackgroundColor;
    StandardPolygonFlat     *p_Back;
    unsigned char            p_UniqueID;

    bool                     p_MouseDown;

  public:

    StandardScreen(StandardGraphics *Graphics);
    ~StandardScreen();

    short               Initialize(unsigned char UniqueID, StandardColor *ScreenColor);
    void                Uninitialize();

    StandardWindow*     CreateGUIWindow(char* WindowName, _Rect *WindowSize);

    void                Apply();

    unsigned long       Draw();
    StandardWindow*     GetWindow(unsigned char WindowNumber);
    unsigned char       GetUniqueID();

    _GUI                Process(_GUI_KEY *key, _GUI_MOUSE *mouse);

};

#endif
