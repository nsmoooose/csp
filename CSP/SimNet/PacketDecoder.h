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
 * @file PacketDecoder.h
 *
 */

#ifndef __SIMNET_PACKETDECODER_H__
#define __SIMNET_PACKETDECODER_H__

#include <SimNet/PacketHandler.h>
#include <SimNet/MessageHandler.h>
#include <SimNet/NetworkMessage.h>
#include <SimNet/RecordCodec.h>
#include <SimNet/HandlerSet.h>

namespace simnet {

/** Packet handler for decoding raw packets to NetworkMessage subclasses.
 */
class PacketDecoder: public PacketHandler {
	RecordCodec m_Codec;
	HandlerSet<MessageHandler> m_MessageHandlers;

	/** Helper functor for passing a network message to a set of MessageHandlers
	 */
	class Callback {
		NetworkMessage::Ref m_msg;
	public:
		Callback(NetworkMessage::Ref const &msg): m_msg(msg) { }
		inline void operator()(MessageHandler::Ref handler) { handler->handleMessage(m_msg); }
	};

public:

	typedef simdata::Ref<PacketDecoder> Ref;

	/** Called by the network interface to handle an incoming packet.
	 *
	 *  The packet payload is decoded using RecordCodec into the appropriate
	 *  NetworkInterface subclass instance (using the message id stored in
	 *  the packet header), and then passed to all registered message handlers.
	 *
	 *  @param header a pointer to the packet header.
	 *  @param payload a buffer containing the payload data.
	 *  @param payload_length the size of the payload, in bytes.
	 */
	virtual void handlePacket(PacketHeader const *header, simdata::uint8 *payload, simdata::uint32 payload_length) {
		NetworkMessage::Ref msg = m_Codec.decode(header->messageId(), payload, payload_length);
		if (!msg) return;
		msg->setSource(header->source());
		msg->setRoutingType(static_cast<simnet::RoutingType>(header->routingType()));
		msg->setRoutingData(header->routingData());
		Callback callback(msg);
		m_MessageHandlers.apply(callback);
	}

	/** Add a message handler.  Each decoded message will be passed to all
	 *  message handlers for subsequent processing.
	 */
	void addMessageHandler(MessageHandler::Ref handler) {
		m_MessageHandlers.addHandler(handler);
	}

	/** Remove a message handler.
	 */
	bool removeMessageHandler(MessageHandler::Ref handler) {
		return m_MessageHandlers.removeHandler(handler);
	}

	/** Pass a message directly to the handlers (FOR DEBUGGING ONLY!)
	 */
	void injectMessage(NetworkMessage::Ref const &msg) {
		Callback callback(msg);
		m_MessageHandlers.apply(callback);
	}
};

} // namespace simnet

#endif // __SIMNET_PACKETDECODER_H__
