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
 * @file ProjectileController.h
 *
 **/

#ifndef __PROJECTILECONTROLLER_H__
#define __PROJECTILECONTROLLER_H__

#include "BaseController.h"
#include "ProjectileObject.h"


/**
 * class ProjectileController - Describe me!
 *
 * @author unknown
 */
class ProjectileController : public BaseController
{
 public:
  
 

 public:
  ProjectileController();
  virtual ~ProjectileController();

  virtual void OnUpdate(unsigned int dt);
  virtual void OnMessage(const Message * message);
  virtual void OnEnter();
  virtual void OnExit();
  virtual void Initialize();
  


};

#endif // __PROJECTILECONTROLLER_H__

