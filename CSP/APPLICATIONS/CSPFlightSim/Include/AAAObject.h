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
 * @file AAAObject.h
 *
 **/


#ifndef __AAAOBJECT_H__
#define __AAAOBJECT_H__

using namespace std;

#include "BaseObject.h"
#include "AirplaneObject.h"
#include "AAAController.h"


/**
 * class AAAObject - Describe me!
 *
 * @author unknown
 */
class AAAObject : public BaseObject
{
 public:

  friend class AAAController;

  AAAObject();
  virtual ~AAAObject();

  virtual void dump();

  virtual void OnUpdate( float dt);
  virtual void initialize();
  virtual unsigned int OnRender();
  virtual int updateScene();


  virtual void setNamedParameter(const char * name, const char * value);

  void setPlayerPlane(AirplaneObject * pPlane);
  void checkForPlayerInRange();
  void fireWeapon();

 protected:
   AirplaneObject * p_PlayerPlane;
   int refire_time;
   bool weapon_ready;
   bool in_range;
};

#endif // __AAAOBJECT_H__
