#include "Framework.h"

class StandardGUI
{

  private:

    // functions
    virtual bool  PreprocessInput() = 0;
    virtual void  PreprocessKeyboard(_KEY_BUFFERED *key) = 0;
    virtual void  PreprocessJoystick(_JOYSTICK *joystick) = 0;
    virtual void  PreprocessMouse(_MOUSE *mouse) = 0;

  public:

    virtual ~StandardGUI() {};

    // functions
    virtual short                 Initialize(StandardApp *App, 
                                             StandardWindow3D *Window3D,
                                             StandardFramework *Framework,
                                             StandardInput *Input,
                                             StandardGraphics *Graphics) = 0;
    virtual void                  Uninitialize() = 0;
    virtual void                  Run() = 0;
    virtual StandardScreen*       CreateScreen() = 0;
    virtual void                  SetScreen(StandardScreen *Screen) = 0;

    virtual unsigned long         Draw() = 0;



};
