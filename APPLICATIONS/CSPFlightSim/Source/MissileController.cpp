#include "stdinc.h"

#include "MissileController.h"
#include "Message.h"
#include "MissileObject.h"
#include "ObjectRangeInfo.h"
#include "VirtualBattlefield.h"

extern VirtualBattlefield * g_pBattlefield;



MissileController::MissileController()
{

}

MissileController::~MissileController()
{

}

void MissileController::OnUpdate(unsigned int dt)
{

}

void MissileController::OnMessage(const Message * message)
{
  MissileObject * missileObject = (MissileObject *)g_pBattlefield->getObjectFromID(object_id);

  if (missileObject)
  {
      switch(message->type)
      {
          // Destroy object if it was involved in a collision
      case COLLISION_MESSAGE:
        missileObject->setDeleteFlag(true);
        break;
      }
  }
}
  
void MissileController::OnEnter()
{

}

void MissileController::OnExit()
{

}

void MissileController::Initialize()
{

}
