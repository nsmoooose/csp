#include "InstantActionManager.h"
#include "InstantActionController.h"
#include "Message.h"
#include "TankObject.h"
#include "ObjectRangeInfo.h"
#include "math.h"
#include "AirplaneObject.h"
#include "AIAirplaneController.h"
#include "MessageQueue.h"
#include "VirtualBattlefield.h"

extern MessageQueue * g_pMessageQueue;
extern VirtualBattlefield * g_pBattlefield;


InstantActionController::InstantActionController()
{

}

InstantActionController::~InstantActionController()
{

}

void InstantActionController::OnMessage(const Message * msg)
{
  InstantActionManager * instantActionObject = (InstantActionManager *)g_pBattlefield->getObjectFromID(object_id);

  switch (msg->type)
  {
  case CHECK_GAME_STATUS_MESSAGE:
      {
         if (!countPlanesInRange())
             g_pMessageQueue->sendDelayedMsg(GENERATE_NEW_ENEMY_PLANES_MESSAGE, object_id, object_id, 5, 0);

         if (!countTanksInRange())
             generateNewEnemyTanks();

         g_pMessageQueue->sendDelayedMsg(CHECK_GAME_STATUS_MESSAGE, object_id, object_id, 2, 0);
         break;
      }
  case GENERATE_NEW_ENEMY_PLANES_MESSAGE:
      {
        generateNewEnemyPlanes();
        break;
      }


  }


}

void InstantActionController::OnUpdate(unsigned int dt)
{

}

void InstantActionController::OnEnter()
{

}

void InstantActionController::OnExit()
{

}

void InstantActionController::Initialize()
{

}

// count all enemy planes in range of player plane.
int InstantActionController::countPlanesInRange()
{
  InstantActionManager * manager = (InstantActionManager *)g_pBattlefield->getObjectFromID(object_id);

  int count=0;
  list<BaseObject *>::iterator iter;

  list<BaseObject *> objectList = g_pBattlefield->getObjectList(); 
  int listSize = objectList.size();

  if (!manager->p_PlayerPlane)
      return 0;

  for (iter = objectList.begin() ; iter != objectList.end() ; ++iter)
  {
    BaseObject * object = *iter;
    if (object->getObjectType() == AIRPLANE_OBJECT_TYPE)
    {
        StandardVector3 D = object->getPosition() - manager->p_PlayerPlane->getPosition();
        float dist = D.Length();
        if (dist < manager->m_planeRange)
            count++;
    }
  }

  return count;
}

