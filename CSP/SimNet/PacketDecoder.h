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

class PacketDecoder: public PacketHandler {
	RecordCodec m_Codec;
	HandlerSet<MessageHandler> m_MessageHandlers;

	struct Callback {
		Callback(NetworkMessage::Ref &msg): m_msg(msg) { }
		inline void operator()(MessageHandler::Ref handler) { handler->handleMessage(m_msg); }
		NetworkMessage::Ref m_msg;
	};

public:
	virtual bool handlePacket(PacketHeader const *header, simdata::uint8 *payload, simdata::uint32 payload_length) {
		NetworkMessage::Ref msg = m_Codec.decode(header->message_id, payload, payload_length);
		assert(msg.valid());
		Callback callback(msg);
		m_MessageHandlers.apply(callback);
	}

	void addMessageHandler(MessageHandler::Ref handler) {
		m_MessageHandlers.addHandler(handler);
	}

	bool removeMessageHandler(MessageHandler::Ref handler) {
		return m_MessageHandlers.removeHandler(handler);
	}
};

} // namespace simnet

#endif // __SIMNET_PACKETDECODER_H__
