#include "Framework.h"

class App : private StandardApp
{

private:

  // standard objects
  StandardFramework     *p_Framework;
  StandardFactory       *p_Factory;
  StandardWindowText    *p_WindowText;
  StandardWindow3D      *p_Window3D;
  StandardGraphics      *p_3D;
  StandardInput         *p_Input;
  StandardGUI           *p_GUI;
  StandardCamera        *p_Camera;
  StandardNetwork       *p_Network;
  StandardAudio         *p_Audio;
  StandardMaterial      *p_Material;

  // extra objects
  StandardPolygonMesh   *p_Model;
  StandardTexture       *p_TextureGrass;

  // members
  _APP_INIT_STRUCT       p_AppInit;
  _GRAPHICS_INIT_STRUCT *p_Init;

  bool                   p_bPaused;
  float                  p_fFPS;
  float                  p_fMultiplier;

  unsigned long          stim, etim, rtim;

public:

  App();
  ~App();

  // StandardApp
  short               Initialize(_APP_INIT_STRUCT *Init);
  void                Uninitialize();

  void                Run();

  void                RequestCommands();
  _APP_INIT_STRUCT*   GetInit();
  bool                GetPaused();

  void                OnStartCycle();
  void                OnEndCycle();
  void                OnRender();
  void                OnNetwork();
  void                On2D(char*InputLine);
  void                OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy, _GUI *gui);

  void                OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy);



};
