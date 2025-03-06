// Combat Simulator Project
// Copyright 2003, 2004 The Combat Simulator Project
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
 * @file Bus.cpp
 * @brief Classes for sharing data between vehicle subsystems.
 **/

#include <csp/cspsim/Bus.h>
#include <csp/csplib/util/StringTools.h>

namespace csp {

Bus::Bus(std::string const &name): m_Bound(false), m_Enabled(true), m_Name(name), m_Status(1.0) {
	CSPLOG(Prio_DEBUG, Cat_OBJECT) << "Bus(" << name << ") created.";
}

ChannelBase* Bus::registerChannel(ChannelBase *channel, std::string const &groups) {
	assert(channel);
	std::string name = channel->getName();
	CSPLOG(Prio_DEBUG, Cat_OBJECT) << "Bus::registerChannel(" << name << ")";
	assert(m_Channels.find(name) == m_Channels.end());
	m_Channels[name] = channel;
	CSPLOG(Prio_DEBUG, Cat_OBJECT) << "Bus::registerChannel: groups = " << groups;
	TokenQueue grouplist(groups, " ");
	for (TokenQueue::iterator group = grouplist.begin(); group != grouplist.end(); ++group) {
		CSPLOG(Prio_DEBUG, Cat_OBJECT) << "Bus::registerChannel: groupX = " << *group;
		m_Groups[*group].push_back(channel);
	}
	return channel;
}

ChannelBase::RefT Bus::getSharedChannel(std::string const &name, bool required, bool override) {
	ChannelMap::iterator iter = m_Channels.find(name);
	if (iter == m_Channels.end()) {
		CSPLOG(Prio_DEBUG, Cat_OBJECT) << "Bus::getSharedChannel(" << name << ") failed.";
		assert(!required);
		return 0;
	}
	assert(iter->second->isShared() || override);
	return iter->second;
}

ChannelBase::CRefT Bus::getChannel(std::string const &name, bool required) {
	ChannelMap::iterator iter = m_Channels.find(name);
	if (iter == m_Channels.end()) {
		if (required) {
			CSPLOG(Prio_ERROR, Cat_OBJECT) << "Bus::getChannel(" << name << ") failed.";
			assert(0);
		} else {
			CSPLOG(Prio_DEBUG, Cat_OBJECT) << "Bus::getChannel(" << name << ") failed.";
		}
		return 0;
	}
	return iter->second;
}

void Bus::setEnabled(bool enabled) {
	if (enabled == m_Enabled) return;
	for (ChannelMap::iterator iter = m_Channels.begin();
		 iter != m_Channels.end(); ++iter) {
		iter->second->setEnabled(enabled);
	}
	m_Enabled = enabled;
}

} // namespace csp

