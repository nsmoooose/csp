#include "stdinc.h"

#include "TankController.h"
#include "Message.h"
#include "TankObject.h"
#include "ObjectRangeInfo.h"
#include "ParticleObject.h"
#include "VirtualBattlefield.h"
#include "MessageQueue.h"
#include "SimTime.h"

#include "TypesMath.h"

extern VirtualBattlefield * g_pBattlefield;
extern MessageQueue * g_pMessageQueue;

extern SimTime * g_pSimTime;

TankController::TankController()
{
  controller_type = 1;

  state = 0;
  waypoint_state = 0;
}

TankController::~TankController()
{

}

// called every game tick.
void TankController::OnUpdate(unsigned int dt)
{

  CSP_LOG(CSP_APP, CSP_BULK, "TankController::OnUpdate() - State: " << state);

  TankObject * tankObject = (TankObject *)g_pBattlefield->getObjectFromID(object_id);
  if (tankObject)
  {

     switch (state)
     {
        case 0:     // initializing state - this should be not reached
	    break;

	    case 1:     // searching state
	    {
	
	      // look for enemies
	      int enemyArmy;
          if (tankObject->getArmy() == 1) enemyArmy = 2;
          if (tankObject->getArmy() == 2) enemyArmy = 1;
	      enemyList.clear();
	      g_pBattlefield->getObjectsInRange(tankObject, 
								tankObject->getMaxViewingRange(), 
								tankObject->getMaxViewingAngle() , enemyArmy, 
								enemyList );
//	      enemyList.dump();
	      // if we find any enemies switch to an attack state.
	      if (!enemyList.empty())
          {
		    // find Nearest enemy.
		    list<ObjectRangeInfo*>::iterator i;
		    ObjectRangeInfo * info_nearest = NULL;
		    for(i = enemyList.begin(); i != enemyList.end() ; i++)
		    {
			  ObjectRangeInfo * info = *i;
			
			  // set the nearest to the info if null (should happen first time)
			  if (info_nearest == NULL)
			    info_nearest = info;

			  if (info->getRange() < info_nearest->getRange() )
			  {
			    info_nearest = info;
			  }
		    }

		    target_id = info_nearest->getObjectID();

		    // if nearest is in firing range switch attack state
		    if (info_nearest->getRange() < tankObject->getMaxFiringRange() )
		    {
			  cout << "Tank " << tankObject->getObjectID() << " switching to state " << 3 << endl;
			  state = 3;
            }
		    // if not in firing range switch to approach range
		    else
            {
			  cout << "Tank " << tankObject->getObjectID() << " switching to state " << 2 << endl;
			  state = 2;

            }


		}


	    checkCourseAndDirToWayPoint(tankObject);
	      
	      
	    
	      
	    break;
	  }
	  case 2:     // enemy spotted but out of range state
	  {
	      BaseObject * toObject = g_pBattlefield->getObjectFromID(target_id);
	      TankObject * toTankObject = (TankObject*)toObject;
	      float distance;
	      StandardVector3 direction;
	      g_pBattlefield->getObjectDistance(tankObject, toTankObject, distance, direction);
	      float angle = angleBetweenTwoVectors(tankObject->getDirection(), direction);
	 

	  }
	  break;

      case 3:     // enemy in range, attacking state
      {

	  }
	  break;

	  case 4:     // dead state
	  {

	  }
	  break;
	  

	  default:
	    break;
   }

  }
}


void TankController::initialize()
{
  TankObject * tankObject = (TankObject *)g_pBattlefield->getObjectFromID(object_id);
  if (tankObject)
    {
      switch (state)
	{
          case 0:     // initializing state 
	    state = 1;  // switch to searching state
	    setNewWayPoint(tankObject);
        tankObject->setMovementState( 1 );
	    break;

	  case 1:     // searching state
	    break;

          case 2:      // enemy spotted but out of range state
	    break;

	  case 3:       // enemy in range, attacking state

	    break;

	  case 4:      // dead state
	    break;

	default:

	  break;
	}

    }

}

// Called when a message is sent to the object.
void TankController::OnMessage(const Message * msg)
{
  cout << "TankController: " << controller_id << " received Message for object " << msg->receiverID 
       << " from object " << msg->senderID << endl;

  TankObject * tankObject = (TankObject *)g_pBattlefield->getObjectFromID(object_id);
  BaseObject * pSendObject = g_pBattlefield->getObjectFromID(msg->senderID);

  switch(msg->type)
  {
      // Destroy object if it was involved in a collision with a missile or projectile.
  case COLLISION_MESSAGE:
      { 
        if (pSendObject->getObjectType() == MISSILE_OBJECT_TYPE ||
            pSendObject->getObjectType() == PROJECTILE_OBJECT_TYPE)
        {
            ParticleObject * pParticle = new ParticleObject;
            pParticle->setGlobalPosition( tankObject->getGlobalPosition() );
            pParticle->initialize();
            g_pBattlefield->addObject(pParticle);


            tankObject->setDeleteFlag( true );
        }
        break;
      }
  }


}

