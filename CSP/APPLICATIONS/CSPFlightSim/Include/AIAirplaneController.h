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
 * @file AIAirplaneController.h 
 *
 **/


#ifndef __AIAIRPLANECONTROLLER_H__
#define __AIAIRPLANECONTROLLER_H__

#include "BaseController.h"
#include "ObjectRangeInfo.h"
#include "AirplaneObject.h"

// alitude states
// 0 level flight
// 1 climb
// 2 dive

class Message;


/**
 * class AIAirplaneController - Describe me!
 *
 * @author unknown
 */
class AIAirplaneController : public BaseController
{

 public:

     enum {LEVEL, CLIMB, DIVE};

 public: 
  AIAirplaneController();
  virtual ~AIAirplaneController();

  virtual void OnUpdate(unsigned int dt);
  virtual void OnMessage(const Message * message);
  virtual void OnEnter();
  virtual void OnExit();
  virtual void Initialize();

  virtual void  SetAltitude(float alt)
  { targetAltitude = alt; }

 protected:

     float targetAltitude;
     float altitudeTol;

    int AltitudeState;
    // pitch angles in degrees
    float divePitch;      
    float climbPitch;
    float pitchTol;
    float targetPitch;


};

#endif // __AIAIRPLANECONTROLLER_H__
