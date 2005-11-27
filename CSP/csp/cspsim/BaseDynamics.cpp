// Combat Simulator Project
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

#include <csp/cspsim/BaseDynamics.h>
#include <csp/cspsim/PhysicsModel.h>

CSP_NAMESPACE


BaseDynamics::BaseDynamics():
	m_Force(Vector3::ZERO),
	m_Moment(Vector3::ZERO)
{
}

BaseDynamics::~BaseDynamics() {
}


void BaseDynamics::bindKinematics(Vector3 const &position_local,
                                  Vector3 const &velocity_body,
                                  Vector3 const &angular_velocity_body,
                                  Quat const &attitude,
                                  Vector3 const &center_of_mass_offset_local)
{
	m_PositionLocal = &position_local;
	m_VelocityBody = &velocity_body;
	m_AngularVelocityBody = &angular_velocity_body;
	m_Attitude = &attitude;
	m_CenterOfMassOffsetLocal = &center_of_mass_offset_local;
}


void BaseDynamics::initializeSimulationStep(double /*dt*/)
{
}

void BaseDynamics::postSimulationStep(double /*dt*/)
{
}

void BaseDynamics::preSimulationStep(double /*dt*/)
{
}


CSP_NAMESPACE_END

