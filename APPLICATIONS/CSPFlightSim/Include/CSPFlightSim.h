#ifndef __CSPFLIGHTSIM_H__
#define __CSPFLIGHTSIM_H__


#include "BaseScreen.h"
#include "BaseObject.h"
#include "SDL.h"
#include "CON_console.h"

class ofstream;


/**
 * class CSPFlightSim - Describe me!
 *
 * @author unknown
 */
class CSPFlightSim
{

public:
    CSPFlightSim();
    virtual ~CSPFlightSim();
    virtual void Init();
    virtual void Run();
    virtual void Exit();
    virtual void Cleanup();

    void ChangeScreen(BaseScreen * newScreen);
	SDL_Surface * GetSDLScreen() {return m_SDLScreen;};

protected:
	void InitSim();
    int InitSDL();
    int InitConsole();
    void ShowStats(float fps);
    void ShowPlaneInfo();
	void ShowPaused();
    
    void DoInput();
    void UpdateObjects(float dt);
    void DoStartupScript();


private:
    SDL_Surface * m_SDLScreen;
    bool m_bFinished;
    BaseScreen * m_CurrentScreen;
    BaseScreen * m_PrevScreen;

    BaseScreen * m_logoScreen;
    BaseScreen * m_gameScreen;
    BaseScreen * m_MainMenuScreen;

    unsigned int curSceneTime, prevSceneTime;

    bool m_bConsoleDisplay;
    ConsoleInformation * m_pConsole;
    bool m_bFreezeSim;
    bool m_bShowStats;


};




#endif







/*


#include "Framework.h"
#include <queue>

#include "BaseObject.h"
#include "TankObject.h"
#include "AirplaneObject.h"
#include "TankController.h"
#include "MissileObject.h"
#include "Message.h"

#include <SISS/TerrainEngine/TerrainEngine.h>
//#include OGLText.h"
#include <SISS/TerrainEngine/Vector.h>


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

    list<BaseController *> controllerList;
    priority_queue<Message *, vector<Message*>, messageComparison > delayedMessageList;

 CTerrainEngine*			m_pTerrainEngine;


  AirplaneObject * p_PlayerPlane;
  AirplaneObject * p_ChasePlane;
  MissileObject * p_Missile;
  StandardTexture * p_MissileTexture;

  TankObject * p_Tank1;
  TankObject * p_Tank2;
  TankObject * p_Tank3;
  TankObject * p_Tank4;
  TankObject * p_Tank5;


  StandardTexture   * p_Texture;
  StandardPolygonMesh * p_Terrain;

  StandardCamera * p_Camera;
  StandardMaterial * p_Material;

//  StandardMatrix3 planeOrientation;
//  StandardVector3  planeInitialDirection;
//  StandardVector3  planeCurrentDirection;
//  StandardVector3  planeCurrentPosition;
//  StandardVector3  planeInitialNormDir;
//  StandardVector3  planeCurrentNormDir;
//  StandardVector3  planeCurrentVelocity;
//  StandardVector3  planeCurrentVelocityDir;

//  StandardMatrix4  planeWorldMatrix;
  
//  StandardMatrix3 planeOrientation2;
//  StandardVector3  planeInitialDirection2;
//  StandardVector3  planeCurrentDirection2;
//  StandardVector3  planeCurrentPosition2;
//  StandardVector3  planeInitialNormDir2;
//  StandardVector3  planeCurrentNormDir2;
//  StandardVector3  planeCurrentVelocity2;
//  StandardVector3  planeCurrentVelocityDir2;
//  float planeSpeed2;

//  StandardMatrix3 missileOrientation;
//  StandardVector3  missileInitialDirection;
//  StandardVector3  missileCurrentDirection;
//  StandardVector3  missileCurrentPosition;
//  StandardVector3  missileInitialNormDir;
//  StandardVector3  missileCurrentNormDir;
//  StandardVector3  missileCurrentVelocity;
//  StandardVector3  missileCurrentVelocityDir;
//  float missileSpeed;
//  bool missileActive;
//  int missileFlightTime;
//  int missileTime;
  
  float dt;
//  float planeSpeed;
//  float thrust;
//  float thrustMin;
//  float thrustMax;
//  float throttle;
//  float aileron;
//  float elevator;
//  float aileronMin;
//  float aileronMax;
//  float elevatorMin;
//  float elevatorMax;
//  StandardVector3 planeCurrentForceTotal;
//  StandardVector3 planeGravityForce;
//  StandardVector3 planeThrustForce;
//  StandardVector3 planeLiftForce;
//  StandardVector3 planeDragForce;
  
  float plusplus;
  float sensativity;
  float planesensx, planesensy;

  float elevatorKey;
  float aileronKey;

  int a_key, s_key, d_key, f_key;
  int left_key, right_key, down_key, up_key;

    StandardPolygonMesh * pAircraftMesh;
    StandardPolygonMesh * pTankMesh;

    StandardPolygonMesh * pMissileMesh;

    int view_mode;
    float missile_delay;

public:

  SimpleFlightApp();
  ~SimpleFlightApp();

  // StandardApp
  short Initialize(_APP_INIT_STRUCT *Init,
                   _FRAMEWORK_INIT_STRUCT * FrameworkInit);
  void  Uninitialize();

  void  OnRender();
  void  OnNetwork();
  void  OnConsoleText(char*InputLine);
  void  OnGUI(_KEY_BUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy, _GUI * GUI);
  void  OnMovement(_KEY_UNBUFFERED *key, _MOUSE *mouse, _JOYSTICK *joy);

protected:
	StandardPolygonMesh * CreateCube();
    StandardPolygonMesh * CreateTerrain();
    void InitializeSim();


    void CreateMeshFrom3DSFile2(char * filename, 
                               StandardPolygonMesh ** pPolygonMesh);

    BaseController * getControllerFromID(unsigned int ID);
    void routeMessage( Message* msg);
    unsigned int getCurTime();
    void sendDelayedMsg(int type, unsigned int senderID, unsigned int receiverID, unsigned int delay, int data);
    void storeDelayedMsg( Message * msg);
    void sendMsg(int type, unsigned int senderID, unsigned int receiverID, int data);
    void dumpDelayedMsgList();
    void processDelayedMsgList();
    float AngleBetweenTwoVectors(const StandardVector3 & v1, const StandardVector3 & v2);




};

*/
