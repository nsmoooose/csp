// Combat Simulator Project - FlightSim Demo
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
 * @file MessageHandler.h
 *
 */

#ifndef __SIMNET_MESSAGEHANDLER_H__
#define __SIMNET_MESSAGEHANDLER_H__

#include <SimNet/NetworkMessage.h>
#include <SimData/Ref.h>

namespace simnet {


/** Abstract interface for handlers of received network messages.
 *
 *  Subclass MessageHandler and add your handler to a PacketDecoder
 *  to process incoming messages.
 */
class MessageHandler: public simdata::Referenced {
public:
	typedef simdata::Ref<MessageHandler> Ref;

	/** Called by PacketDecoder to handle an incoming message.
	 */
	virtual void handleMessage(NetworkMessage::Ref message)=0;

	virtual ~MessageHandler() { }
};


// TODO under construction
#if 0
/**
 * macros for building MessageHandler subclasses that dispatch messages
 * to handlers.
 */
#define SIMNET_DISPATCH_BEGIN(TARGET) \
class TARGET##Dispatch: public simnet::MessageHandler { \
	TARGET *m_target; \
public: \
	TARGET##Dispatch(TARGET *target): simnet::MessageHandler(), m_target(target) { } \
	virtual void handleMessage(simnet::NetworkMessage::Ref msg) { \
		switch (msg->getId()) {

#define SIMNET_DISPATCH(MESSAGE, HANDLER) \
		case MESSAGE::_Id(): \
			m_target->HANDLER(simnet::NetworkMessage::FastCast<MESSAGE>(msg)); \
			break;

#define SIMNET_DISPATCH_END \
		default: target->defaultMessageHandler(msg); \
		} \
	} \
};

#define SIMNET_DISPATCH_CLASS(TARGET) TARGET##Dispatch
#endif


} // namespace simnet

#endif // __SIMNET_MESSAGEHANDLER_H__


