#include "Framework.h"

class App : public BaseApp
{

private:

  // standard objects

    // members
  _APP_INIT_STRUCT           p_AppInit;
  _GRAPHICS_INIT_STRUCT     *p_Init;

public:

  App();
  ~App();

  // StandardApp
  virtual short       Initialize(_APP_INIT_STRUCT *Init, _FRAMEWORK_INIT_STRUCT * FrameworkInit);
  virtual void        Uninitialize();

  virtual void        Run();

  void                RequestCommands();
  _APP_INIT_STRUCT*   GetInit();

  virtual void        OnStartCycle();
  virtual void        OnEndCycle();
  virtual void        OnRender();
  virtual void        OnNetwork();
  virtual void        OnConsoleText(char*InputLine);
  virtual void        OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy,
                            _GUI *GUI);
  virtual void        OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy);

};

