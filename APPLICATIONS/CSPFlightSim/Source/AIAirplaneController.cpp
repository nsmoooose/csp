
#include "stdinc.h"

#include "AIAirplaneController.h"
#include "Message.h"
#include "TankObject.h"
#include "ObjectRangeInfo.h"
#include "ParticleObject.h"
#include "VirtualBattlefield.h"
#include "ObjectFactory.h"

#include "TypesMath.h"

extern VirtualBattlefield * g_pBattlefield;
extern ObjectFactory * g_pObjectFactory;

AIAirplaneController::AIAirplaneController()
{
   targetAltitude = 2000;
   altitudeTol = 50;
   AltitudeState = LEVEL;
   divePitch = -45;      
   climbPitch = 45;
   pitchTol = 3;
   targetPitch = 0;


}

AIAirplaneController::~AIAirplaneController()
{

}

void AIAirplaneController::OnUpdate(unsigned int dt)
{
    AirplaneObject * airplaneObject = (AirplaneObject *)g_pBattlefield->getObjectFromID(object_id);

    float pitchDeg = RadiansToDegrees(airplaneObject->getPitch())/2;

    // set controls to for altitude
    if (airplaneObject->getLocalPosition().z < (targetAltitude - altitudeTol) )
    {
        AltitudeState = CLIMB;
        targetPitch = climbPitch;
    }
    else if (airplaneObject->getLocalPosition().z > (targetAltitude + altitudeTol) )
    {
        AltitudeState = DIVE;
        targetPitch = divePitch;
    }
    else 
    {
        AltitudeState = LEVEL;
        targetPitch = 0;
    }


    if(pitchDeg < targetPitch-pitchTol)
       airplaneObject->setElevator( -0.10 );
    else if (pitchDeg > targetPitch+pitchTol)
       airplaneObject->setElevator( 0.10 );
    else 
       airplaneObject->setElevator( 0 );

}

void AIAirplaneController::OnMessage(const Message * message)
{
  AirplaneObject * airplaneObject = (AirplaneObject *)g_pBattlefield->getObjectFromID(object_id);

  if (airplaneObject)
  {
      switch(message->type)
      {
          // Destroy object if it was involved in a collision
      case COLLISION_MESSAGE:
          {
            // create an explosion particle system. this is probably a bad place to do this but...
            ParticleObject * pParticle = new ParticleObject;
            pParticle->setGlobalPosition( airplaneObject->getGlobalPosition() );
            pParticle->initialize();
            g_pBattlefield->addObject(pParticle);

            airplaneObject->setDeleteFlag( true );
            break;
          }
      case SET_ALTITUDE_MESSAGE:
        float newAlt = (float)message->data;
        targetAltitude = newAlt;
      }
  }
}
  
void AIAirplaneController::OnEnter()
{

}

void AIAirplaneController::OnExit()
{

}

void AIAirplaneController::Initialize()
{

}
