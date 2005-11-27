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
 * @file RoutingHandler.cpp
 *
 */


#include <csp/csplib/net/RoutingHandler.h>
#include <csp/csplib/net/MessageHandler.h>
#include <csp/csplib/net/NetLog.h>
#include <csp/csplib/util/Verify.h>


CSP_NAMESPACE

CSP_STATIC_CONST_DEF(unsigned RoutingHandler::RoutingTableSize);

RoutingHandler::RoutingHandler() {
}

RoutingHandler::~RoutingHandler() {
}

Ref<MessageHandler> RoutingHandler::setDefaultHandler(Ref<MessageHandler> const &handler) {
	Ref<MessageHandler> old_handler = m_DefaultHandler;
	m_DefaultHandler = handler;
	return old_handler;
}

void RoutingHandler::handleMessage(Ref<NetworkMessage> const &msg) {
	if (msg.isNull()) return;
	const unsigned routing_type = msg->getRoutingType();
	if (routing_type < RoutingTableSize) {
		Ref<MessageHandler> handler = m_RoutingTable[routing_type];
		if (handler.valid()) {
			handler->handleMessage(msg);
		} else if (m_DefaultHandler.valid()) {
			m_DefaultHandler->handleMessage(msg);
		}
	}
}

Ref<MessageHandler> RoutingHandler::getMessageHandler(unsigned routing_type) const {
	CSP_ASSERT_LT(routing_type, RoutingTableSize);
	if (routing_type >= RoutingTableSize) return 0;
	return m_RoutingTable[routing_type];
}

Ref<MessageHandler> RoutingHandler::setMessageHandler(unsigned routing_type, Ref<MessageHandler> const &handler) {
	CSP_ASSERT_LT(routing_type, RoutingTableSize);
	if (routing_type >= RoutingTableSize) return 0;
	Ref<MessageHandler> old_handler = m_RoutingTable[routing_type];
	m_RoutingTable[routing_type] = handler;
	return old_handler;
}

void RoutingHandler::removeAll() {
	for (unsigned i = 0; i < RoutingTableSize; ++i) m_RoutingTable[i] = 0;
}

CSP_NAMESPACE_END

