#include "Framework.h"

#ifdef HAVE_OPENAL
 #include <AL/al.h>
 #include <AL/alu.h>
 #include <AL/altypes.h>
#endif

class SimpleFlightApp : public BaseApp
{

private:


#ifdef HAVE_OPENAL
  int                    sound_a;
  ALfloat                listenerPos[3];
  ALfloat                listenerVel[3];
  ALfloat                listenerOri[6];
#endif
  unsigned long          stim, etim, rtim;
  int mouse_x, mouse_y;


  StandardTexture   * p_Texture;
  StandardPolygonMesh * p_Terrain;

  StandardCamera * p_Camera;
  StandardMaterial * p_Material;

  StandardMatrix3 planeOrientation;
  StandardVector3  planeInitialDirection;
  StandardVector3  planeCurrentDirection;
  StandardVector3  planeCurrentPosition;
  StandardVector3  planeInitialNormDir;
  StandardVector3  planeCurrentNormDir;
  StandardVector3  planeCurrentVelocity;
  StandardVector3  planeCurrentVelocityDir;

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
  StandardVector3 planeCurrentForceTotal;
  StandardVector3 planeGravityForce;
  StandardVector3 planeThrustForce;
  StandardVector3 planeLiftForce;
  StandardVector3 planeDragForce;
  
  float plusplus;
  float sensativity;
  float planesensx, planesensy;

  float elevatorKey;
  float aileronKey;

  int a_key, s_key, d_key, f_key;
  int left_key, right_key, down_key, up_key;

public:

  SimpleFlightApp();
  ~SimpleFlightApp();

  // StandardApp
  short Initialize(_APP_INIT_STRUCT *Init);
  void  Uninitialize();

  void  OnRender();
  void  OnNetwork();
  void  On2D(char*InputLine);
  void  OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy, _GUI * GUI);
  void  OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy);

protected:
	StandardPolygonMesh * CreateCube();
    StandardPolygonMesh * CreateTerrain();
    void InitializeSim();


};

