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
 * @file VoiceMessageUnit.cpp
 *
 **/


#include <csp/cspsim/f16/VoiceMessageUnit.h>
#include <csp/cspsim/f16/F16Channels.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <osgAL/SoundState>

CSP_NAMESPACE

// TODO notes for generalizing the VMU interface:
//   - Sounds triggered by channel events:
//     + boolean push channels only for now
//   - Playback modes:
//     + loop while active, complete current loop when deactivated.
//     + loop while active, stop when deactivated.
//     + loop while active, pause when deactivated.
//     + play once when activated, optionally abort when deactivated
//     + play once when deactivated, optionally abort when activated
//     + others?
//
//  Could be extended arbitrarily using channel adaptors from arbitrary
//  input channels to a single output boolean push channel.
//
//  Messages would be a list of {channel, sample, playback_mode}.

CSP_XML_BEGIN(VoiceMessageUnit)
CSP_XML_END


VoiceMessageUnit::VoiceMessageUnit() {
}

void VoiceMessageUnit::registerChannels(Bus*) {
}

void VoiceMessageUnit::importChannels(Bus* bus) {
	// TODO generalize to a list of messages, channels, and playback modes
	b_AltitudeAdvisory = bindChannel(bus, "F16.GroundAvoidance.AltitudeAdvisory", &VoiceMessageUnit::onAltitudeAdvisory);
	b_DescentWarningAfterTakeoff = bindChannel(bus, "F16.GroundAvoidance.DescentWarningAfterTakeoff", &VoiceMessageUnit::onDescentWarningAfterTakeoff);
	m_AltitudeSoundEffect = addSoundEffect("altitude_warning_sample", SoundEffect::HEADSET);
}


DataChannel<bool>::CRefT VoiceMessageUnit::bindChannel(Bus *bus, std::string const &channel_name, void (VoiceMessageUnit::*handler)()) {
	assert(bus);
	DataChannel<bool>::CRefT channel = bus->getChannel(channel_name, false);
	if (channel.valid()) channel->connect(this, handler);
	return channel;
}

void VoiceMessageUnit::onAltitudeAdvisory() {
	assert(b_AltitudeAdvisory.valid());
	playMessage(m_AltitudeSoundEffect, b_AltitudeAdvisory->value());
}


void VoiceMessageUnit::onDescentWarningAfterTakeoff() {
	assert(b_DescentWarningAfterTakeoff.valid());
	playMessage(m_AltitudeSoundEffect, b_DescentWarningAfterTakeoff->value());
}

void VoiceMessageUnit::playMessage(Ref<SoundEffect> const &effect, bool active) {
	if (!effect) return;
	if (active) {
		if (!effect->state()->isActive()) effect->state()->setPlay(false);
		effect->state()->setLooping(true);
		if (!effect->state()->getPlay()) effect->play(true);
	} else {
		// set the advisory warning to turn off after it completes.
		if (effect->state()->getLooping()) effect->state()->setLooping(false);
		if (!effect->state()->isActive()) effect->state()->setPlay(false);
	}
}

CSP_NAMESPACE_END

