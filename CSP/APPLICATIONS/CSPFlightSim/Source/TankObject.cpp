#include "stdinc.h"


#include "BaseObject.h"
#include "TankObject.h"
#include "TankController.h"
#include "BaseController.h"
#include "MessageQueue.h"
#include "VirtualBattlefield.h"

extern MessageQueue * g_pMessageQueue;
extern VirtualBattlefield * g_pBattlefield;

using namespace std;

TankObject::TankObject()
{

  m_iObjectType = TANK_OBJECT_TYPE;
  m_iObjectID = g_pBattlefield->getNewObjectID();
  m_iControllerID = 0;
  m_sObjectName = "UNNAMEDTANK";
  m_bDeleteFlag = false;
  m_bOnGround = true;
  m_pController = NULL;

  movement_state = 1;

  m_Orientation = StandardMatrix3::IDENTITY;
  m_InitialDirection = StandardVector3(1.0, 0.0, 0.0);
  m_InitialNormDir = StandardVector3(0.0,0.0,1.0);


  setGlobalPosition( StandardVector3(0.0f, 0.0f, 0.0f) );
  m_LinearVelocity = StandardVector3(0.0f, 0.0f, 0.0f);
  m_Direction = StandardVector3(0.0f, 0.0f, 0.0f);
  gun_angle = 45.0f;
  max_viewing_range = 2000.0;
  max_viewing_angle = 60.0;
  max_firing_range = 1000.0;
  forward_speed = 5.0;
  backward_speed = 2.0;
  turn_speed = 0.15;
  m_iArmy = 0;

  m_fBoundingSphereRadius = 20;

//  m_color = new StandardColor(0, 255, 0, 255);

}

TankObject::~TankObject()
{

}

void TankObject::initialize()
{
  if (m_pController)
    m_pController->Initialize();
}

void TankObject::setNamedParameter(const char * name, const char * value)
{
  cout << "TankObject::SetNamedParameter" << endl;
  if (strcmp(name, "NAME") == 0)
    {
      cout << "TankObject::SetNamedParameter - Setting name" << endl;
      m_sObjectName = value;
    }
    

}

void TankObject::dump()
{
  cout << "ID: " << m_iObjectID
           << ", TYPE: " << m_iObjectType
           << ", ARMY: " << m_iArmy
           << ", GLOPOSX: " << m_GlobalPosition.x
           << ", GLOPOSY: " << m_GlobalPosition.y
           << ", GLOPOSZ: " << m_GlobalPosition.z
           << ", DIRX: " << m_Direction.x
           << ", DIRY: " << m_Direction.y
           << ", DIRZ: " << m_Direction.z
           << ", VELX: " << m_LinearVelocity.x
	   << ", VELY: " << m_LinearVelocity.y
           << ", VELZ: " << m_LinearVelocity.z
           << ", GUNANGLE: " << gun_angle
           << endl;      

}

void TankObject::OnUpdate(double dt)
{
  doMovement(dt);
  if (m_pController)
    m_pController->OnUpdate(dt);
}

void TankObject::doMovement(double dt)
{
  double step_size = dt;

  // Save the objects old position
  m_PrevPosition = m_LocalPosition;

  m_Direction.Normalize(); 
  m_LinearVelocity = m_Speed * m_Direction;
  m_LocalPosition += dt * m_LinearVelocity;

  /*
  switch (movement_state)
    {
    case 0:
      // do nothing this is the stop state.
      break;
    case 1:
      // forward state
      {
	//	cout << "Tank " << tankObject->object_id << " moving forward." << endl;
	m_LocalPosition = m_LocalPosition + step_size*forward_speed*m_Direction;
      }
      break;
    case 2:
      // backward state
      {
	//	    cout << "Tank " << tankObject->object_id << " moving backward." << endl;
	m_LocalPosition = m_LocalPosition - step_size*backward_speed*m_Direction;
      }
      break;
    case 3:
      // right turn state
      {
	    
	double cur_x = m_Direction.x; double cur_y = m_Direction.y;
	m_Direction.x = m_Direction.x + step_size*turn_speed*cur_y;
	m_Direction.y = m_Direction.y - step_size*turn_speed*cur_x;
      }
      break;
    case 4:
      // left turn state
      {

	double cur_x = m_Direction.x; double cur_y = m_Direction.y;
	m_Direction.x = m_Direction.x - step_size*turn_speed*cur_y;
	m_Direction.y = m_Direction.y + step_size*turn_speed*cur_x;	
      }
      break;
      
    }
	*/


    updateGroundPosition();
  	updateLocalPosition();
	updateGlobalPosition();


}

unsigned int TankObject::OnRender()
{
//  updateScene();
  return 0;
}

int TankObject::updateScene()
{
	BaseObject::updateScene();
	return 0;
}
