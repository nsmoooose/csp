#include <Framework.h>

#include <BaseObject.h>
#include <Projectile.h>
#include <BaseController.h>
#include <ObjectManager.h>
#include <ControllerManager.h>

#include <iostream>

using namespace std;

Projectile::Projectile()
{
  cout << "Projectile::Projectile()" << endl;

  object_id = ObjectManager::getObjectManager().getNewObjectID();
  object_type = 2;
  object_name = "UNNAMEDPROJECTILE";

  controller_id = 0;
  deleteFlag = false;

  position = StandardVector3(0,0,0);
  velocity = StandardVector3(0,0,0);

}

Projectile::~Projectile()
{
  cout << "Projectile::~Projectile()" << endl;
}

/*! \fn Projectile::Projectile(const Projectile & rhs)
 *  \brief Copy Constructor
 */
Projectile::Projectile(const Projectile & rhs)
{

}

/*! \fn Projectile::operator=(const Projectile & rhs)
 *  \brief assignment operator
 */
Projectile & Projectile::operator=(const Projectile & rhs)
{
  return *this;
}



void Projectile::Initialize()
{

}

void Projectile::dump()
{
  cout << "ID: " << object_id
       << ", TYPE: " << object_type
       << ", POSX: " << position.x
       << ", POSY: " << position.y
       << ", POSZ: " << position.z
       << ", VELX: " << velocity.x
       << ", VELY: " << velocity.y
       << ", VELZ: " << velocity.z
       << endl;
}

void Projectile::OnUpdate(unsigned int dt)
{
  DoMovement(dt);
  BaseController * controller = ControllerManager::getControllerManager().getControllerFromID(controller_id);
  if (controller)
    controller->OnUpdate(dt);

}

void Projectile::DoMovement(unsigned int dt)
{
  float step_size = 0.001*(float)dt;      // convert time to secs

  // update Projectiles position and velocity.
  float speed = velocity.Length();
  float speed2 = speed*speed;
  float dragc = 0.001;

  // assume air drag is portional to speed squared.

  StandardVector3 old_position = position;
  StandardVector3 old_velocity = velocity;

  velocity.x = velocity.x  - velocity.x*speed*dragc*step_size;
  velocity.y = velocity.y  - velocity.y*speed*dragc*step_size ;
  velocity.z = velocity.z - 9.81*step_size  - velocity.z*speed*dragc*step_size ;
  
  position.x = position.x + (velocity.x+old_velocity.x)/2.0*step_size;
  position.y = position.y + (velocity.y+old_velocity.y)/2.0*step_size;
  position.z = position.z + (velocity.z+old_velocity.z)/2.0*step_size;

  // dump current state
  //  dump();


}

void Projectile::OnPhysics(unsigned int dt)
{

}

void Projectile::OnController(unsigned int dt)
{

}

void Projectile::ReceiveUpdate(const ObjectData &)
{

}

void Projectile::ReceiveMessage(const Message & )
{

}


void Projectile::SetNamedValue(const char * name, const char * value)
{

}

void Projectile::SetNamedObject(const char * name, const BaseObject * value)
{
 
}
 
void Projectile::SetValue(const char * value)
{

}

void Projectile::SetTokenizedValue(int token, const char * value)
{

}

BaseObject * Projectile::CreateNamedObject(const char * str)
{
  return NULL;
}
