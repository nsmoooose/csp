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

#include "CSPSim.h"
#include "Engine.h"
#include "ConditionsChannels.h"
#include "FlightDynamicsChannels.h"

#include <SimData/Conversions.h>
#include <SimData/Quat.h>
#include <SimData/ObjectInterface.h>

#include <iomanip>
#include <sstream>


SIMDATA_XML_BEGIN(ThrustData)
	SIMDATA_DEF("idle_thrust", m_idle_thrust, true)
	SIMDATA_DEF("mil_thrust", m_mil_thrust, true)
	SIMDATA_DEF("ab_thrust", m_ab_thrust, true)
SIMDATA_XML_END

SIMDATA_XML_BEGIN(Engine)
	SIMDATA_DEF("thrust_data", m_ThrustData, true)
	SIMDATA_DEF("engine_idle_rpm", m_EngineIdleRpm, true)
	SIMDATA_DEF("engine_ab_thrust", m_EngineAbRpm, true)
	SIMDATA_DEF("thrust_direction", m_ThrustDirection, true)
	SIMDATA_DEF("engine_offset", m_EngineOffset, true)
	SIMDATA_DEF("smoke_emitter_location", m_SmokeEmitterLocation, true)
SIMDATA_XML_END

SIMDATA_XML_BEGIN(EngineDynamics)
	SIMDATA_DEF("engine_set", m_Engine, true)
SIMDATA_XML_END


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
	// XMLize: A, B & C should be functions of mach
	m_A = simdata::toRadians(10.0);
	m_B = simdata::toRadians(15.0);
	m_C = simdata::toRadians(40.0);
}

void EngineDynamics::registerChannels(Bus *) {
}

void EngineDynamics::importChannels(Bus *bus) {
	if (!bus) return;
	b_ThrottleInput = bus->getChannel("ControlInputs.ThrottleInput");
	b_Mach = bus->getChannel(bus::Conditions::Mach);
	b_Alpha = bus->getChannel(bus::FlightDynamics::Alpha);
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
		throttle = throttle * 1.05263f + std::max(0.0f, throttle-0.95f) * 20.0f;
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

double EngineDynamics::flatten(double x) {
	// an increasing picewise linear continuous function f on R such that:
	//		  / -pi/2 if x = -a
	// f(x) = | 0     if x in [-a/2,b/2] (0 < a, b)
	//        \ b     if x = b 
	double ret = std::abs(x-m_B/2) + x-m_B/2 + simdata::PI_2*(m_A/2+x - std::abs(m_A/2+x))/m_A;
	return ret;
}

void EngineDynamics::cut() {
	double alpha = b_Alpha->value();
	// f is a totally ad-hoc means of preventing a slight tailwind from killing
	// the engine when we aren't moving.
	double f = simdata::clampTo(4.0 * (b_Mach->value() - 0.25), 0.0, 1.0);
	if ((std::abs(alpha) > simdata::PI_2) || (alpha < -m_A)) {
		m_Force *= (1.0 - f);
	}
	else if (alpha < m_B) {
		m_Force *= (1.0 - f) + f * cos(flatten(alpha));
 	}
	else if (alpha < m_C) {
		m_Force *= (1.0 - f) + f * cos(m_B + (simdata::PI_2 - m_B)*(alpha - m_B)/(m_C-m_B));
	}
	else {
		m_Force *= (1.0 - f);
	}
}

void EngineDynamics::computeForceAndMoment(double /*x*/) {
	// all the work is done by preSimulationStep
	cut();
}

void EngineDynamics::getInfo(InfoList &info) const {
	std::stringstream line;
	line.setf(std::ios::fixed | std::ios::showpos);
	line << "Throttle: " << std::setprecision(2) << std::setw(6) << b_ThrottleInput->value()
	     << ", Thrust: " << std::setprecision(0) << std::setw(8) << m_Force.length();
	info.push_back(line.str());
}

