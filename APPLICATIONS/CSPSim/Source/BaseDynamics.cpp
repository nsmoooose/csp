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
 * @file BaseDynamics.cpp
 *
 **/

#include "BaseDynamics.h"
#include "PhysicsModel.h"


BaseDynamics::BaseDynamics():
	m_Force(simdata::Vector3::ZERO),
	m_Moment(simdata::Vector3::ZERO) 
{
}

BaseDynamics::~BaseDynamics() {
}

void BaseDynamics::bindKinematics(simdata::Vector3 const &position_local, 
                                  simdata::Vector3 const &velocity_body, 
                                  simdata::Vector3 const &angular_velocity_body, 
                                  simdata::Quaternion const &orientation) 
{
	m_PositionLocal = &position_local;
	m_VelocityBody = &velocity_body;
	m_AngularVelocityBody = &angular_velocity_body;
	m_qOrientation = &orientation;
}

void BaseDynamics::bindGroundParameters(bool const &near_ground, 
                                        double const &height, 
                                        simdata::Vector3 const &normal_ground) 
{
	m_NearGround = &near_ground;
	m_Height = &height;
	m_NormalGround = &normal_ground;
}

void BaseDynamics::bindAeroParameters(double const &qbar, 
                                      simdata::Vector3 const &wind_body) 
{
	m_qBar = &qbar;
	m_WindBody = &wind_body;
}

void BaseDynamics::initializeSimulationStep(double dt) 
{
}

void BaseDynamics::postSimulationStep(double dt) 
{
}

void BaseDynamics::preSimulationStep(double dt) 
{
}


