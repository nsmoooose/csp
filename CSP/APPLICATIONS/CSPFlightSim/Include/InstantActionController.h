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
 * @file InstantActionController.h
 *
 **/

#ifndef __INSTANTACTIONCONTROLLER_H__
#define __INSTANTACTIONCONTROLLER_H__


class BaseObject;
class Message;


/**
 * class InstantActionController - Describe me!
 *
 * @author unknown
 */
class InstantActionController : public BaseController
{
 public:

 public:
  InstantActionController();
  virtual ~InstantActionController();

  virtual void OnMessage(const Message *);
  virtual void OnUpdate(unsigned int dt);
  virtual void OnEnter();
  virtual void OnExit();
  virtual void Initialize();


 protected:
  int countPlanesInRange();
  int countTanksInRange();
  void generateNewEnemyPlanes();
  void generateNewEnemyTanks();
  void removeEnemyPlanesOutOfRange();
  void removeEnemyTanksOutOfRange();


};

#endif // __INSTANTACTIONCONTROLLER_H__

