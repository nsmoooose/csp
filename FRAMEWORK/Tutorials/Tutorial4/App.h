#include "Framework.h"

class App : private StandardApp
{

private:

  // standard objects
  StandardFramework         *p_Framework;
  StandardFactory           *p_Factory;
  StandardWindowText        *p_WindowText;
  StandardWindow3D          *p_Window3D;
  StandardGraphics          *p_3D;
  StandardInput             *p_Input;
  StandardGUI               *p_GUI;

  // members
  _APP_INIT_STRUCT           p_AppInit;
  _GRAPHICS_INIT_STRUCT     *p_Init;

public:

  App();
  ~App();

  // StandardApp
  short               Initialize(_APP_INIT_STRUCT *Init);
  void                Uninitialize();

  void                Run();

  void                RequestCommands();
  _APP_INIT_STRUCT*   GetInit();

  void                OnStartCycle();
  void                OnEndCycle();
  void                OnRender();
  void                OnNetwork();
  void                On2D(char*InputLine);
  void                OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy,
                            _GUI *GUI);
  void                OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy);

};

