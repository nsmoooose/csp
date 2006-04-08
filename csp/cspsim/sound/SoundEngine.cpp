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
#include <csp/csplib/util/undef.h>

#include <cassert>

CSP_NAMESPACE


SoundEngine &SoundEngine::getInstance() {
	static SoundEngine *engine = 0;
	if (!engine) {
		CSPLOG(DEBUG, APP) << "Creating SoundEngine";
		engine = new SoundEngine;
	}
	return *engine;
}


SoundEngine::SoundEngine() {
}

SoundEngine::~SoundEngine() {
}

osgAL::SoundManager *SoundEngine::getManager() {
	return osgAL::SoundManager::instance();
}

void SoundEngine::initialize() {
	CSPLOG(DEBUG, APP) << "SoundEngine::initialize";
	osgAL::SoundManager *manager = getManager();
	CSPLOG(DEBUG, APP) << "SoundEngine::initialize manager = " << manager;
	assert(manager);
	manager->init(32);
	CSPLOG(DEBUG, APP) << "SoundEngine::initialize manager init";
	CSPLOG(DEBUG, APP) << "SoundEngine::initialize env = " << manager->getEnvironment();
	manager->getEnvironment()->setDistanceModel(openalpp::InverseDistanceClamped);
	manager->getEnvironment()->setDopplerFactor(0.3);  // full doppler produces artifacts
	manager->setMaxVelocity(200);
	//manager->setClampVelocity(true);  (not available in 20041121-3 debian package)
	CSPLOG(DEBUG, APP) << "SoundEngine::initialize done";
}

void SoundEngine::shutdown() {
	getManager()->shutdown();
}

osgAL::SoundRoot *SoundEngine::getSoundRoot() {
	if (!m_SoundRoot) createSoundRoot();
	return m_SoundRoot.get();
}

void SoundEngine::createSoundRoot() {
	assert(!m_SoundRoot);
	m_SoundRoot = new osgAL::SoundRoot();
}

void SoundEngine::mute() {
	getManager()->getEnvironment()->setGain(0.0);
}

void SoundEngine::unmute() {
	getManager()->getEnvironment()->setGain(1.0);
}

CSP_NAMESPACE_END

