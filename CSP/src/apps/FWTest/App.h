#include "app/StandardApp.h"

class App : private StandardApp
{

private:

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
  _VertexBuffer *Buff;
  _IndexBuffer *IndexBuff;
  
  _VertexBuffer *GroundBuff;
  _IndexBuffer *GroundIndexBuff;

  _VertexBuffer *SkyBuff;
  _IndexBuffer *SkyIndexBuff;
  _VertexBuffer *HudBuff;
  _IndexBuffer *HudIndexBuff;
  _VertexBuffer *CockpitBuff;
  _IndexBuffer *CockpitIndexBuff;

  _Light *Sun;
  _Material *Mat;
  _Material *Sky;
  _Texture *Tex;
  _Texture *Hud;
  _Material *HudMat;
  _Material *CockpitMat;

  // 3d
  _Matrix MatrixWorld;
  _Matrix MatrixFOV;
  _Matrix MatrixCamera;
  _Matrix MatrixTemp;
  _Matrix matHead, matBody, matPlane;

  //app
  BOOL appQuit;
  BOOL appInit;
  BOOL appMessageWaiting;
  BOOL appHasFocus;
  BOOL appPaused;
  BOOL appCanFlip;

  //game
  short fps;
  float plusplus;
  float sensativity;
  float planesensx, planesensy;
  float heading;
  float headingx, headingz;
  short tris;

  //input
  _KEY_STRUCT key;
  _MOUSE_STRUCT mouse;
  _JOYSTICK_STRUCT joystick;

  // sound
  int  m_sh1, m_sh2;

  // internal functions
  void Movement(void);
  void Render(void);
  void ProcessInputMessage(void);

public:

  App();
  ~App();

  // Standard Framework - required for every object
  void  SetSetup(void *setup);
  short GetDependencyCount();
  void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
  short SetDependencyData(short Index, void* Data);
  void  GetSetup(void *setup);
  short CheckStatus();

  // Standard App       - required for app
  short CreateObjects(void);
  short Initialize(void);
  void  Run(void);
  void  Uninitialize();
  void  DeleteObjects(void);
};

