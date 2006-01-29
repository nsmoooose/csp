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
 * @file SoundEffect.cpp
 *
 **/


#include <csp/cspsim/sound/SoundEffect.h>
#include <csp/cspsim/sound/SoundModel.h>
#include <csp/cspsim/sound/Sample.h>
#include <csp/csplib/util/Log.h>

#include <osgAL/SoundState>

#include <cassert>

CSP_NAMESPACE

SoundEffect::SoundEffect(Ref<const SoundSample> const &sample, Ref<SoundModel> const &model, Mode mode) {
	assert(sample.valid() && model.valid());
	m_SoundState = sample->getSoundState();
	assert(m_SoundState.valid());
	m_SoundState->allocateSource(sample->priority(), false);
	m_SoundState->apply();
	switch (mode) {
		case EXTERNAL: model->addExternalSound(m_SoundState.get()); break;
		case INTERNAL: model->addInternalSound(m_SoundState.get()); break;
		case HEADSET: model->addHeadsetSound(m_SoundState.get()); break;
		default:
			CSPLOG(ERROR, AUDIO) << "Unknown sound effect mode " << mode;
			m_SoundState = 0;
	}
}

SoundEffect::~SoundEffect() {}

void SoundEffect::play(bool on) {
	m_SoundState->setPlay(on);
}

osgAL::SoundState *SoundEffect::state() {
	return m_SoundState.get();
}

SoundEffect* SoundEffect::HeadsetSound(Ref<const SoundSample> const &sample, Ref<SoundModel> const &model) {
	return (!sample || !model) ? 0 : new SoundEffect(sample, model, HEADSET);
}

SoundEffect* SoundEffect::InternalSound(Ref<const SoundSample> const &sample, Ref<SoundModel> const &model) {
	return (!sample || !model) ? 0 : new SoundEffect(sample, model, INTERNAL);
}

SoundEffect* SoundEffect::ExternalSound(Ref<const SoundSample> const &sample, Ref<SoundModel> const &model) {
	return (!sample || !model) ? 0 : new SoundEffect(sample, model, EXTERNAL);
}

CSP_NAMESPACE_END

