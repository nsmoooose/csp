#include <ObjectRangeInfo.h>
#include <ObjectManager.h>
#include <BaseObject.h>
#include <Tank.h>
#include <Message.h>

#include <string>
#include <sstream>
#include <iostream>

using namespace std;

unsigned int ObjectManager::latest_object_id = 0;
ObjectManager * ObjectManager::ms_manager = 0;
unsigned int sim_time = 0;

void ObjectManager::addObject(BaseObject * obj)
{
  cout  << "ObjectManager::addObject" << endl;
  objectList.push_front(obj);
  ostringstream strg;
  strg << "Adding object " << obj->object_id << " at time " << getCurTime();
  objectListHistory.push_back(strg.str());
}


// look for the object given an id, return null if object
// is not found, in the future to speed this up a map could
// be used.
BaseObject * ObjectManager::getObjectFromID( unsigned int ID)
{
  
  cout << "ObjectManager::getObjectFromID() - ID: " << ID << endl;
  list<BaseObject *>::iterator i;
  for (i = objectList.begin() ; i != objectList.end() ; ++i)
    {
      BaseObject * object = *i;
      if (ID == object->object_id)
	return object;
    }
  return NULL;

}





// removing object with ID. 
// change the deleteFlag to true.
void ObjectManager::removeObjectWithID( unsigned int ID)
{
  cout << "removeObjectWithID - ID: " << ID << endl;

  list<BaseObject *>::iterator i;
  for (i = objectList.begin(); i != objectList.end(); ++i)
    {
      BaseObject * object = *i;
      if (object)
	{
	  if (ID == object->object_id)
	    {
	      object->deleteFlag = true;
	    }
	}
    }

  cout << "removeObjectWithID - checking list" << endl;
  dumpAllObjects();
  
  cout << "removeObjectWithID - finishing" << endl;

}

void ObjectManager::removeObjectsMarkedForDelete()
{
  cout << "removeObjectsMarkedForDelete() - entering" << endl;
  list<BaseObject *>::iterator i;
      
  for (i = objectList.begin() ; i != objectList.end() ; ++i)
    {
      BaseObject * object = *i;
      if (object)
	{
	  if (object->deleteFlag)
	    {
	      ostringstream strg;
	      strg << "Removing object " << object->object_id << " at time " << getCurTime();
	      objectListHistory.push_back(strg.str());

	      delete (object);
	      i = objectList.erase(i);
	    }
	}
      else
	cerr << "removeObjectsMarkedForDelete - found null object in objectList" << endl;
    }
  

  cout << "removeObjectsMarkedForDelete() - exiting" << endl;

}

void ObjectManager::dumpAllObjects()
{
  cout << "dumpAllObjects() - Dumping List of Game Objects" << endl;
  list<BaseObject *>::iterator i;
  for (i = objectList.begin() ; i != objectList.end() ; ++i)
    {
      BaseObject * object = *i;
      if (object)
	object->dump();
      else
	cerr << "dumpAllObjects - Found null object in the object list." << endl;
    }

  cout << "dumpAllObjects() - Finished Dumping List of Game Objects" << endl;

}

void ObjectManager::dumpObjectHistory()
{
  cout << "dumpAllObjects() - Dumping object history" << endl;
  list<string>::iterator i;
  for (i = objectListHistory.begin(); i != objectListHistory.end(); ++i)
    {
      cout << *i << endl;

    }
  cout << "dumpAllObjects() - Finished dumping object history" << endl;
}

// process the OnUpdate of all game objects.
void ObjectManager::updateAllObjects(unsigned int dt)
{
  cout << "processOnUpdate - Entering" << endl;

  list<BaseObject *>::iterator i;
      
  for (i = objectList.begin() ; i != objectList.end() ; ++i)
    {
      BaseObject * object = *i;
      if (object)
	{
	  object->OnUpdate(dt);
	}
      else
	cerr << "processOnUpdate - found null pointer in object list" << endl;
    }

  cout << "processOnUpdate - Leaving" << endl;

}

void ObjectManager::initializeAllObjects()
{
  cout << "ObjectManager::initializeAllObjects" << endl;
  list<BaseObject *>::iterator i;
  i = objectList.begin();
  for (i = objectList.begin() ; i != objectList.end() ; ++i)
    {
      BaseObject * object = *i;
      if (object)
	{
	  object->Initialize();
	}
      else
	cerr << "initializeGameObjects - null object found in objectList" << endl;
    }
}


// return the current simulation time.
unsigned int ObjectManager::ObjectManager::getCurTime()
{
  return sim_time;
}



void ObjectManager::getObjectDistance(BaseObject * fromObject, BaseObject * toObject, float & distance, StandardVector3 & direction)
{
  Tank * fromTankObject = (Tank*)fromObject;
  Tank * toTankObject   = (Tank*)toObject;

  StandardVector3 distVect = fromTankObject->position - toTankObject->position;
  distance = distVect.Length();
  direction = Normalized(distVect);
  
}



/*! \fn float AngleBetweenTwoVectors(const StandardVector3 & v1, const StandardVector3 & v2)
 *
 * Calculates the angle between two vectors. This routine is assuming both vectors are in the
 * x-y plane. returned angle is in radians.
 */
