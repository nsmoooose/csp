#include <Framework.h>
#include <ObjectManager.h>
#include <TankController.h>
#include <Message.h>
#include <Tank.h>
#include <ObjectRangeInfo.h>
#include <math.h>

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
  Tank * tankObject = (Tank *)ObjectManager::getObjectManager().getObjectFromID(object_id);
  if (tankObject)
    {

      switch (state)
	{
          case 0:     // initializing state - this should be not reached
	    break;

	  case 1:     // searching state
	    {
	
	      // look for enemies
	      
	      ObjectManager::getObjectManager().clearObjectsInRangeList(enemyList);
	      ObjectManager::getObjectManager().getObjectsInRange(tankObject, 
								tankObject->max_viewing_range, 
								tankObject->max_viewing_angle , 
								enemyList );
	      ObjectManager::getObjectManager().dumpAllInfoInRangeList(enemyList);
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

			if (info->range < info_nearest->range)
			  {
			    info_nearest = info;
			  }
		      }

		    target_id = info_nearest->object_id;

		    // if nearest is in firing range switch attack state
		    if (info_nearest->range < tankObject->max_firing_range)
		      {
			cout << "Tank " << tankObject->object_id << " switching to state " << 3 << endl;
			state = 3;
		      }
		    // if not in firing range switch to approach range
		    else
		      {
			cout << "Tank " << tankObject->object_id << " switching to state " << 2 << endl;
			state = 2;

		      }


		}


	      checkCourseAndDirToWayPoint(tankObject);
	      
	      
	    
	      
	      break;
	    }
	  case 2:     // enemy spotted but out of range state
	    {
	      BaseObject * toObject = ObjectManager::getObjectManager().getObjectFromID(target_id);
	      Tank * toTankObject = (Tank*)toObject;
	      float distance;
	      StandardVector3 direction;
	      ObjectManager::getObjectManager().getObjectDistance(tankObject, toTankObject, distance, direction);
	      float angle = ObjectManager::getObjectManager().AngleBetweenTwoVectors(tankObject->direction, direction);
	 

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


void TankController::Initialize()
{
  Tank * tankObject = (Tank *)ObjectManager::getObjectManager().getObjectFromID(object_id);
  if (tankObject)
    {
      switch (state)
	{
          case 0:     // initializing state 
	    state = 1;  // switch to searching state
	    SetNewWayPoint(tankObject);
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
       << " from object " << msg->senderID << " at " << ObjectManager::getObjectManager().getCurTime() << endl;
}

// Called when entering a state
void TankController::OnEnter()
{
  Tank * tankObject = (Tank *)ObjectManager::getObjectManager().getObjectFromID(object_id);
  if (tankObject)
    {
      switch (state)
	{
          case 0:     // initializing state 
	    state = 1;  // switch to searching state
	    SetNewWayPoint(tankObject);
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


void TankController::checkCourseAndDirToWayPoint(Tank * tankObject)
{
  // dump position, direction and waypoint.
  //  cout << "Tank " << tankObject->object_id << " Pos: (" << 
  //    tankObject->position.x << "," << tankObject->position.y << "," << tankObject->position.z 
  //       << ") Dir: (" << tankObject->direction.x << "," << tankObject->direction.y 
  //       << "," << tankObject->direction.z << ") WayPoint: (" << target_position.x << ","
  //       << target_position.y << "," << target_position.z << ")" << endl;
  

  // position waypoint
  // calc dist to waypoint
  float diff = StandardVector3(target_position - tankObject->position).Length();
  //  cout << "Tank " << tankObject->object_id << " is " << diff << "M from waypoint." << endl;
  if (diff < 5.0)           // use hardcoded waypoint tolerance for now.
    {
      cout << "Tank " << tankObject->object_id << " has reached waypoint at time " << ObjectManager::getObjectManager().getCurTime() << "." << endl;
      tankObject->dump();
      SetNewWayPoint(tankObject);     // we have reached waypoint. go to next one.
    }
    

  // are we directly facing target_position? 
	
  StandardVector3 distVect = target_position - tankObject->position;
  float dist = distVect.Length();
  StandardVector3 waypoint_direction = Normalized(distVect);
  
  float angle = ObjectManager::getObjectManager().AngleBetweenTwoVectors(tankObject->direction, waypoint_direction);

  angle = angle*360/2.0/3.1415;

  if (angle > 360)
    angle = angle - 180;
  
  //  cout << "Tank " << tankObject->object_id << " is " << angle << " degrees from facing waypoint." << endl; 
  if (angle < 3.0 && angle > -3)         // use hardcoded waypoing tolerance for now.
    {
      //      if (movement_state != 1)
      //	cout << "Tank " << tankObject->object_id << " is facing waypoint, moving forward. Time=" << ObjectManager::getObjectManager().getCurTime() << "." << endl;
      tankObject->movement_state = 1;    // facing waypoint move forward
    }
  else if (angle > 3.0 && angle <= 180.0)
    {
      // turn left
      //      if (movement_state != 4)
      //	cout << "Tank " << tankObject->object_id << " is turning left to face waypoint. Time=" << ObjectManager::getObjectManager().getCurTime() << "." << endl;
      tankObject->movement_state = 4;
    }
  else
    {
      // turn right
      //      if (movement_state != 3)
      //	cout << "Tank " << tankObject->object_id << " is turning right to face waypoint. Time=" << ObjectManager::getObjectManager().getCurTime() << "." << endl;
      tankObject->movement_state = 3;
    }
	      
}


void TankController::SetNewWayPoint(Tank * tankObject)
{

  float rand_length = 200.0*rand()/RAND_MAX;
  float rand_angle = 2.0*3.1415*rand()/RAND_MAX;

  cout << "SetNewWayPoint: " << rand_length << "," << rand_angle << endl; 

  StandardVector3 dir = StandardVector3(cos(rand_angle), sin(rand_angle), 0);
  target_position = tankObject->position+rand_length*dir;
  
  cout << "Setting a new random waypoint for tank " << tankObject->object_id << " (" 
       << target_position.x << ", " << target_position.y << ", " << target_position.z << ")" << endl;;



}

