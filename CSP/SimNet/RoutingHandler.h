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
 *       m_Dispatch->registerHandler(this, &ObjectManager::onUpdate);
 *       m_Dispatch->registerHandler(this, &ObjectManager::onDestroyed);
 *       m_Dispatch->
 *     }
 *
 *     Ref<DispatchHandle> getMessageHandler() const { return m_Dispatch; }
 *
 *     // handler methods called when a message of the argument type is received.
 *     // the return value, if set, is automatically queued for transmission.
 *     simnet::NetworkMessage::Ref onUpdate(ObjectUpdateMessage::Ref update);
 *     simnet::NetworkMessage::Ref onDestroyed(ObjectDestroyedMessage::Ref update);
 *   };
 *
 *   Server::Server() {
 *     // message queue stores outbound messages
 *     m_MessageQueue = new simnet::MessageQueue;
 *
 *     // object manager provides callbacks for ObjectUpdateMessages and
 *     // ObjectDestroyedMessages.
 *     m_ObjectManager = new ObjectManager(m_MessageQueue);
 *
 *     // the routing handler is configured to route messages with routing_type
 *     // equal to "ObjectRoutingType" (an 8-bit, application-specific constant)
 *     // to the dispthe ObjectManager for dispatch.  see ObjectManager.h for details.
 *     m_RoutingHandler = new RoutingHandler();
 *     m_RoutingHandler->registerMessageHandler(ObjectRoutingType, m_ObjectManager->getMessageHandler());
 *
 *     // packet decoder converts raw packets to messages
 *     m_PacketDecoder = new simnet::PacketDecoder();
 *     m_PacketDecoder->addMessageHandler(m_RoutingHandler);
 *
 *     // network interface is manages inbound and outbound packets
 *     m_NetworkInterface = new simnet::NetworkInterface();
 *     // other interface initialiazation here...
 *     m_NetworkInterface->setPacketSource(m_MessageQueue);
 *     m_NetworkInterface->addPacketHandler(m_PacketDecoder);
 *   }
 *   @endcode
 *
 */

#ifndef __SIMNET_ROUTINGHANDLER_H__
#define __SIMNET_ROUTINGHANDLER_H__


#include <SimNet/NetBase.h>
#include <SimNet/NetworkMessage.h>
#include <SimNet/MessageHandler.h>

#include <SimData/Ref.h>


namespace simnet {


/** A MessageHandler for distributing messages based on the routing-type
 *  field of NetworkMessage.  If the routing type field is used, it typically
 *  serves as the first stage of message dispatch.  Add a RoutingHandler to
 *  the main PacketDecoder to provide this functionality.
 */
class RoutingHandler: public MessageHandler {
	// NetworkMessage::routing_type is 8-bits
	static const unsigned RoutingTableSize = 256;
	// Use a simple table for fast lookup.
	MessageHandler::Ref m_RoutingTable[RoutingTableSize];
	MessageHandler::Ref m_DefaultHandler;
public:
	typedef simdata::Ref<RoutingHandler> Ref;

	RoutingHandler();
	virtual ~RoutingHandler();

	/** Route a message to the appropriate handler.
	 */
	virtual void handleMessage(NetworkMessage::Ref const &msg);

	/** Set the default handler to use for routing types without explicit handlers.
	 *
	 *  @param handler The default message handler to use.
	 *  @return The previous default handler.
	 */
	MessageHandler::Ref setDefaultHandler(MessageHandler::Ref const &handler);

	/** Get the handler for a given routing type.
	 *  @param routing_type The routing type.
	 *  @return The handler for the specified routing type, or null if no handler has
	 *    been explicitly set (does not return the default handler).
	 */
	MessageHandler::Ref getMessageHandler(unsigned routing_type) const;

	/** Set the message handler for a given routing type.
	 *
	 *  @param routing_type The routing type.
	 *  @param handler The message handler to use for this routing type.
	 *  @return The previous handler for this routing type.
	 */
	MessageHandler::Ref setMessageHandler(unsigned routing_type, MessageHandler::Ref const &handler);

	/** Remove all the handlers (other than the default).
	 */
	void removeAll();
};


} // namespace simnet

#endif // __SIMNET_ROUTINGHANDLER_H__

