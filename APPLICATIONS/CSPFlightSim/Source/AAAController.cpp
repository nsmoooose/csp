

#include "stdinc.h"


#include "AAAController.h"
#include "Message.h"
#include "AAAObject.h"
#include "ObjectRangeInfo.h"
#include "ParticleObject.h"
#include "MessageQueue.h"
#include "VirtualBattlefield.h"



extern MessageQueue * g_pMessageQueue;
extern VirtualBattlefield * g_pBattlefield;

AAAController::AAAController()
{

}

AAAController::~AAAController()
{

}

void AAAController::OnMessage(const Message * msg)
{
  AAAObject * aaaObject = (AAAObject *)g_pBattlefield->getObjectFromID(object_id);

  if (aaaObject)
  {

    
    switch (msg->type)
    {
    case LOOK_FOR_ENEMIES_MESSAGE:
        {
            aaaObject->checkForPlayerInRange();
            g_pMessageQueue->sendDelayedMsg(LOOK_FOR_ENEMIES_MESSAGE, object_id, object_id, 2.5, 0);

        }
        break;
    case WEAPON_READY_MESSAGE:
        aaaObject->weapon_ready = true;
        break;
    }
  }
}

void AAAController::OnUpdate(unsigned int dt)
{

}

void AAAController::OnEnter()
{

}

void AAAController::OnExit()
{

}

void AAAController::Initialize()
{

}
