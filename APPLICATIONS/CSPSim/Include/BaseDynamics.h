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
 * @file BaseDynamics.h
 *
 **/

#ifndef __BASEDYNAMICS_H__
#define __BASEDYNAMICS_H__

#include <SimData/Vector3.h>

class simdata::Quaternion;

class BaseDynamics {
protected:
	simdata::Vector3 m_Force, m_Moment;
	simdata::Vector3 const *m_PositionLocal, *m_VelocityBody, *m_AngularVelocityBody;
	simdata::Quaternion const *m_qOrientation;
	double const *m_Height;
	bool const *m_NearGround;
	simdata::Vector3 const *m_NormalGround;
public:
	BaseDynamics();
	virtual void initializeSimulationStep(double dt);
	virtual void preSimulationStep(double dt);
	virtual void postSimulationStep(double dt);
	virtual void update(double dt) = 0;
	simdata::Vector3 getForce() const;
	simdata::Vector3 getMoment() const;
	void bindKinematics(simdata::Vector3 const &position_local, simdata::Vector3 const &velocity_body, 
					simdata::Vector3 const &angular_velocity_body, simdata::Quaternion const &orientation);
	void bindGroundParameters(bool const &near_ground, double const &height, simdata::Vector3 const &normal_ground);
	virtual ~BaseDynamics();
};


class InitializeSimulationStep {
	double m_dt;
public:
	InitializeSimulationStep(double dt): m_dt(dt){}
	void operator()(BaseDynamics *bd) {
		bd->initializeSimulationStep(m_dt);
	}
};


class PostSimulationStep {
	double m_dt;
public:
	PostSimulationStep(double dt): m_dt(dt){}
	void operator()(BaseDynamics *bd) {
		bd->postSimulationStep(m_dt);
	}
};


class PreSimulationStep {
	double m_dt;
public:
	PreSimulationStep(double dt): m_dt(dt){}
	void operator()(BaseDynamics *bd) {
		bd->preSimulationStep(m_dt);
	}
};

#endif //__BASEDYNAMICS_H__


