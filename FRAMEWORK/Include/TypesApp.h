#ifndef TYPESAPPTAG

  #define TYPESAPPTAG

  #include "Framework.h"

  //class StandardFramework;

  class StandardApp
  {

    private:

    public:

      virtual                       ~StandardApp() {};

      // functions
      virtual short                 Initialize(_APP_INIT_STRUCT *Init,
                                                _FRAMEWORK_INIT_STRUCT * FrameworkInit) = 0;
      virtual void                  Uninitialize() = 0;

      virtual void                  Run() = 0;
      virtual void                  RequestCommands() = 0;
      virtual _APP_INIT_STRUCT*     GetAppInit() = 0;
      virtual bool                  GetPaused() = 0;

      virtual void                  OnStartCycle() = 0;
      virtual void                  OnEndCycle() = 0;
      virtual void                  OnRender() = 0;
      virtual void                  OnNetwork() = 0;
      virtual void                  OnConsoleText(char*InputLine) = 0;
      virtual void                  OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy,
                                        _GUI *GUI) = 0;
      virtual void                  OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy) = 0;



  };

#endif
