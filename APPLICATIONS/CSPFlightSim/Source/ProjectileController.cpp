#include "stdinc.h"



#include "ProjectileController.h"
#include "Message.h"
#include "ProjectileObject.h"
#include "ObjectRangeInfo.h"

#include "MessageQueue.h"
#include "VirtualBattlefield.h"

extern MessageQueue * g_pMessageQueue;
extern VirtualBattlefield * g_pBattlefield;


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
  ProjectileObject * obj = (ProjectileObject *)g_pBattlefield->getObjectFromID(object_id);

  if (obj)
    {
      // see if we have hit anything (ground or another object).
      if (obj->getLocalPosition().z <= 0.0)
	{
	  cout << "Projectile: " << obj->getObjectID() << " has hit the ground, Z:" << obj->getLocalPosition().z << endl;
      obj->setDeleteFlag(true);
	}

    }

}

void ProjectileController::OnMessage(const Message * message)
{

}

void ProjectileController::OnEnter()
{
  cout << "ProjectileController::OnEnter()" << endl;

  ProjectileObject * obj = (ProjectileObject *)g_pBattlefield->getObjectFromID(object_id);

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

  ProjectileObject * obj = (ProjectileObject *)g_pBattlefield->getObjectFromID(object_id);

  if (obj)
    {


    }

}
