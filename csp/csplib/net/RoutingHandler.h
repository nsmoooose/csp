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
 * @file RoutingHandler.h
 *
 * A message handler for routing messages to dispatch handlers based
 * on routing_type.
 *
 * Example usage:
 *
 *   @code
 *   class ObjectManager {
 *     Ref<MessageHandler> m_Dispatch;
 *     ObjectManager();
 *
 *     void bindNetwork(Ref<MessageQueue> queue) {
 *       m_Dispatch = new MessageHandler(queue);
 *       m_Dispatch->setMessageHandler(this, &ObjectManager::onUpdate);
 *       m_Dispatch->setMessageHandler(this, &ObjectManager::onDestroyed);
 *     }
 *
 *     Ref<DispatchHandle> getMessageHandler() const { return m_Dispatch; }
 *
 *     // handler methods called when a message of the argument type is received.
 *     // the return value, if set, is automatically queued for transmission.
 *     Ref<NetworkMessage> onUpdate(Ref<ObjectUpdateMessage> update);
 *     Ref<NetworkMessage> onDestroyed(Ref<ObjectDestroyedMessage> update);
 *   };
 *
 *   Server::Server() {
 *     // message queue stores outbound messages
 *     m_MessageQueue = new MessageQueue;
 *
 *     // object manager provides callbacks for ObjectUpdateMessages and
 *     // ObjectDestroyedMessages.
 *     m_ObjectManager = new ObjectManager(m_MessageQueue);
 *
 *     // the routing handler is configured to route messages with routing_type
 *     // equal to "ObjectRoutingType" (an 8-bit, application-specific constant)
 *     // to the dispthe ObjectManager for dispatch.  see ObjectManager.h for details.
 *     m_RoutingHandler = new RoutingHandler();
 *     m_RoutingHandler->setMessageHandler(ObjectRoutingType, m_ObjectManager->getMessageHandler());
 *
 *     // packet decoder converts raw packets to messages
 *     m_PacketDecoder = new PacketDecoder();
 *     m_PacketDecoder->addMessageHandler(m_RoutingHandler);
 *
 *     // network interface is manages inbound and outbound packets
 *     m_NetworkInterface = new NetworkInterface();
 *     // other interface initialiazation here...
 *     m_NetworkInterface->setPacketSource(m_MessageQueue);
 *     m_NetworkInterface->addPacketHandler(m_PacketDecoder);
 *   }
 *   @endcode
 *
 */

#ifndef __CSPLIB_NET_ROUTINGHANDLER_H__
#define __CSPLIB_NET_ROUTINGHANDLER_H__


#include <csp/csplib/net/NetBase.h>
#include <csp/csplib/net/NetworkMessage.h>
#include <csp/csplib/net/MessageHandler.h>

#include <csp/csplib/util/Ref.h>


CSP_NAMESPACE


/** A MessageHandler for distributing messages based on the routing-type
 *  field of NetworkMessage.  If the routing type field is used, it typically
 *  serves as the first stage of message dispatch.  Add a RoutingHandler to
 *  the main PacketDecoder to provide this functionality.
 *
 *  @ingroup net
 */
class CSPLIB_EXPORT RoutingHandler: public MessageHandler {
	// NetworkMessage::routing_type is 8-bits
	static const unsigned RoutingTableSize = 256;
	// Use a simple table for fast lookup.
	Ref<MessageHandler> m_RoutingTable[RoutingTableSize];
	Ref<MessageHandler> m_DefaultHandler;

public:
	RoutingHandler();
	virtual ~RoutingHandler();

	/** Route a message to the appropriate handler.
	 */
	virtual void handleMessage(Ref<NetworkMessage> const &msg);

	/** Set the default handler to use for routing types without explicit handlers.
	 *
	 *  @param handler The default message handler to use.
	 *  @return The previous default handler.
	 */
	Ref<MessageHandler> setDefaultHandler(Ref<MessageHandler> const &handler);

	/** Get the handler for a given routing type.
	 *  @param routing_type The routing type.
	 *  @return The handler for the specified routing type, or null if no handler has
	 *    been explicitly set (does not return the default handler).
	 */
	Ref<MessageHandler> getMessageHandler(unsigned routing_type) const;

	/** Set the message handler for a given routing type.
	 *
	 *  @param routing_type The routing type.
	 *  @param handler The message handler to use for this routing type.
	 *  @return The previous handler for this routing type.
	 */
	Ref<MessageHandler> setMessageHandler(unsigned routing_type, Ref<MessageHandler> const &handler);

	/** Remove all the handlers (other than the default).
	 */
	void removeAll();
};


CSP_NAMESPACE_END

#endif // __CSPLIB_NET_ROUTINGHANDLER_H__

