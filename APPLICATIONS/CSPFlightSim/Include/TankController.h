#ifndef TANKCONTROLLER_H__
#define TANKCONTROLLER_H__

#include "BaseController.h"
#include "ObjectRangeInfo.h"
#include "TankObject.h"


// states for the tank object

// 0 - initializing
// 1 - searching
// 2 - enemy spotted out of range
// 3 - enemy in range attack
// 4 - dead

// movement states for the tank object
// 0 - stopped
// 1 - forward
// 2 - backward
// 3 - turning right
// 4 - turning left

// waypoint state
// 0 - no waypoint
// 1 - position waypoint
// 2 - direction waypoint
// 2 - next waypoint

class Message;


/**
 * class TankController - Describe me!
 *
 * @author unknown
 */
class TankController : public BaseController
{

 public:

  

 public: 
  TankController();
  virtual ~TankController();

  virtual void OnUpdate(unsigned int dt);
  virtual void OnMessage(const Message * message);
  virtual void OnEnter();
  virtual void OnExit();
  virtual void initialize();


 protected:

  void setNewWayPoint(TankObject * tankObject);
  void checkCourseAndDirToWayPoint(TankObject * tankObject);

  unsigned int target_id;

  list<ObjectRangeInfo*> enemyList;

  unsigned int state;
  unsigned int next_state;
  bool force_state_change;

  
  unsigned int waypoint_state;

  StandardVector3 target_position;
  //  StandardVector3 target_direction;



  
};

#endif
