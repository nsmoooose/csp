#ifndef _OBJECTMANAGER_H__
#define _OBJECTMANAGER_H__

#include <BaseObject.h>
#include <BaseController.h>
#include <Message.h>

#include <ObjectRangeInfo.h>
#include <list>
#include <queue>

class ObjectManager
{
 protected:
  static ObjectManager * ms_manager;
  list<BaseObject *> objectList;

  static unsigned int latest_object_id;
  list<string> objectListHistory;
  priority_queue<Message *, vector<Message*>, messageComparison > delayedMessageList;

  ObjectManager() { ms_manager = this; }

 public:
  ~ObjectManager() { ms_manager = 0; }

  static ObjectManager& getObjectManager() 
    { if (ms_manager == NULL) 
         ms_manager = new ObjectManager; 
      return *ms_manager; 
    }

  static ObjectManager* getObjectManagerPtr() 
    { 
      if (ms_manager == NULL)
	ms_manager = new ObjectManager;
      return ms_manager; 
    }

  void addObject(BaseObject *);
  unsigned int getNewObjectID() { return ++latest_object_id; }

  BaseObject * getObjectFromID( unsigned int ID);

  void removeObjectWithID( unsigned int ID);
  void removeObjectsMarkedForDelete();

  void dumpAllObjects();
  void updateAllObjects(unsigned int dt);
  void dumpObjectHistory();


  void getObjectsInRange(BaseObject * fromObject, float range, float view_angle , list<ObjectRangeInfo*> & rangeList );
  void initializeAllObjects();
    

  void routeMessage(Message *);
  void processDelayedMsgList();
  void sendDelayedMsg(int type, unsigned int senderID, unsigned int receiverID, unsigned int delay, int data);
  void sendMsg(int type, unsigned int senderID, unsigned int receiverID, int data);
  void storeDelayedMsg( Message * msg);
  void dumpDelayedMsgList();

  unsigned int getCurTime();
  void clearObjectsInRangeList(list<ObjectRangeInfo*> &rangeList);
  void dumpAllInfoInRangeList(list<ObjectRangeInfo*> &rangeList);
  void getObjectDistance(BaseObject * fromObject, BaseObject * toObject, float & distance, StandardVector3 & direction);
  float AngleBetweenTwoVectors(const StandardVector3 & v1, const StandardVector3 & v2);

 private:


};


#endif
