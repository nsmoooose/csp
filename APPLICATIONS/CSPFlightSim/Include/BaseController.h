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
 * @file BaseController.h
 *
 **/


#ifndef __BASECONTROLLER_H__
#define __BASECONTROLLER_H__


class BaseObject;
class Message;


/**
 * class BaseController - Describe me!
 *
 * @author unknown
 */
class BaseController
{
 public:
  unsigned int object_id;      // the game object this controller is in charge of.
  unsigned int controller_id;
  unsigned int controller_type;

 public:
  BaseController();
  virtual ~BaseController();

  virtual void OnMessage(const Message *) = 0;
  virtual void OnUpdate(unsigned int dt) = 0;
  virtual void OnEnter() = 0;
  virtual void OnExit() = 0;
  virtual void Initialize() = 0;

};

#endif // __BASECONTROLLER_H__

