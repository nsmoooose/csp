// Combat Simulator Project - FlightSim Demo
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
 * @file AnimationSequence.cpp
 *
 **/


#include "AnimationSequence.h"

SIMDATA_REGISTER_INTERFACE(SharedSequence)
SIMDATA_REGISTER_INTERFACE(TimedSequence)
SIMDATA_REGISTER_INTERFACE(DrivenSequence)

void SharedSequence::registerChannels(Bus *bus) {
	// SharedSequence is used by dynamic object subclasses, so it needs to be able
	// to bind to more than one bus; hence the complicated registration dance.
	if (bus->hasChannel(m_SequenceChannel)) {
		KeyChannel channel = bus->getSharedChannel(m_SequenceChannel);
		if (!b_SequenceKey) {
			b_SequenceKey = channel;
		} else {
			assert(b_SequenceKey == channel);
		}
	} else {
		if (!b_SequenceKey) {
			b_SequenceKey = bus->registerSharedDataChannel(m_SequenceChannel, 0.0);
		} else {
			bus->registerChannel(b_SequenceKey.get());
		}
	}
	if (!m_NormalizedChannel.empty()) {
		if (!b_NormalizedKey) {
			b_NormalizedKey = bus->registerSharedPushChannel<double>(m_NormalizedChannel, m_NormalizedKey);
			m_Updater = new UpdateSequenceKey(*this);
			b_NormalizedKey->connect(m_Updater);
		} else {
			bus->registerChannel(b_NormalizedKey.get());
		}
	}
}

void SharedSequence::setNormalizedKey(double key) {
	key = simdata::clampTo(key, 0.0, 1.0);
	m_NormalizedKey = key;
	m_SequenceKey = m_InitialKey + key * (m_FinalKey - m_InitialKey);
	update();
}

void SharedSequence::updateSequenceKeyFromNormalizedChannel() {
	setNormalizedKey(b_NormalizedKey->value());
}

void SharedSequence::setSequenceKey(double key) {
	key = simdata::clampTo(key, m_InitialKey, m_FinalKey);
	m_SequenceKey = key;
	m_NormalizedKey = (key - m_InitialKey) / (m_FinalKey - m_InitialKey);
	update();
}

void SharedSequence::update(bool force) {
	const double value = b_SequenceKey->value();
	if (force || (value >= m_InitialKey && value <= m_FinalKey)) {
		if (b_NormalizedKey.valid()) b_NormalizedKey->value() = m_NormalizedKey;
		b_SequenceKey->value() = m_SequenceKey;
	}
}