// Called when entering a state
void TankController::OnEnter()
{
  TankObject * tankObject = (TankObject *)g_pBattlefield->getObjectFromID(object_id);
  if (tankObject)
    {
      switch (state)
	{
          case 0:     // initializing state 
	    state = 1;  // switch to searching state
	    setNewWayPoint(tankObject);
	    break;

	  case 1:     // searching state
	    break;

          case 2:      // enemy spotted but out of range state
	    break;

	  case 3:       // enemy in range, attacking state

	    break;

	  case 4:      // dead state
	    break;

	default:

	  break;
	}

    }
}

// Called when leaving a state
void TankController::OnExit()
{

}


void TankController::checkCourseAndDirToWayPoint(TankObject * tankObject)
{

    CSP_LOG(CSP_APP, CSP_BULK, "TankController::checkCourseAndDirToWayPoint" );

  // dump position, direction and waypoint.
  //  cout << "Tank " << tankObject->object_id << " Pos: (" << 
  //    tankObject->position.x << "," << tankObject->position.y << "," << tankObject->position.z 
  //       << ") Dir: (" << tankObject->direction.x << "," << tankObject->direction.y 
  //       << "," << tankObject->direction.z << ") WayPoint: (" << target_position.x << ","
  //       << target_position.y << "," << target_position.z << ")" << endl;
  

  // position waypoint
  // calc dist to waypoint
  float diff = StandardVector3(target_position - tankObject->getGlobalPosition() ).Length();
  //  cout << "Tank " << tankObject->object_id << " is " << diff << "M from waypoint." << endl;
  if (diff < 5.0)           // use hardcoded waypoint tolerance for now.
    {
      CSP_LOG(CSP_APP, CSP_DEBUG, "Tank " << tankObject->getObjectID() << " has reached waypoint." << ", Time: " << g_pSimTime->getSimTime());
      tankObject->dump();
      setNewWayPoint(tankObject);     // we have reached waypoint. go to next one.
    }
    

  // are we directly facing target_position? 
	
  StandardVector3 distVect = target_position - tankObject->getGlobalPosition();
  float dist = distVect.Length();
  StandardVector3 waypoint_direction = Normalized(distVect);
  
  float angle = angleBetweenTwoVectors(tankObject->getDirection(), waypoint_direction);

  angle = angle*360/2.0/3.1415;

  if (angle > 360)
    angle = angle - 180;
  
  //  cout << "Tank " << tankObject->object_id << " is " << angle << " degrees from facing waypoint." << endl; 
  if (angle < 3.0 && angle > -3)         // use hardcoded waypoing tolerance for now.
    {
      //	cout << "Tank " << tankObject->object_id << " is facing waypoint, moving forward. Time=" << getCurTime() << "." << endl;

  //    if (tankObject->movement_state != 1)
      {
         CSP_LOG(CSP_APP, CSP_BULK, "Tank " << tankObject->getObjectID() << " is facing waypoint, moving forward. Distance to way point "
            << diff << ", Angle to Waypoint " << angle << ", Time: " << g_pSimTime->getSimTime());
         tankObject->setMovementState( 1);    // facing waypoint move forward
      }
    }
  else if (angle > 3.0 && angle <= 180.0)
    {
      // turn left
//      if (tankObject->movement_state != 4)
      {
        CSP_LOG(CSP_APP, CSP_BULK, "Tank " << 
            tankObject->getObjectID() << 
            " is turning left to face waypoint.  Distance to way point " 
            << diff << ", Angle to waypoint " << angle << ", Time: " << g_pSimTime->getSimTime());
        tankObject->setMovementState( 4);
      }
    }
  else
    {
      // turn right
 //     if (tankObject->movement_state != 3)
      {
        CSP_LOG(CSP_APP, CSP_BULK, "Tank " << tankObject->getObjectID() << 
            " is turning right to face waypoint. Distance to waypoint" 
            << diff << ", Angle to waypoint " << angle << ", Time: " << g_pSimTime->getSimTime());
        tankObject->setMovementState( 3);
      }
    }
	      
}


void TankController::setNewWayPoint(TankObject * tankObject)
{

  float rand_length = 2000.0*rand()/RAND_MAX;
  float rand_angle = 2.0*3.1415*rand()/RAND_MAX;

  cout << "SetNewWayPoint: " << rand_length << "," << rand_angle << endl; 

  StandardVector3 dir = StandardVector3(cos(rand_angle), sin(rand_angle), 0);
  target_position = tankObject->getGlobalPosition() + rand_length*dir;
  
  CSP_LOG(CSP_APP, CSP_INFO, "Setting a new random waypoint for tank " << tankObject->getObjectID() << " (" 
       << target_position.x << ", " << target_position.y << ", " << target_position.z << ")" << ", Time: " << g_pSimTime->getSimTime());




}

