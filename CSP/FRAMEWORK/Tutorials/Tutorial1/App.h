#include "Framework.h"

class App : public BaseApp
{

private:


public:

  App();
  ~App();

  // StandardApp
  short               Initialize(_APP_INIT_STRUCT *Init, _FRAMEWORK_INIT_STRUCT * FrameworkInit);
  void                Uninitialize();

  void                OnRender();
  void                OnNetwork();
  void                On2D(char*InputLine);
  void                OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy,
                            _GUI *GUI);
  void                OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy);

};

