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
 * @file Engine.cpp
 *
 **/
#include <SimData/Conversions.h>
#include <SimData/Quat.h>

#include "CSPSim.h"
#include "Engine.h"
#include "KineticsChannels.h"

#include <iomanip>
#include <sstream>

using bus::Kinetics;


SIMDATA_REGISTER_INTERFACE(ThrustData)
SIMDATA_REGISTER_INTERFACE(Engine)
SIMDATA_REGISTER_INTERFACE(EngineDynamics)


ThrustData::ThrustData() {
}

ThrustData::~ThrustData() {
}

float ThrustData::getMil(float altitude, float mach) const {
	return m_mil_thrust[altitude][mach];
}

float ThrustData::getIdle(float altitude, float mach) const {
	return m_idle_thrust[altitude][mach];
}

float ThrustData::getAb(float altitude, float mach) const {
	return m_ab_thrust[altitude][mach];
}



Engine::Engine(simdata::Vector3 const &/*thrustDirection*/) {
}

Engine::~Engine() {
}

void Engine::setThrustDirection(simdata::Vector3 const& thrustDirection) {
	m_ThrustDirection = thrustDirection;
}

simdata::Vector3 const &Engine::getSmokeEmitterLocation() const {
	return m_SmokeEmitterLocation;
}

simdata::Vector3 Engine::getThrust() const {
	float throttle = m_Throttle;
	if (throttle < 0.0) {
		throttle = 0.0;
	}
	float milComponent = m_ThrustData->getMil(m_Altitude, m_Mach);
	float other = 0.0;
	if (throttle <= 1.0)
		other = m_ThrustData->getIdle(m_Altitude, m_Mach);
	else {
		other = m_ThrustData->getAb(m_Altitude, m_Mach);
		throttle = 2.0f - throttle;
	}
	return (throttle * milComponent + (1.0f - throttle) * other) * m_ThrustDirection;
}

//float Thrust::getThrustByrpm(float rpm, float mach, float altitude) const {
//	float thrust = 0.0;
//	if (rpm < m_engine_idle_rpm) {
//			float factor = rpm / m_engine_idle_rpm;
//			thrust	= getThrustByThrottle(0.0, mach, altitude) * factor;
//	}
//	else if (rpm <=	1.0) {
//			float power = (rpm - m_engine_idle_rpm)	/ (1.0f - m_engine_idle_rpm);
//			thrust	= getThrustByThrottle(power, mach, altitude);
//	}
//	else {
//			float power =	1.0f	+ (rpm - 1.0f) /	(m_engine_ab_rpm - 1.0f);
//			thrust	= getThrustByThrottle(power, mach, altitude);
//	}
//	return thrust;
//}


void EngineDynamics::postCreate() {
}

void EngineDynamics::registerChannels(Bus *) {
}

void EngineDynamics::importChannels(Bus *bus) {
	if (!bus) return;
	b_ThrottleInput = bus->getChannel("ControlInputs.ThrottleInput");
	b_Mach = bus->getChannel("Conditions.Mach");
}

EngineDynamics::EngineDynamics() {
}

std::vector<simdata::Vector3> EngineDynamics::getSmokeEmitterLocation() const {
	std::vector<simdata::Vector3> smoke_emitter_location;
	if (!m_Engine.empty()) {
		simdata::Link<Engine>::vector::const_iterator i = m_Engine.begin();
		simdata::Link<Engine>::vector::const_iterator iEnd = m_Engine.end();
			for (; i !=iEnd; ++i)
				smoke_emitter_location.push_back((*i)->getSmokeEmitterLocation());
	}
	return smoke_emitter_location;
}

void EngineDynamics::preSimulationStep(double dt) {
	BaseDynamics::preSimulationStep(dt);
	m_Force = m_Moment = simdata::Vector3::ZERO;
	if (!m_Engine.empty()) {
		float altitude = static_cast<float>(m_PositionLocal->z());
		float mach = static_cast<float>(b_Mach->value());
		float throttle = static_cast<float>(b_ThrottleInput->value());
		throttle = throttle * 1.05263 + std::max(0.0, throttle-0.95) * 20.0;
		EngineSet::iterator i = m_Engine.begin();
		EngineSet::const_iterator iEnd = m_Engine.end();
		for (; i !=iEnd; ++i) {
			(*i)->setMach(mach);
			(*i)->setAltitude(altitude);
			(*i)->setThrottle(throttle);
			simdata::Vector3 force = (*i)->getThrust();
			m_Force += force;
			m_Moment += (*i)->m_EngineOffset^force;
		}
	}
}

void EngineDynamics::computeForceAndMoment(double /*x*/) {
	// all the work is done by preSimulationStep
}

void EngineDynamics::getInfo(InfoList &info) const {
	std::stringstream line;
	line.setf(std::ios::fixed | std::ios::showpos);
	line << "Throttle: " << std::setprecision(2) << std::setw(6) << b_ThrottleInput->value()
	     << ", Thrust: " << std::setprecision(0) << std::setw(8) << m_Force.length();
	info.push_back(line.str());
}

