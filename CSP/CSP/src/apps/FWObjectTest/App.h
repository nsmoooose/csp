#include "app/StandardApp.h"
//#include <graphics/win/GLPolygonListObject.h>
#include <graphics/win/Texture.h>
#include <graphics/win/TextureFormat.h>
#include <standard/mtxlib.h>
#include <graphics/win/TextWindow.h>
#include "AppBase.h"

class App : public AppBase
{

private:


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


  matrix33 planeOrientation;
  vector3  planeInitialDirection;
  vector3  planeCurrentDirection;
  vector3  planeCurrentPosition;
  vector3  planeInitialNormDir;
  vector3  planeCurrentNormDir;
  vector3  planeCurrentVelocity;
  vector3  planeCurrentVelocityDir;

  float dt;
  float planeSpeed;
  float thrust;
  float thrustMin;
  float thrustMax;
  float aileron;
  float elevator;
  float aileronMin;
  float aileronMax;
  float elevatorMin;
  float elevatorMax;
  vector3 planeCurrentForceTotal;
  vector3 planeGravityForce;
  vector3 planeThrustForce;
  vector3 planeLiftForce;
  vector3 planeDragForce;
  
  PolygonListObject * terrainObject;
  PolygonListObject * skyObject;
  _Texture * textureObject;

  Texture2D * grassTexture;
  Texture * skyTexture;
  MaterialObject * pTerrainMaterialObject;
  MaterialObject * m_pSkyMaterial;
  TextureFormat * grassTextureFormat;
  TextureFormat * skyTextureFormat;
  Camera * camera;

  PolygonListObject * m_pCube;
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
//  void  SetSetup(void *setup);
//  short GetDependencyCount();
//  void  GetDependencyDescription(short Index, _DEPENDENCY *Dependency);
//  short SetDependencyData(short Index, void* Data);
//  void  GetSetup(void *setup);
//  short CheckStatus();

  // Standard App       - required for app
//  short CreateObjects(void);
  short Initialize(void);
  void  Run(void);
  void  Uninitialize();
//  void  DeleteObjects(void);

  void CreateSceneObjects();
  PolygonListObject * CreateCube();


};

