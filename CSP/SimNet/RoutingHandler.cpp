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


#include <SimNet/RoutingHandler.h>
#include <SimNet/MessageHandler.h>
#include <SimNet/NetLog.h>
#include <SimData/Verify.h>


namespace simnet {


const unsigned RoutingHandler::RoutingTableSize;

RoutingHandler::RoutingHandler() {
}

RoutingHandler::~RoutingHandler() {
}

MessageHandler::Ref RoutingHandler::setDefaultHandler(MessageHandler::Ref const &handler) {
	MessageHandler::Ref old_handler = m_DefaultHandler;
	m_DefaultHandler = handler;
	return old_handler;
}

void RoutingHandler::handleMessage(NetworkMessage::Ref const &msg) {
	if (msg.isNull()) return;
	const unsigned routing_type = msg->getRoutingType();
	if (routing_type < RoutingTableSize) {
		MessageHandler::Ref handler = m_RoutingTable[routing_type];
		if (handler.valid()) {
			handler->handleMessage(msg);
		} else if (m_DefaultHandler.valid()) {
			m_DefaultHandler->handleMessage(msg);
		}
	}
}

MessageHandler::Ref RoutingHandler::getMessageHandler(unsigned routing_type) const {
	SIMDATA_ASSERT_LT(routing_type, RoutingTableSize);
	if (routing_type >= RoutingTableSize) return 0;
	return m_RoutingTable[routing_type];
}

MessageHandler::Ref RoutingHandler::setMessageHandler(unsigned routing_type, MessageHandler::Ref const &handler) {
	SIMDATA_ASSERT_LT(routing_type, RoutingTableSize);
	if (routing_type >= RoutingTableSize) return 0;
	MessageHandler::Ref old_handler = m_RoutingTable[routing_type];
	m_RoutingTable[routing_type] = handler;
	return old_handler;
}

void RoutingHandler::removeAll() {
	for (unsigned i = 0; i < RoutingTableSize; ++i) m_RoutingTable[i] = 0;
}

} // namespace simnet