float ObjectManager::AngleBetweenTwoVectors(const StandardVector3 & v1, const StandardVector3 & v2)
{
  float cosa, sina;
  cosa = Dot(v1, v2);
  StandardVector3 crossvec = Cross(v1, v2);
  sina = crossvec.z;

  // if the cross product is negative then the angle is negative.
  if (sina >= 0)
    return acos(cosa);
  else
    return -acos(cosa);
  
}

void ObjectManager::clearObjectsInRangeList(list<ObjectRangeInfo*> &rangeList)
{
  while (!(rangeList.empty()))
    {
      ObjectRangeInfo * objInfo = rangeList.front();
      rangeList.pop_front();

      // todo use a better memory scheme
      delete (objInfo);
    }

}

void ObjectManager::dumpAllInfoInRangeList(list<ObjectRangeInfo*> &rangeList)
{
  list<ObjectRangeInfo*>::iterator i;
  for(i = rangeList.begin(); i != rangeList.end() ; ++i)
    {
      (*i)->dump();
    }
}

// calculates a list of objects within range from a centralObject.
void ObjectManager::getObjectsInRange(BaseObject * fromObject, float range, float view_angle  , list<ObjectRangeInfo*> & rangeList  )
{
  list<BaseObject *>::iterator i;
  Tank * fromTankObj = (Tank*)fromObject;
  
  for (i = objectList.begin() ; i != objectList.end() ; ++i)
    {
      Tank * toTankObj = (Tank*)*i;
      if (fromTankObj->object_id != toTankObj->object_id)
	{
	  StandardVector3 distVect = toTankObj->position - fromTankObj->position;
	  float dist = distVect.Length();
	  StandardVector3 distVectNormalized = Normalized(distVect);
	  float angle = AngleBetweenTwoVectors(fromTankObj->direction, distVectNormalized);
	  angle = angle*360.0/3.1415/2.0;
	  
	  //	  cout << "Distance between objects " << fromTankObj->object_id << " and "
	  //	       << toTankObj->object_id << " is " << dist << ", angle is " << angle*360.0/2.0/3.1415 << endl;
	  if (dist < range && fabs(angle) < view_angle/360.0*2.0*3.1415)
	    {
	      //   cout << "Object " << toTankObj->object_id << " is in sight from object " << fromTankObj->object_id << endl;

	      // todo use a better memory scheme
              ObjectRangeInfo * objectRangeInfo = new ObjectRangeInfo;
              objectRangeInfo->object_id = toTankObj->object_id;
              objectRangeInfo->range = dist;
              objectRangeInfo->direction = distVectNormalized;
	      rangeList.push_front(objectRangeInfo);
	    }
	  //	  else
	  //	    {
	  //	      cout << "Object " << toTankObj->object_id << " is not in sight from object " << fromTankObj->object_id << endl;
	  //	    }	      

	}
    }

}

// store a message in the delayed message list.
void ObjectManager::storeDelayedMsg( Message * msg)
{
  cout << "Storing Delayed Message" << endl;
  msg->dump();
  delayedMessageList.push(msg);
}

// dump all the message in the delayed message list.
void ObjectManager::dumpDelayedMsgList()
{
  cout << "Dumping Delayed Message List" << endl;
  while(!delayedMessageList.empty())
    {
      Message * message = delayedMessageList.top();
      message->dump();
      delayedMessageList.pop();
    }
}

// a priority queue would speed this up.
void ObjectManager::processDelayedMsgList()
{
  if (!delayedMessageList.empty())
    {
      unsigned int curTime = getCurTime();
      Message * message = delayedMessageList.top();
      if (message->receiveTime <= curTime)
	{
	  cout << "Processing delayed message, curTime " << curTime << endl;
	  delayedMessageList.pop();
	  message->dump();
	  routeMessage(message);
	}
    }
  
}

void ObjectManager::routeMessage( Message* msg)
{

  cout << "routeMessage - Entering" << endl;

  BaseObject * object = ObjectManager::getObjectManager().getObjectFromID( msg->receiverID );

  // check to make sure we found an object. We could have been 
  // given a bogus ID or the object could be gone.
  if (!object)
    {
      cerr << "routeMessage - Message is for unknown object" << endl;
      return;
    }

  // if the message needs to be receive in the future store
  // it in the delayed message list.
  if (msg->receiveTime > getCurTime())
    {
      storeDelayedMsg(msg);
      return;
    }

  object->ReceiveMessage(*msg);

  //  // send message to the stateMachine/controller of the object
  //  BaseController * controller = getControllerFromID( object->controller_id);
  //  if (controller)
  //    controller->OnMessage(msg);
  //  else
  //    cerr << "routeMessage - failed to find controller for object " << object->object_id << endl;

  cout << "routeMessage - Leaving" << endl;

}

// send a message to an object
void ObjectManager::sendMsg(int type, unsigned int senderID, unsigned int receiverID, int data)
{
  Message * msg = new Message;
  msg->type = type;
  msg->senderID = senderID;
  msg->receiverID = receiverID;
  msg->sentTime = getCurTime();
  msg->receiveTime = getCurTime();
  msg->data = data;

  routeMessage( msg );

}

//send a delayed message to an object, the message will be received at the time (curTime+delay)
void ObjectManager::sendDelayedMsg(int type, unsigned int senderID, unsigned int receiverID, unsigned int delay, int data)
{
  Message * msg = new Message;
  msg->type = type;
  msg->senderID = senderID;
  msg->receiverID = receiverID;
  msg->sentTime = getCurTime();
  msg->receiveTime = getCurTime() + delay;
  msg->data = data;

  routeMessage( msg );

}

