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
 * @file AirplaneObject.h
 *
 **/


#ifndef __AIRPLANEOBJECT_H__
#define __AIRPLANEOBJECT_H__

#include "BaseObject.h"
#include "AirplanePhysics.h"
#include "AeroParam.h"
#include "CockpitDrawable.h"
#include "Hud.h"


/**
 * class AirplaneObject - Describe me!
 *
 * @author unknown
 */
class AirplaneObject : public BaseObject
{
 public:

  

  AirplaneObject();
  virtual ~AirplaneObject();
  virtual void dump();
  virtual void OnUpdate(double dt);
  virtual void initialize();
  void initializeHud();
  virtual int updateScene();

  virtual unsigned int OnRender();

  void doSimplePhysics(double dt);
  void doComplexPhysics(double dt);
  void doMovement(double dt);

  void setElevator(double setting);
  double getElevator() const;

  void setAileron(double setting);
  double getAileron() const; 

  void setRudder(double setting);
  double getRudder() const; 

  void setThrust(double setting);
  double getThrust() const;

  void setThrottle(double setting);
  double getThrottle() const;

  double getAngleOfAttack() { return m_AngleOfAttack; }

  void setGForce(double p_gForce) {m_gForce = p_gForce;};
  virtual double getGForce() {return m_gForce;};

  void setComplexPhysics(bool flag) { m_bComplex = flag; }

  void setAero( AeroParam * pAeroParam );

  Hud * GetpHud() const { return m_phud;};

protected:


 
  Hud * m_phud;
  AirplanePhysics * m_pAirplanePhysics;
  bool m_bPhysicsInitialized;
  bool m_bComplex;

  double m_ThrustMin;
  double m_ThrustMax;
  //double m_Alevator;
  double m_AileronMin;
  double m_AileronMax;
  double m_ElevatorMin;
  double m_ElevatorMax;

  StandardVector3 m_AngularVelocity;

  StandardVector3 m_ForceBody;

  StandardVector3 m_CurrentForceTotal;
  StandardVector3 m_GravityForce;
  StandardVector3 m_ThrustForce;
  StandardVector3 m_LiftForce;
  StandardVector3 m_DragForce;
  double m_gForce;
  
  double plusplus;
  double sensativity;
  double planesensx, planesensy;
  double m_ThrottleFactor;

  double m_ElevatorKey;
  double m_AileronKey;
  double m_Throttle;

  double m_AngleOfAttack;
  double m_SideSlip;
  double m_Thrust;
  double m_Aileron;
  double m_Elevator;
  double m_Rudder;
};

#endif // __AIRPLANEOBJECT_H__

