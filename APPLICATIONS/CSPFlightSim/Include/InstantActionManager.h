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
 * @file InstantActionManager.h
 *
 **/

#ifndef __INSTANTACTIONMANAGER_H__
#define __INSTANTACTIONMANAGER_H__

#include <BaseController.h>
#include <AirplaneObject.h>

using namespace std;

class InstantActionController;


/**
 * class InstantActionManager - Describe me!
 *
 * @author unknown
 */
class InstantActionManager : public BaseObject
{
 public:

  friend class InstantActionController;

  InstantActionManager();
  virtual ~InstantActionManager();

  virtual void dump();

  virtual void OnUpdate(float dt);

  virtual void Initialize();
  virtual unsigned int OnRender();
  virtual void SetNamedParameter(const char * name, const char * value);

  void SetPlayerPlane(AirplaneObject * pPlane);

 protected:
     AirplaneObject * p_PlayerPlane;
    float m_planeRange;
    float m_tankRange;

};

#endif // __INSTANTACTIONMANAGER_H__


