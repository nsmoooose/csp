#include "stdinc.h"

#include "AirplaneController.h"
#include "Message.h"
#include "TankObject.h"
#include "ObjectRangeInfo.h"
#include "VirtualBattlefield.h"

extern VirtualBattlefield * g_pBattlefield;


AirplaneController::AirplaneController()
{

}

AirplaneController::~AirplaneController()
{

}

void AirplaneController::OnUpdate(unsigned int dt)
{
 
}

void AirplaneController::OnMessage(const Message * message)
{
  AirplaneObject * airplaneObject = (AirplaneObject *)g_pBattlefield->getObjectFromID(object_id);

  if (airplaneObject)
  {
      switch(message->type)
      {
          // Destroy object if it was involved in a collision
      case COLLISION_MESSAGE:
        airplaneObject->setDeleteFlag(true);
        break;
      }
  }
}
  
void AirplaneController::OnEnter()
{

}

void AirplaneController::OnExit()
{

}

void AirplaneController::Initialize()
{

}
