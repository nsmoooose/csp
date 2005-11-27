// Combat Simulator Project
// Copyright (C) 2004 The Combat Simulator Project
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
 * @file DispatchHandler.cpp
 *
 */


#include <csp/csplib/net/DispatchHandler.h>
#include <csp/csplib/net/NetLog.h>

CSP_NAMESPACE


DispatchHandler::DispatchHandler(Ref<MessageQueue> const &queue): m_Queue(queue) {
}

DispatchHandler::~DispatchHandler() {
}

void DispatchHandler::handleMessage(Ref<NetworkMessage> const &msg) {
	SIMNET_LOG(INFO, MESSAGE) << "handling message of type " << msg->getName();
	if (!dispatch(msg) && m_DefaultHandler.valid()) {
		SIMNET_LOG(INFO, MESSAGE) << "unknown message type, calling default handler";
		m_DefaultHandler->call(msg, m_Queue);
	}
}

bool DispatchHandler::dispatch(Ref<NetworkMessage> const &msg) const {
	Ref<BaseCallback> callback = getCallback(msg->getId());
	if (!callback) return false;
	callback->call(msg, m_Queue);
	return true;
}

void DispatchHandler::_registerHandler(std::string const &name, const MessageId id, Ref<BaseCallback> const &handler) {
	SIMNET_LOG(INFO, MESSAGE) << "registering handler for " << name;
	assert(!getCallback(id));
	m_DispatchMap[id] = handler;
}

void DispatchHandler::disconnect(NetworkMessage::Id id) {
	DispatchMap::iterator iter = m_DispatchMap.find(id);
	if (iter != m_DispatchMap.end()) m_DispatchMap.erase(iter);
}

inline Ref<BaseCallback> DispatchHandler::getCallback(const TaggedRecord::Id id) const {
	DispatchMap::const_iterator iter = m_DispatchMap.find(id);
	return (iter == m_DispatchMap.end() ? 0 : iter->second);
}

CSP_NAMESPACE_END

