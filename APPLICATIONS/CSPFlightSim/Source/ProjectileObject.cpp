#include "stdinc.h"

#include "BaseObject.h"
#include "ProjectileObject.h"
#include "BaseController.h"
#include "VirtualBattlefield.h"

extern VirtualBattlefield * g_pBattlefield;

#include "iostream"

using namespace std;


ProjectileObject::ProjectileObject()
{
  cout << "ProjectileObject::ProjectileObject()" << endl;

  m_iObjectID = g_pBattlefield->getNewObjectID();
  m_iObjectType = PROJECTILE_OBJECT_TYPE;
  m_sObjectName = "UNNAMEDPROJECTILE";

  m_pController = NULL;
  m_iControllerID = 0;
  m_bDeleteFlag = false;

  setGlobalPosition( StandardVector3(0,0,0) );
  m_LinearVelocity = StandardVector3(0,0,0);

}

ProjectileObject::~ProjectileObject()
{
  cout << "ProjectileObject::~ProjectileObject()" << endl;
}

void ProjectileObject::initialize()
{

}

void ProjectileObject::dump()
{
  cout << "ID: " << m_iObjectID
       << ", TYPE: " << m_iObjectType
       << ", GLOPOSX: " << m_GlobalPosition.x
       << ", GLOPOSY: " << m_GlobalPosition.y
       << ", GLOPOSZ: " << m_GlobalPosition.z
       << ", VELX: " << m_LinearVelocity.x
       << ", VELY: " << m_LinearVelocity.y
       << ", VELZ: " << m_LinearVelocity.z
       << endl;
}

void ProjectileObject::OnUpdate(double dt)
{
  doMovement(dt);
  if (m_pController)
    m_pController->OnUpdate(dt);

}

void ProjectileObject::doMovement(double dt)
{
  double step_size = dt;      // convert time to secs

  // update Projectiles position and velocity.
  double speed = m_LinearVelocity.Length();
  double speed2 = speed*speed;
  double dragc = 0.001;

  // assume air drag is portional to speed squared.

  StandardVector3 old_position = m_LocalPosition;
  StandardVector3 old_velocity = m_LinearVelocity;

  m_LinearVelocity.x = m_LinearVelocity.x  - m_LinearVelocity.x*speed*dragc*step_size;
  m_LinearVelocity.y = m_LinearVelocity.y  - m_LinearVelocity.y*speed*dragc*step_size ;
  m_LinearVelocity.z = m_LinearVelocity.z - 9.81*step_size  - m_LinearVelocity.z*speed*dragc*step_size ;
  
  m_LocalPosition.x = m_LocalPosition.x + (m_LinearVelocity.x+old_velocity.x)/2.0*step_size;
  m_LocalPosition.y = m_LocalPosition.y + (m_LinearVelocity.y+old_velocity.y)/2.0*step_size;
  m_LocalPosition.z = m_LocalPosition.z + (m_LinearVelocity.z+old_velocity.z)/2.0*step_size;

  // dump current state
  //  dump();


}

unsigned int ProjectileObject::OnRender()
{
  updateScene();
  return 0;
}
