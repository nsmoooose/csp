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
 * @file Engine.cpp
 *
 **/

#include <csp/cspsim/Engine.h>
#include <csp/cspsim/ConditionsChannels.h>
#include <csp/cspsim/FlightDynamicsChannels.h>
#include <csp/cspsim/ThrustData.h>

#include <csp/csplib/util/Conversions.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <iomanip>
#include <sstream>

CSP_NAMESPACE

CSP_XML_BEGIN(Engine)
	CSP_DEF("thrust_data", m_ThrustData, true)
	CSP_DEF("thrust_direction", m_ThrustDirection, true)
	CSP_DEF("engine_offset", m_EngineOffset, true)
	CSP_DEF("smoke_emitter_location", m_SmokeEmitterLocation, true)
CSP_XML_END

CSP_XML_BEGIN(EngineDynamics)
	CSP_DEF("engine_set", m_Engine, true)
CSP_XML_END


Engine::Engine(Vector3 const &/*thrustDirection*/):
	m_LastIdleThrust(0.01),
	m_LastMilitaryThrust(0.01),
	m_LastAfterburnerThrust(0.01),
	m_Throttle(0.0),
	m_Mach(0.0),
	m_CAS(0.0),
	m_Altitude(0.0),
	m_Alpha(0.0),
	m_Density(0.01)
{
	// XMLize: A, B & C should be functions of mach
	m_A = toRadians(10.0);
	m_B = toRadians(15.0);
	m_C = toRadians(40.0);
}

Engine::~Engine() {
}

void Engine::setThrustDirection(Vector3 const& thrustDirection) {
	m_ThrustDirection = thrustDirection;
}

Vector3 const &Engine::getSmokeEmitterLocation() const {
	return m_SmokeEmitterLocation;
}

void Engine::updateThrust() {
	double blend = clampTo(getBlend(), 0.0, 3.0);
	m_LastIdleThrust = m_ThrustData->getIdle(m_Altitude, m_Mach);
	m_LastMilitaryThrust = m_ThrustData->getMil(m_Altitude, m_Mach);
	m_LastAfterburnerThrust = m_ThrustData->hasAfterburner() ? m_ThrustData->getAb(m_Altitude, m_Mach) : m_LastMilitaryThrust;
	double a = 0.0;
	double b = 0.0;
	if (blend < 1.0) {
		b = m_LastIdleThrust;
	} else if (blend < 2.0) {
		a = m_LastIdleThrust;
		b = m_LastMilitaryThrust;
		blend = blend - 1.0;
	} else {
		a = m_LastMilitaryThrust;
		b = m_LastAfterburnerThrust;
		blend = blend - 2.0;
	}
	m_LastThrust = ((1.0 - blend) * a + blend * b) * getThrustScale();
}

double Engine::getBlend() const {
	// rescale throttle [0, 0.95, 1.0] to [1, 2, 3]
	return 1.0 + m_Throttle + std::max(0.0, m_Throttle - 0.95) * 20.0;
}

double Engine::getThrustScale() const {
	const double alpha = m_Alpha;
	// f is a totally ad-hoc means of preventing a slight tailwind from killing
	// the engine when we aren't moving.
	const double f = clampTo(4.0 * (m_Mach - 0.25), 0.0, 1.0);
	if ((std::abs(alpha) > PI_2) || (alpha < -m_A)) {
		return (1.0 - f);
	}
	if (alpha < m_B) {
		return (1.0 - f) + f * cos(flatten(alpha));
 	}
	if (alpha < m_C) {
		return (1.0 - f) + f * cos(m_B + (PI_2 - m_B)*(alpha - m_B)/(m_C-m_B));
	}
	return (1.0 - f);
}

double Engine::flatten(double x) const {
	// an increasing picewise linear continuous function f on R such that:
	//        / -pi/2 if x = -a
	// f(x) = | 0     if x in [-a/2,b/2] (0 < a, b)
	//        \ b     if x = b
	double ret = std::abs(x-m_B/2) + x-m_B/2 + PI_2*(m_A/2+x - std::abs(m_A/2+x))/m_A;
	return ret;
}


void EngineDynamics::registerChannels(Bus *bus) {
	for (unsigned i = 0; i < m_Engine.size(); ++i) {
		m_Engine[i]->registerChannels(bus);
	}
}

void EngineDynamics::importChannels(Bus *bus) {
	if (!bus) return;
	b_ThrottleInput = bus->getChannel("ControlInputs.ThrottleInput");
	b_Mach = bus->getChannel(bus::Conditions::Mach);
	b_CAS = bus->getChannel(bus::Conditions::CAS);
	b_Density = bus->getChannel(bus::Conditions::Density);
	b_Alpha = bus->getChannel(bus::FlightDynamics::Alpha);
	for (unsigned i = 0; i < m_Engine.size(); ++i) {
		m_Engine[i]->importChannels(bus);
	}
}

EngineDynamics::EngineDynamics() {
}

std::vector<Vector3> EngineDynamics::getSmokeEmitterLocation() const {
	std::vector<Vector3> smoke_emitter_location;
	if (!m_Engine.empty()) {
		Link<Engine>::vector::const_iterator i = m_Engine.begin();
		Link<Engine>::vector::const_iterator iEnd = m_Engine.end();
			for (; i !=iEnd; ++i)
				smoke_emitter_location.push_back((*i)->getSmokeEmitterLocation());
	}
	return smoke_emitter_location;
}

void EngineDynamics::preSimulationStep(double dt) {
	BaseDynamics::preSimulationStep(dt);
	m_Force = m_Moment = Vector3::ZERO;
	if (!m_Engine.empty()) {
		const double alpha = b_Alpha->value();
		const double altitude = m_PositionLocal->z();
		const double mach = b_Mach->value();
		const double cas = b_CAS->value();
		const double density = b_Density->value();
		const double throttle = b_ThrottleInput->value();
		EngineSet::iterator i = m_Engine.begin();
		EngineSet::const_iterator iEnd = m_Engine.end();
		for (; i != iEnd; ++i) {
			(*i)->setConditions(cas, mach, altitude, density, alpha);
			(*i)->setThrottle(throttle);
			(*i)->update(dt);
			Vector3 force = (*i)->getThrustVector();
			m_Force += force;
			m_Moment += (*i)->m_EngineOffset ^ force;
		}
	}
}

void EngineDynamics::computeForceAndMoment(double) {
	// all the work is done by preSimulationStep
}

void EngineDynamics::getInfo(InfoList &info) const {
	std::stringstream line;
	line.setf(std::ios::fixed | std::ios::showpos);
	line << "Throttle: " << std::setprecision(2) << std::setw(6) << b_ThrottleInput->value()
	     << ", Thrust: " << std::setprecision(0) << std::setw(8) << m_Force.length();
	info.push_back(line.str());
}

CSP_NAMESPACE_END

