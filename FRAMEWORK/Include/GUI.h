#ifndef __GUI_H_
#define __GUI_H_

#include "Framework.h"

class GUI : public StandardGUI
{

  private:

    // functions
    bool  PreprocessInput();
    void  PreprocessKeyboard(_KEY_BUFFERED *key);
    void  PreprocessJoystick(_JOYSTICK *joystick);
    void  PreprocessMouse(_MOUSE *mouse);

    // members
    StandardApp             *p_App;
    _GRAPHICS_INIT_STRUCT   *p_Init;
    StandardFramework       *p_Framework;
    StandardInput           *p_Input;
    StandardWindow3D        *p_Window3D;
    StandardGraphics        *p_3D;

    StandardScreen          *p_ActiveScreen;

    float                    p_MouseX;
    float                    p_MouseY;

    bool                     p_MouseDown;
    _GUI                     p_MouseDGUI;
    _GUI                     p_MouseUGUI;
    _GUI                     p_NoGUI;

    bool                     p_Shift;
    bool                     p_Alt;
    bool                     p_Ctrl;

  public:

    GUI();
    ~GUI();

    // functions
    short                   Initialize(StandardApp *App,
                                       StandardWindow3D *Window3D,
                                       StandardFramework *Framework,
                                       StandardInput *Input,
                                       StandardGraphics *Graphics);
    void                    Uninitialize();
    void                    Run();

    StandardScreen*         CreateScreen();
    void                    SetScreen(StandardScreen *Screen);

    unsigned long           Draw();

};


#endif
