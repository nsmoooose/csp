#include "stdinc.h"

#include "BaseObject.h"
#include "MissileObject.h"
#include "BaseController.h"
#include "Message.h"

#include "MessageQueue.h"
#include "VirtualBattlefield.h"

extern MessageQueue * g_pMessageQueue;
extern VirtualBattlefield * g_pBattlefield;


MissileObject::MissileObject()
{

  m_iObjectType = MISSILE_OBJECT_TYPE;
  m_iObjectID = g_pBattlefield->getNewObjectID();
  m_iControllerID = 0;
  m_sObjectName = "UNNAMEDMISSILE";
  m_bDeleteFlag = false;
  m_pController = NULL;

  m_Orientation = StandardMatrix3::IDENTITY;
  m_InitialDirection = StandardVector3(0.0, 1.0, 0.0);
  m_InitialNormDir = StandardVector3(0.0,0.0,1.0);

  m_Direction = StandardVector3(0,1,0);
  m_CurrentNormDir = StandardVector3(0.0,0.0,1.0);
  setGlobalPosition( StandardVector3(0.0,0.0,0.0) );
  m_Speed = 500.0;      // meters per sec
  m_LinearVelocity = m_Speed * m_Direction;
  m_LinearVelocityDirection = m_LinearVelocity.Normalize();
  m_Thrust = 0.7;
  m_ThrustMin = 0.0;
  m_ThrustMax = 1.0;
  m_Aileron=0.0;
  m_Elevator=0.0;
  m_AileronMin = -1.0;
  m_AileronMax = 1.0;
  m_ElevatorMin = -1.0;
  m_ElevatorMax = 1.0;

  m_AileronKey = 0.0;
  m_ElevatorKey = 0.0;

  m_GravityForce = StandardVector3(0,0,-1);    // gravity force is always towards earth.
  m_ThrustForce = StandardVector3(0,0,0);
  m_LiftForce = StandardVector3(0,0,0);
  m_DragForce = StandardVector3(0,0,0);
  m_CurrentForceTotal = StandardVector3(0,0,0);

  sensativity = 0.01;
  planesensx = 0.1;
  planesensy = 0.1;
  plusplus = 0.1;

  m_fBoundingSphereRadius = 5;

  particleCount = 0;
  particlePos = 0;
  particleMax = 1000;
  m_ParticlePosition = new StandardVector3[particleMax];

//  m_MaxFlightTime = 30;
  m_MaxFlightTime = 3.0;
  m_FlightTime = 0.0;

}

MissileObject::~MissileObject()
{
    delete [] m_ParticlePosition;
}

void MissileObject::initialize()
{
}

void MissileObject::dump()
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
           << endl;      

}

void MissileObject::OnUpdate(double dt)
{
  doMovement(dt);
  if (m_pController)
    m_pController->OnUpdate(dt);

  return;

  m_ParticlePosition[particlePos] = m_GlobalPosition;

  particlePos++; 
  if (particlePos == particleMax)
      particlePos = 0;

  if (particleCount < particleMax)
      particleCount++;

  
  for (int i = 0;i<9 ; i++)
  {
    m_ParticlePosition[particlePos].x = m_ParticlePosition[particlePos-1].x + 
                                        2.0*rand()/RAND_MAX - 1.0;
    m_ParticlePosition[particlePos].y = m_ParticlePosition[particlePos-1].y + 
                                        2.0*rand()/RAND_MAX - 1.0;
    m_ParticlePosition[particlePos].z = m_ParticlePosition[particlePos-1].z +
                                        2.0*rand()/RAND_MAX - 1.0;

    particlePos++; 
    if (particlePos == particleMax)
      particlePos = 0;

    if (particleCount < particleMax)
      particleCount++;

  
  }

  list<BaseObject *>::iterator iter;

  list<BaseObject *> objectList = g_pBattlefield->getObjectList(); 
  int listSize = objectList.size();


  // Do collision tests
 
//  for (iter = objectList.begin() ; iter != objectList.end() ; ++iter)
//  {
//    BaseObject * object1 = *iter;
//    if (object1 != this)
//    {
//        if (g_pBattlefield->doSphereTest(this, object1))
//        {
//            g_pMessageQueue->sendMsg(COLLISION_MESSAGE, m_iObjectID, object1->getObjectID(), 0);
//            m_bDeleteFlag = true;
//        }   
//    }
  //}

}

void MissileObject::doMovement(double dt)
{
  double   step_size = dt;

  m_FlightTime += dt;
  if (m_FlightTime > m_MaxFlightTime)
  {
    m_bDeleteFlag = true;
  }
  else 
	  if ( m_FlightTime < 0.5 )
	  {
        m_LinearVelocity -= dt * 8.0 * m_CurrentNormDir;
	  }
	  if ( m_FlightTime > 0.5 && m_FlightTime < 2.0 )
		  m_LinearVelocity += (dt * 150.0  / m_LinearVelocity.Length() ) * m_LinearVelocity;

  doSimplePhysics(dt);



}

void MissileObject::doSimplePhysics(double dt)
{
  // update the orientation matrix based on control surfaces;
  /*StandardMatrix3 matZ = RotationZMatrix3(-(m_Aileron) * sensativity * planesensx * plusplus);
  m_Orientation = matZ*m_Orientation;

  StandardMatrix3 matX = RotationXMatrix3((m_Elevator) * sensativity * planesensy * plusplus);
  m_Orientation = matX*m_Orientation;
  m_OrientationInverse = m_Orientation.Inverse();

  // calculate the vector plane directions.
  m_Direction = m_Orientation * m_InitialDirection;
  m_CurrentNormDir = m_Orientation * m_InitialNormDir;

  m_Speed = m_LinearVelocity.Length();*/
 
  m_LocalPosition += dt * m_LinearVelocity;
  updateLocalPosition();
  updateGlobalPosition();
  

  CSP_LOG(CSP_APP, CSP_DEBUG, "MissileObject::SimplePhysics: ObjectID: " << m_iObjectID 
      << ", Elevator: " << m_Elevator << ", Aileron: " << m_Aileron);
  CSP_LOG(CSP_APP, CSP_DEBUG, "MissileObject::SimplePhysics: Position: " << m_GlobalPosition <<
                  ", Direction: " << m_Direction << ", Up: " << m_CurrentNormDir <<
                  ", Velocity: " << m_LinearVelocity );
  CSP_LOG(CSP_APP, CSP_DEBUG, "MissileObject::SimplePhysics: Orientation: " << m_Orientation);


}

unsigned int MissileObject::OnRender()
{
//  updateScene();
  return 0;
}

int MissileObject::updateScene()
{
	BaseObject::updateScene();
	return 0;
}
