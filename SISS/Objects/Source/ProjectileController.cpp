#include <Framework.h>
#include <ProjectileController.h>
#include <Message.h>
#include <Projectile.h>
#include <ObjectManager.h>
#include <ObjectRangeInfo.h>
#include <math.h>



ProjectileController::ProjectileController()
{
  cout << "ProjectileController::ProjectileController()" << endl;
  controller_type = 2;
}

ProjectileController::~ProjectileController()
{

}

void ProjectileController::OnUpdate(unsigned int dt)
{
  Projectile * obj = (Projectile *)ObjectManager::getObjectManager().getObjectFromID(object_id);

  if (obj)
    {
      // see if we have hit anything (ground or another object).
      if (obj->position.z <= 0.0)
	{
	  cout << "Projectile: " << obj->object_id << " has hit the ground, Z:" << obj->position.z << endl;
	  ObjectManager::getObjectManager().removeObjectWithID(object_id);
	}

    }

}

void ProjectileController::OnMessage(const Message * message)
{

}

void ProjectileController::OnEnter()
{
  cout << "ProjectileController::OnEnter()" << endl;

  Projectile * obj = (Projectile *)ObjectManager::getObjectManager().getObjectFromID(object_id);

  if (obj)
    {


    }
}

void ProjectileController::OnExit()
{

}

void ProjectileController::Initialize()
{
 cout << "ProjectileController::Initialize()" << endl;

  Projectile * obj = (Projectile *)ObjectManager::getObjectManager().getObjectFromID(object_id);

  if (obj)
    {


    }

}