void InstantActionController::generateNewEnemyPlanes()
{
  // Create the other planes
  InstantActionManager * manager = (InstantActionManager *)g_pBattlefield->getObjectFromID(object_id);
  AirplaneObject * pPlayerPlane = manager->p_PlayerPlane;

  float altitOne = 1000;
  float altitTwo = 2000;
  float altitThree = 1000;

  AirplaneObject * p_ChasePlane = NULL;
  AIAirplaneController * aIAirplaneController = NULL;

  // First Plane
  p_ChasePlane = new AirplaneObject;

  p_ChasePlane->setOrientation( (StandardMatrix3 &)StandardMatrix3::IDENTITY);
//  p_ChasePlane->m_OrientationInverse = p_ChasePlane->m_Orientation.Inverse();

//  p_ChasePlane->m_Direction = StandardVector3(1.0, 0.0,0.0);
  StandardVector3 pos = pPlayerPlane->getPosition() + StandardVector3(1500.0, 0, 0);
  p_ChasePlane->setPosition(pos);
  p_ChasePlane->m_CurrentNormDir = StandardVector3(0.0,0.0,1.0);
//  p_ChasePlane->m_Speed = 100.0;

  p_ChasePlane->setVelocity(100.0 * StandardVector3(1.0, 0.0,0.0));
//  p_ChasePlane->m_LinearVelocityDirection = p_ChasePlane->m_LinearVelocity.Normalize();

//  p_ChasePlane->p_Mesh = pPlayerPlane->p_Mesh;

  aIAirplaneController = new AIAirplaneController;
  aIAirplaneController->controller_id = g_pBattlefield->getNewObjectID();
  aIAirplaneController->object_id = p_ChasePlane->getObjectID();
  aIAirplaneController->SetAltitude(altitTwo);
  p_ChasePlane->setController(aIAirplaneController);
  

  g_pBattlefield->addObject(p_ChasePlane);
  g_pBattlefield->addController(aIAirplaneController);

  g_pMessageQueue->sendDelayedMsg(SET_ALTITUDE_MESSAGE, p_ChasePlane->getObjectID(), p_ChasePlane->getObjectID(), 50, altitThree);


  // Second Plane
  p_ChasePlane = new AirplaneObject;

  p_ChasePlane->m_Orientation = StandardMatrix3::IDENTITY;
  p_ChasePlane->m_OrientationInverse = p_ChasePlane->m_Orientation.Inverse();

  p_ChasePlane->m_Direction = StandardVector3(1.0, 0.0,0.0);
  p_ChasePlane->m_Position = pPlayerPlane->m_Position + StandardVector3(1750.0, 0, 0);
  p_ChasePlane->m_CurrentNormDir = StandardVector3(0.0,0.0,1.0);
  p_ChasePlane->m_Speed = 100.0;

  p_ChasePlane->m_LinearVelocity= p_ChasePlane->m_Speed*p_ChasePlane->m_Direction;
  p_ChasePlane->m_LinearVelocityDirection = p_ChasePlane->m_LinearVelocity.Normalize();

//  p_ChasePlane->p_Mesh = pPlayerPlane->p_Mesh;

  aIAirplaneController = new AIAirplaneController;
  aIAirplaneController->controller_id = g_pBattlefield->getNewObjectID();;
  aIAirplaneController->object_id = p_ChasePlane->object_id;
  aIAirplaneController->SetAltitude(altitTwo);
  p_ChasePlane->controller = aIAirplaneController;
  p_ChasePlane->controller_id = aIAirplaneController->controller_id;

  g_pBattlefield->addObject(p_ChasePlane);
  g_pBattlefield->addController(aIAirplaneController);

  g_pMessageQueue->sendDelayedMsg(SET_ALTITUDE_MESSAGE, p_ChasePlane->object_id, p_ChasePlane->object_id, 50, altitThree);


  // Third Plane
  p_ChasePlane = new AirplaneObject;

  p_ChasePlane->m_Orientation = StandardMatrix3::IDENTITY;
  p_ChasePlane->m_OrientationInverse = p_ChasePlane->m_Orientation.Inverse();

  p_ChasePlane->m_Direction = StandardVector3(1.0, 0.0,0.0);
  p_ChasePlane->m_Position = pPlayerPlane->m_Position + StandardVector3(1500.0, 250, 0);
  p_ChasePlane->m_CurrentNormDir = StandardVector3(0.0,0.0,1.0);
  p_ChasePlane->m_Speed = 100.0;

  p_ChasePlane->m_LinearVelocity= p_ChasePlane->m_Speed*p_ChasePlane->m_Direction;
  p_ChasePlane->m_LinearVelocityDirection = p_ChasePlane->m_LinearVelocity.Normalize();

//  p_ChasePlane->p_Mesh = pPlayerPlane->p_Mesh;

  aIAirplaneController = new AIAirplaneController;
  aIAirplaneController->controller_id = g_pBattlefield->getNewObjectID();;
  aIAirplaneController->object_id = p_ChasePlane->object_id;
  aIAirplaneController->SetAltitude(altitTwo);
  p_ChasePlane->controller = aIAirplaneController;
  p_ChasePlane->controller_id = aIAirplaneController->controller_id;

  g_pBattlefield->addObject(p_ChasePlane);
  g_pBattlefield->addController(aIAirplaneController);

  g_pMessageQueue->sendDelayedMsg(SET_ALTITUDE_MESSAGE, p_ChasePlane->object_id, p_ChasePlane->object_id, 50, altitThree);

  // Fourth Plane
  p_ChasePlane = new AirplaneObject;

  p_ChasePlane->m_Orientation = StandardMatrix3::IDENTITY;
  p_ChasePlane->m_OrientationInverse = p_ChasePlane->m_Orientation.Inverse();

  p_ChasePlane->m_Direction = StandardVector3(1.0, 0.0,0.0);
  p_ChasePlane->m_Position = pPlayerPlane->m_Position + StandardVector3(1500.0, -250, 0);
  p_ChasePlane->m_CurrentNormDir = StandardVector3(0.0,0.0,1.0);
  p_ChasePlane->m_Speed = 100.0;

  p_ChasePlane->m_LinearVelocity= p_ChasePlane->m_Speed*p_ChasePlane->m_Direction;
  p_ChasePlane->m_LinearVelocityDirection = p_ChasePlane->m_LinearVelocity.Normalize();

//  p_ChasePlane->p_Mesh = pPlayerPlane->p_Mesh;

  aIAirplaneController = new AIAirplaneController;
  aIAirplaneController->controller_id = g_pBattlefield->getNewObjectID();
  aIAirplaneController->object_id = p_ChasePlane->object_id;
  aIAirplaneController->SetAltitude(altitTwo);

  p_ChasePlane->SetController(aIAirplaneController);

  g_pBattlefield->addObject(p_ChasePlane);
  g_pBattlefield->addController(aIAirplaneController);

  g_pMessageQueue->sendDelayedMsg(SET_ALTITUDE_MESSAGE, p_ChasePlane->object_id, p_ChasePlane->object_id, 50, altitThree);
  
}

// count all enemy planes in range of player plane.
int InstantActionController::countTanksInRange()
{

    return 1;
}

void InstantActionController::generateNewEnemyTanks()
{

}

void InstantActionController::removeEnemyPlanesOutOfRange()
{

}

void InstantActionController::removeEnemyTanksOutOfRange()
{

}