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

#include <csp/cspsim/AircraftEngine.h>
#include <csp/cspsim/ConditionsChannels.h>
#include <csp/cspsim/FlightDynamicsChannels.h>
#include <csp/cspsim/ResourceBundle.h>
#include <csp/cspsim/sound/SoundEngine.h>
#include <csp/cspsim/sound/Sample.h>
#include <csp/cspsim/sound/SoundModel.h>
#include <csp/cspsim/SystemsModel.h>
#include <csp/cspsim/ThrustData.h>

#include <csp/csplib/util/Conversions.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/util/osg.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <osgAL/SoundState>
#include <csp/csplib/util/undef.h>

#include <iomanip>
#include <sstream>

namespace csp {

CSP_XML_BEGIN(AircraftEngine)
	CSP_DEF("thrust_data", m_ThrustData, true)
	CSP_DEF("thrust_direction", m_ThrustDirection, true)
	CSP_DEF("engine_offset", m_EngineOffset, true)
	CSP_DEF("smoke_emitter_location", m_SmokeEmitterLocation, true)
CSP_XML_END

CSP_XML_BEGIN(AircraftEngineDynamics)
	CSP_DEF("engine_set", m_Engine, true)
CSP_XML_END


AircraftEngine::AircraftEngine(Vector3 const &/*thrustDirection*/):
	m_LastIdleThrust(0.01),
	m_LastMilitaryThrust(0.01),
	m_LastAfterburnerThrust(0.01),
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

AircraftEngine::~AircraftEngine() {
}

void AircraftEngine::setThrustDirection(Vector3 const& thrustDirection) {
	m_ThrustDirection = thrustDirection;
}

Vector3 const &AircraftEngine::getSmokeEmitterLocation() const {
	return m_SmokeEmitterLocation;
}

void AircraftEngine::bindSounds(SoundModel* model, ResourceBundle* bundle) {
	if ( !SoundEngine::getInstance().getSoundEnabled() ) return;
	assert(model);
	CSPLOG(DEBUG, AUDIO) << "AircraftEngine::bindSounds";
	if (bundle) {
		// set main engine sound
		CSPLOG(DEBUG, AUDIO) << "AircraftEngine::bindSounds have bundle";
		Ref<const SoundSample> engine_sample(bundle->getSoundSample("engine"));
		m_EngineSound = SoundEffect::ExternalSound(engine_sample, model);
		if (m_EngineSound.valid()) {
			CSPLOG(DEBUG, AUDIO) << "AircraftEngine::bindSounds engine sound available";
			m_EngineSound->state()->setPosition(toOSG(m_EngineOffset));
			m_EngineSound->state()->setDirection(toOSG(m_ThrustDirection));
			CSPLOG(DEBUG, AUDIO) << "engine sound position " << m_EngineOffset;
			CSPLOG(DEBUG, AUDIO) << "engine sound direction " << m_ThrustDirection;
			m_EngineSound->state()->apply();
			m_EngineSound->play();	// ToDo: check if engine's really running
		}

		// set afterburner sound
		Ref<const SoundSample> afterburner_sample(bundle->getSoundSample("afterburner"));
		m_AfterburnerSound = SoundEffect::ExternalSound(afterburner_sample, model);
		if (m_AfterburnerSound.valid()) {
			CSPLOG(DEBUG, AUDIO) << "AircraftEngine::bindSounds afterburner sound available";
			m_AfterburnerSound->state()->setPosition(toOSG(m_EngineOffset));
			m_AfterburnerSound->state()->setDirection(toOSG(m_ThrustDirection));
			CSPLOG(DEBUG, AUDIO) << "afterburner sound position " << m_EngineOffset;
			CSPLOG(DEBUG, AUDIO) << "afterburner sound direction " << m_ThrustDirection;
			m_AfterburnerSound->state()->apply();
		}
	}
	CSPLOG(DEBUG, AUDIO) << "AircraftEngine::bindSounds exit";
}

void AircraftEngine::updateThrust() {
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

double AircraftEngine::getBlend() const {
	// rescale throttle [0, 0.95, 1.0] to [1, 2, 3]
	return 1.0 + m_Throttle + std::max(0.0, m_Throttle - 0.95) * 20.0;
}

double AircraftEngine::getThrustScale() const {
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

double AircraftEngine::flatten(double x) const {
	// an increasing picewise linear continuous function f on R such that:
	//        / -pi/2 if x = -a
	// f(x) = | 0     if x in [-a/2,b/2] (0 < a, b)
	//        \ b     if x = b
	double ret = std::abs(x-m_B/2) + x-m_B/2 + PI_2*(m_A/2+x - std::abs(m_A/2+x))/m_A;
	return ret;
}


void AircraftEngineDynamics::registerChannels(Bus *bus) {
	SoundModel *sound_model = getModel()->getSoundModel();
	for (unsigned i = 0; i < m_Engine.size(); ++i) {
		m_Engine[i]->registerChannels(bus);
		if (sound_model) {
			m_Engine[i]->bindSounds(sound_model, getResourceBundle());
		}
	}
}

void AircraftEngineDynamics::importChannels(Bus *bus) {
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

AircraftEngineDynamics::AircraftEngineDynamics() {
}

std::vector<Vector3> AircraftEngineDynamics::getSmokeEmitterLocation() const {
	std::vector<Vector3> smoke_emitter_location;
	if (!m_Engine.empty()) {
		Link<AircraftEngine>::vector::const_iterator i = m_Engine.begin();
		Link<AircraftEngine>::vector::const_iterator iEnd = m_Engine.end();
			for (; i !=iEnd; ++i)
				smoke_emitter_location.push_back((*i)->getSmokeEmitterLocation());
	}
	return smoke_emitter_location;
}

void AircraftEngineDynamics::preSimulationStep(double dt) {
	BaseDynamics::preSimulationStep(dt);
	m_Force = m_Moment = Vector3::ZERO;
	float fPitch, fBlend;
	bool isPlaying;
	if (!m_Engine.empty()) {
		const double alpha = b_Alpha->value();
		const double altitude = m_Position->z();
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
			fBlend = (*i)->getBlend();
			fPitch = fBlend;
			// Using fBlend as a modulator for pitch and gain is a simple ad-hoc
			// solution, but yields to nice results. fPitch must be clamped to 1.999,
			// because OpenAL crashes when using a value >= 2.0
			if((*i)->m_EngineSound.valid()) {
				if (fBlend >= 2.0) {
					fPitch = 1.999;
				}
				(*i)->m_EngineSound->state()->setPitch(fPitch);
				(*i)->m_EngineSound->state()->setGain(fBlend / 3);
//				(*i)->m_EngineSound->state()->apply();
			}

			// the afterburner sound is played when fBlend is between 2 and 3
			// ToDo: check whether this is the right parameter to determine if burner is running
			if((*i)->m_AfterburnerSound.valid()) {
				(*i)->m_AfterburnerSound->state()->setGain(fBlend / 3);
				isPlaying = (*i)->m_AfterburnerSound->state()->getPlay();
				if (fBlend > 2.0 && !isPlaying) {
					(*i)->m_AfterburnerSound->play();
				}
				if (fBlend <= 2.0 && isPlaying) {
					(*i)->m_AfterburnerSound->play(false);
				}
			}
		}
	}
}

void AircraftEngineDynamics::computeForceAndMoment(double) {
	// all the work is done by preSimulationStep
}

void AircraftEngineDynamics::getInfo(InfoList &info) const {
	std::stringstream line;
	line.setf(std::ios::fixed | std::ios::showpos);
	line << "Throttle: " << std::setprecision(2) << std::setw(6) << b_ThrottleInput->value()
	     << ", Thrust: " << std::setprecision(0) << std::setw(8) << m_Force.length();
	info.push_back(line.str());
}

} // namespace csp

