#include "app/StandardApp.h"
#include <graphics/win/GLPolygonListObject.h>
#include <graphics/win/Texture.h>
#include <graphics/win/TextureFormat.h>
#include <graphics/win/Camera.h>
#include "mtxlib.h"
#include <graphics/win/TextWindow.h>

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
  bool appQuit;
  bool appInit;
  bool appMessageWaiting;
  bool appHasFocus;
  bool appPaused;
  bool appCanFlip;

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

//  double plane_x, plane_y, plane_z;
//  double plane_dx, plane_dy, plane_dz;
//  double plane_ang_xaxis, plane_ang_yaxis, plane_ang_zaxis;
//  double plane_nx, plane_ny, plane_nz;
//  double plane_upx, plane_upy, plane_upz;
//  double plane_view_dir;

  matrix33 planeOrientation;
  vector3  planeInitialDirection;
  vector3  planeCurrentDirection;
  vector3  planeCurrentPosition;
  vector3  planeInitialUp;
  float dt;
  float planeSpeed;

  PolygonListObject * terrainObject;
  PolygonListObject * skyObject;
  _Texture * textureObject;

  Texture * grassTexture;
  Texture * skyTexture;
  TextureFormat * grassTextureFormat;
  TextureFormat * skyTextureFormat;
  Camera * camera;
	Font * font;

	TextWindow * textWindow;

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

