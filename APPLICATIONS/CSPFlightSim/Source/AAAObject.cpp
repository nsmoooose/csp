
#include "stdinc.h"

#include "BaseObject.h"
#include "AAAObject.h"
#include "AAAController.h"
#include "BaseController.h"
#include "Message.h"
#include "MissileObject.h"
#include "MissileController.h"
#include "VirtualBattlefield.h"
#include "MessageQueue.h"
#include "ObjectFactory.h"

extern VirtualBattlefield * g_pBattlefield;
extern MessageQueue * g_pMessageQueue;
extern ObjectFactory * g_pObjectFactory;

AAAObject::AAAObject()
{
  m_iObjectType = AAA_OBJECT_TYPE;
  m_iObjectID = g_pBattlefield->getNewObjectID();
  m_iControllerID = 0;
  m_sObjectName = "AAABATTERY";
  m_bDeleteFlag = false;

  m_Orientation = StandardMatrix3::IDENTITY;
  m_InitialDirection = StandardVector3(1.0, 0.0, 0.0);
  m_InitialNormDir = StandardVector3(0.0,0.0,1.0);


  setGlobalPosition( StandardVector3(0.0f, 0.0f, 0.0f) );
  m_LinearVelocity = StandardVector3(0.0f, 0.0f, 0.0f);
  m_Direction = StandardVector3(0.0f, 0.0f, 0.0f);

  refire_time = 10;
  weapon_ready = true;
  in_range = false;

  m_fBoundingSphereRadius = 0.001;
}

AAAObject::~AAAObject()
{

}

void AAAObject::dump()
{

}

void AAAObject::OnUpdate( float dt)
{
  if (m_pController)
    m_pController->OnUpdate(dt);

  if (weapon_ready && in_range)
      fireWeapon();

}

void AAAObject::initialize()
{

}

unsigned int AAAObject::OnRender()
{

    return 0;
}

int AAAObject::updateScene()
{
	BaseObject::updateScene();
	return 0;
}


void AAAObject::setNamedParameter(const char * name, const char * value)
{

}

void AAAObject::setPlayerPlane(AirplaneObject * pPlane)
{
   p_PlayerPlane = pPlane;
}

void AAAObject::checkForPlayerInRange()
{
    // Assume in range to be within a sphere centered 2500 meters above.
    StandardVector3 D = m_GlobalPosition + StandardVector3(0,0,2500) - p_PlayerPlane->getGlobalPosition();
    float dist = D.Length();

    if (dist < 2500)
        in_range = true;
}

void AAAObject::fireWeapon()
{
  // aim weapon and fire

  // create a weapon object
    MissileObject * pMissile = (MissileObject*)g_pObjectFactory->createNamedObject("MISSILE", "UNNAMED MISSILE");

    pMissile->setOrientation( (StandardMatrix3 &)StandardMatrix3::IDENTITY );
    pMissile->setGlobalPosition ( m_GlobalPosition );
    pMissile->setVelocity( StandardVector3(0,0,500) );


    g_pBattlefield->addObject(pMissile);

    MissileController * missileController = new MissileController;
    missileController->controller_id = g_pBattlefield->getNewObjectID();
    missileController->object_id = pMissile->getObjectID();
    pMissile->setController(missileController);

    g_pBattlefield->addController(missileController);

  // reload weapon
  weapon_ready = false;
  g_pMessageQueue->sendDelayedMsg(WEAPON_READY_MESSAGE, m_iObjectID, m_iObjectID, 10, 0);
}


