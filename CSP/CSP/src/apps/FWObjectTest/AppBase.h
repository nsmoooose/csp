#ifndef __APPBASE_H_
#define __APPBASE_H_

#include "app/StandardApp.h"

class AppBase : public StandardApp
{

protected:

  FrameworkError        Error;
  StandardGraphics      *display;
  StandardPlatform      *platform;
  StandardInput         *input;
  StandardNetwork       *network;
  StandardAudio         *audio;
  StandardMath          *math;

  _APP_INIT_STRUCT      *globals;
  _INPUT_INIT_STRUCT    input_init_struct;
  _AUDIO_INIT_STRUCT    audio_init_struct;
  short                 dependencycount;
  _DEPENDENCY           dependency;

  // graphics

  // 3d
  _Matrix MatrixWorld;
  _Matrix MatrixFOV;
  _Matrix MatrixCamera;
  _Matrix MatrixTemp;
  _Matrix matHead, matBody, matPlane;

  //app
  bool appQuit;
  bool appInit;
  bool appMessageWaiting;
  bool appHasFocus;
  bool appPaused;
  bool appCanFlip;

  //game
  short fps;
  float plusplus;

  //input
  _KEY_STRUCT key;
  _MOUSE_STRUCT mouse;
  _JOYSTICK_STRUCT joystick;

  // sound

  // internal functions
  virtual void Movement(void);
  virtual void Render(void);
  virtual void ProcessInputMessage(void);

public:

  AppBase();
  ~AppBase();

  // Standard Framework - required for every object
  virtual void  SetSetup(void *setup);
  virtual short GetDependencyCount();
  virtual void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
  virtual short SetDependencyData(short Index, void* Data);
  virtual void  GetSetup(void *setup);
  virtual short CheckStatus();

  // Standard App       - required for app
  virtual short CreateObjects(void);
  virtual short Initialize(void);
  virtual void  Run(void);
  virtual void  Uninitialize();
  virtual void  DeleteObjects(void);

  inline StandardGraphics * GetDisplay()
		  { return display; }
};

#endif


