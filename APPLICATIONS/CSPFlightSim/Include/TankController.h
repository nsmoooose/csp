// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file TankController.h
 *
 **/

#ifndef __TANKCONTROLLER_H__
#define __TANKCONTROLLER_H__

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

  std::list<ObjectRangeInfo*> enemyList;

  unsigned int state;
  unsigned int next_state;
  bool force_state_change;

  
  unsigned int waypoint_state;

  StandardVector3 target_position;
  //  StandardVector3 target_direction;



  
};

#endif // __TANKCONTROLLER_H__

