#include "Framework.h"

class App : private StandardApp
{

private:

  StandardFramework     *p_Framework;
  StandardFactory       *p_Factory;
  StandardWindowText    *p_WindowText;
  StandardWindow3D      *p_Window3D;




  StandardGraphics  *p_3D;
  StandardGUI       *p_GUI;
  StandardViewport  *p_Viewport_leftLower;
  StandardViewport  *p_Viewport_leftUpper;
  StandardViewport  *p_Viewport_rightLower;
  StandardViewport  *p_Viewport_rightUpper;
  StandardViewport  *p_Viewport_full;
  StandardPolygonMesh *p_Model;
  StandardTexture   * p_TextureGrass;
  StandardTexture   * p_TextureSky;
  StandardTexture   * p_TextureCinder;
  StandardTexture   * p_TextureErde;
  StandardCamera    * p_Camera;
  StandardNetwork   * p_Network;
  StandardAudio     * p_Audio;
  StandardMaterial  * p_Material;
  bool                p_bPaused;
  _APP_INIT_STRUCT    p_AppInit;
  _GRAPHICS_INIT_STRUCT *p_Init;
  StandardInput         *p_Input;

  float                  p_fFPS;
  float                  p_fMultiplier;

  unsigned long          stim, etim, rtim;

  float xpos1, xpos2, xpos3, xpos4;
  float ypos1, ypos2, ypos3, ypos4;


public:

  App();
  ~App();

  // StandardApp
  short Initialize(_APP_INIT_STRUCT *Init);
  void  Run();
  void  Uninitialize();

  void  OnRender();
  void  OnNetwork();
  void  On2D(char*InputLine);
  void  OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy, _GUI *gui);

  void  OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy);
  void                OnStartCycle();
  void                OnEndCycle();

  _APP_INIT_STRUCT*   GetInit();
  bool                GetPaused();


  void  RequestCommands();

protected:
	StandardPolygonMesh * CreateCube();

};
