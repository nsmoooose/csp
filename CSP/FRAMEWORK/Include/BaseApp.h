#ifndef __BASEAPP_H__
#define __BASEAPP_H__

#include "Framework.h"


class BaseApp : public StandardApp
{

public:

	static BaseApp * s_App;

    inline StandardFactory     * GetFactory()     { return p_Factory; }
    inline StandardWindowText  * GetWindowText()  { return p_WindowText; }
    inline StandardWindow3D    * GetWindow3D()    { return p_Window3D; }
    inline StandardGraphics    * GetGraphics()    { return p_3D; }
    inline StandardGUI         * GetGUI()         { return p_GUI; }
    inline StandardNetwork     * GetNetwork()     { return p_Network; }
    inline StandardInput       * GetInput()       { return p_Input; }
    inline StandardAudio       * GetAudio()       { return p_Audio; }
    inline StandardFramework   * GetFramework()   { return p_Framework; }
    inline bool                  GetPaused()      { return p_bPaused; }
    inline void                  SetPaused(bool flag)      { p_bPaused = flag; }
    inline _APP_INIT_STRUCT       * GetAppInit()       { return p_AppInit; }
    inline _GRAPHICS_INIT_STRUCT  * GetGraphicsInit()  { return p_GraphicsInit; }
    inline _FRAMEWORK_INIT_STRUCT * GetFrameworkInit() { return p_FrameworkInit; }

private:
  StandardFramework     *p_Framework;
  StandardFactory       *p_Factory;
  StandardWindowText    *p_WindowText;
  StandardWindow3D      *p_Window3D;
  StandardGraphics      *p_3D;
  StandardGUI           *p_GUI;
  StandardNetwork       *p_Network;
  StandardInput         *p_Input;
  StandardAudio         *p_Audio;

  bool                p_bPaused;
  _APP_INIT_STRUCT *   p_AppInit;
  _GRAPHICS_INIT_STRUCT *p_GraphicsInit;
  _FRAMEWORK_INIT_STRUCT * p_FrameworkInit;

public:


  float                  p_fFPS;
  float                  p_fMultiplier;

  unsigned long          stim, etim, rtim;
  int mouse_x, mouse_y;


 

public:

  BaseApp();
  ~BaseApp();

  // StandardApp
  virtual short Initialize(_APP_INIT_STRUCT *Init,
                            _FRAMEWORK_INIT_STRUCT * FrameworkInit);
  virtual void  Run();
  virtual void  OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy,
                            _GUI *GUI);

  virtual void  Uninitialize();

  virtual void  OnStartCycle();
  virtual void  OnEndCycle();

  void  RequestCommands();

protected:
    void InitializeSim();


};


#endif

