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
 * @file MissileObject.h
 *
 **/

#ifndef __MISSILEOBJECT_H__
#define __MISSILEOBJECT_H__

#include "BaseObject.h"

/**
 * class MissileObject
 *
 * @author unknown
 */
class MissileObject : public BaseObject
{
 public:

  MissileObject();
  virtual ~MissileObject();
  virtual void dump();
  virtual void OnUpdate(double dt);
  virtual void initialize();
  virtual unsigned int OnRender();
  void doSimplePhysics(double dt);

  virtual int updateScene();

  void doMovement(double dt);

 protected:


  double m_Thrust;
  double m_ThrustMin;
  double m_ThrustMax;
  double m_Aileron;
  double m_Elevator;
  double m_AileronMin;
  double m_AileronMax;
  double m_ElevatorMin;
  double m_ElevatorMax;
  StandardVector3 m_CurrentForceTotal;
  StandardVector3 m_GravityForce;
  StandardVector3 m_ThrustForce;
  StandardVector3 m_LiftForce;
  StandardVector3 m_DragForce;
  
  double plusplus;
  double sensativity;
  double planesensx, planesensy;

  double m_ElevatorKey;
  double m_AileronKey;

  double m_Active;
  double m_MaxFlightTime;
  double m_FlightTime;

  int particleCount;
  int particleMax;
  int particlePos;
  StandardVector3 * m_ParticlePosition;

};

#endif // __MISSILEOBJECT_H__

