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

SIMDATA_REGISTER_INTERFACE(ThrustData)

ThrustData::ThrustData() {
}

ThrustData::~ThrustData() {
}

void ThrustData::pack(simdata::Packer &p) const { 
    simdata::Object::pack(p); 
    p.pack(m_idle_thrust); 
    p.pack(m_mil_thrust);
	p.pack(m_ab_thrust);
} 

void ThrustData::unpack(simdata::UnPacker &p) { 
    simdata::Object::unpack(p); 
    p.unpack(m_idle_thrust); 
    p.unpack(m_mil_thrust);
	p.unpack(m_ab_thrust);
}  

float ThrustData::getMil(float altitude, float mach) const {
	return m_mil_thrust.getValue(mach, altitude);
}

float ThrustData::getIdle(float altitude, float mach) const {
	return m_idle_thrust.getValue(mach, altitude);
}

float ThrustData::getAb(float altitude, float mach) const {
	return m_ab_thrust.getValue(mach, altitude);
}


SIMDATA_REGISTER_INTERFACE(Engine)

Engine::Engine(simdata::Vector3 const &thrustDirection) {
}

Engine::~Engine() {
}

void Engine::pack(simdata::Packer &p) const { 
    simdata::Object::pack(p); 
	p.pack(m_ThrustData);
    p.pack(m_EngineIdleRpm); 
	p.pack(m_EngineAbRpm);
	p.pack(m_ThrustDirection);
	p.pack(m_EngineOffset);
	p.pack(m_SmokeEmitterLocation);
} 

void Engine::unpack(simdata::UnPacker &p) { 
    simdata::Object::unpack(p); 
	p.unpack(m_ThrustData);
    p.unpack(m_EngineIdleRpm); 
	p.unpack(m_EngineAbRpm);
	p.unpack(m_ThrustDirection);
	p.unpack(m_EngineOffset);
	p.unpack(m_SmokeEmitterLocation);
} 

void Engine::setThrustDirection(simdata::Vector3 const& thrustDirection) {
	m_ThrustDirection = thrustDirection;
}

void Engine::bindThrottle(float const &throttle) {
	m_Throttle = &throttle;
}

void Engine::setMach(float mach) {
	m_Mach = mach;
}

void Engine::setAltitude(float altitude) {
	m_Altitude = altitude;
}

simdata::Vector3 const &Engine::getSmokeEmitterLocation() const {
	return m_SmokeEmitterLocation;
}

simdata::Vector3 Engine::getThrust() const {
	float throttle = *m_Throttle;
	if (throttle < 0.0)
		throttle = 0.0;
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
 

SIMDATA_REGISTER_INTERFACE(EngineDynamics)

void EngineDynamics::pack(simdata::Packer &p) const { 
    simdata::Object::pack(p); 
	p.pack(m_Engine);
} 

void EngineDynamics::unpack(simdata::UnPacker &p) { 
    simdata::Object::unpack(p); 
	p.unpack(m_Engine);
} 

void EngineDynamics::postCreate() { 
	if (!m_Engine.empty()) {
		simdata::Link<Engine>::vector::iterator i = m_Engine.begin();
		simdata::Link<Engine>::vector::const_iterator iEnd = m_Engine.end();
			for (; i !=iEnd; ++i)
				(*i)->bindThrottle(m_Throttle);
	}
}

EngineDynamics::EngineDynamics():
	m_Throttle(0.0f) {
}

void EngineDynamics::setThrottle(double const throttle) {
	m_Throttle = static_cast<float>(throttle);
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
		float altitude = static_cast<float>(m_PositionLocal->z);
		float speed = static_cast<float>(m_VelocityBody->Length());
		float mach = static_cast<float>(CSPSim::theSim->getAtmosphere()->getMach(speed, altitude));
		simdata::Link<Engine>::vector::iterator i = m_Engine.begin();
		simdata::Link<Engine>::vector::const_iterator iEnd = m_Engine.end();
		for (; i !=iEnd; ++i) {
			(*i)->setMach(mach);
			(*i)->setAltitude(altitude);
			simdata::Vector3 force = (*i)->getThrust();
			m_Force += force;
			m_Moment += (*i)->m_EngineOffset^force;
		}
	}
}

void EngineDynamics::computeForceAndMoment(double x) {
	// all the work is done by preSimulationStep
}

