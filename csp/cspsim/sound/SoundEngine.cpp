// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file SoundEngine.h
 *
 **/


#include <csp/cspsim/sound/SoundEngine.h>
#include <csp/csplib/util/Log.h>

#include <osgAL/SoundManager>
#include <osgAL/SoundRoot>

#include <cassert>

namespace csp {

// Local log macro

#define LOG(P) CSPLOG(P, Cat_AUDIO)

#define ERR LOG(Prio_ERROR)
#define DEB LOG(Prio_DEBUG)
#define INF LOG(Prio_INFO) 

SoundEngine &SoundEngine::getInstance() {
	static SoundEngine *engine = 0;
	if (!engine) {
		CSPLOG(Prio_DEBUG, Cat_APP) << "Creating SoundEngine";
		engine = new SoundEngine;
	}
	return *engine;
}


SoundEngine::SoundEngine() : m_SoundEnabled(true) {
}

SoundEngine::~SoundEngine() {
}

osgAL::SoundManager *SoundEngine::getManager() {
	return osgAL::SoundManager::instance();
}


void SoundEngine::initialize ()
{
	DEB << "Initializing the sound engine";
	try {
		assert (getManager ());
		osgAL::SoundManager& manager = *getManager ();

		DEB << "Initializing osgAL::SoundManager";
		manager.init (32); // num_soundsources
		assert (manager.initialized ());

		DEB << "Setting sound environment parameters";
		assert (manager.getEnvironment ());
		openalpp::AudioEnvironment& env = *manager.getEnvironment ();
		env.setDistanceModel (openalpp::InverseDistanceClamped);
		env.setDopplerFactor (0.3); // full doppler produces artifacts
		manager.setMaxVelocity (200);
		manager.setClampVelocity (true);
	}
	catch (const std::exception& x) {
		ERR << "Sound engine initialization failed - are the OpenAL drivers installed? (" << x.what() << ")";
		m_SoundEnabled = false;
		//throw; // Don't know how to continue without sound.
	}
	catch (...) {
		ERR << "Sound engine initialization failed (unknown exception) - are the OpenAL drivers installed?";
		m_SoundEnabled = false;
		//throw; // Don't know how to continue without sound.
	}
	INF << "Sound engine initialization succeeded";
}

void SoundEngine::shutdown() {
	if(m_SoundEnabled) {
		getManager()->shutdown();
	}
}

osgAL::SoundRoot *SoundEngine::getSoundRoot() {
	if(m_SoundEnabled) {
		if (!m_SoundRoot) createSoundRoot();
		return m_SoundRoot.get();
	}
	return NULL;
}

bool SoundEngine::getSoundEnabled() {
	return m_SoundEnabled;
}

void SoundEngine::createSoundRoot() {
	if(m_SoundEnabled) {
		assert(!m_SoundRoot);
		m_SoundRoot = new osgAL::SoundRoot();
	}
}

void SoundEngine::mute() {
	if(m_SoundEnabled) {
		getManager()->getEnvironment()->setGain(0.0);
	}
}

void SoundEngine::unmute() {
	if(m_SoundEnabled) {
		getManager()->getEnvironment()->setGain(1.0);
	}
}

} // namespace csp

