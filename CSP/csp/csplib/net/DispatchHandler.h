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
 * @file DispatchHandler.h
 *
 * A simple handler for dispatching messages to handler methods
 * based on message id.
 *
 * Sample usage:
 *
 *   @code
 *   class ObjectManager {
 *   public:
 *     void bindHandlers(Ref<DispatchHandler> dispatch) {
 *       dispatch->registerHandler(this, &ObjectManager::onUpdate);
 *       dispatch->registerHandler(this, &ObjectManager::onDestroyed);
 *       dispatch->registerDefaultHandler(this, &ObjectManager::onUnknownMessage);
 *     }
 *
 *     // handler methods called when a message of the argument type is received.
 *     // the return value, if set, is automatically queued for transmission.
 *     Ref<NetworkMessage> onUpdate(Ref<ObjectUpdateMessage> update);
 *     Ref<NetworkMessage> onDestroyed(Ref<ObjectDestroyedMessage> update);
 *     Ref<NetworkMessage> onUnknownMessage(Ref<NetworkMessage> update);
 *   };
 *   @endcode
 *
 *   The following example shows how to connect this set of handlers to a
 *   network interface.   See RoutingHandler.h for a more advanced example.
 *
 *   @code
 *   Server::Server() {
 *     // message queue stores outbound messages
 *     m_MessageQueue = new MessageQueue;
 *
 *     // dispatch handler passes incoming messages to registered handlers
 *     m_DispatchHandler = new DispatchHandler(m_MessageQueue);
 *
 *     // object manager provides callbacks for ObjectUpdateMessages and
 *     // ObjectDestroyedMessages.
 *     m_ObjectManager = new ObjectManager();
 *     m_ObjectManager->bindHandlers(m_DispatchHandler);
 *
 *     // packet decoder converts raw packets to messages
 *     m_PacketDecoder = new PacketDecoder();
 *     m_PacketDecoder->addMessageHandler(m_DispatchHandler);
 *
 *     // network interface is manages inbound and outbound packets
 *     m_NetworkInterface = new NetworkInterface();
 *     m_NetworkInterface->setPacketSource(m_MessageQueue);
 *     m_NetworkInterface->addPacketHandler(m_PacketDecoder);
 *     // other interface initialiazation here...
 *   }
 *   @endcode
 *
 *   TODO(os) simplify the example above using PacketServer
 *
 */

#ifndef __CSPLIB_NET_DISPATCHHANDLER_H__
#define __CSPLIB_NET_DISPATCHHANDLER_H__


#include <csp/csplib/net/NetBase.h>
#include <csp/csplib/net/NetworkMessage.h>
#include <csp/csplib/net/MessageHandler.h>
#include <csp/csplib/net/MessageQueue.h>
#include <csp/csplib/net/Callbacks.h>
#include <csp/csplib/util/HashUtility.h>

#include <vector>


CSP_NAMESPACE


/** A MessageHandler that dispatches messages to callback handlers
 *  based on message id.
 *  @ingroup net
 */
class DispatchHandler: public MessageHandler {
public:
	/** Construct a new DispatchHandler.
	 *
	 *  @param queue A MessageQueue that is passed to the message handlers for
	 *    sending responses.
	 */
	DispatchHandler(Ref<MessageQueue> const &queue);
	virtual ~DispatchHandler();

	/** Register a new handler for a specific message type.
	 *
	 *  @param instance The instance that will handle the message.
	 *  @param handler The instance method that will handle the message.
	 */
	template <class CLASS, class MSG>
	void registerHandler(CLASS *instance, void (CLASS::*handler)(Ref<MSG> const &, Ref<MessageQueue> const &)) {
		_registerHandler(MSG::_getName(), MSG::_getId(), new MessageCallback<CLASS, MSG>(instance, handler));
	}

	/** Set a default handler for messages that do not have explicit handlers.
	 *
	 *  @param instance The instance that will handle the message.
	 *  @param handler The instance method that will handle the message.
	 */
	template <class CLASS>
	void setDefaultHandler(CLASS *instance, void (CLASS::*handler)(Ref<NetworkMessage> const &, Ref<MessageQueue> const &)) {
		m_DefaultHandler = new DefaultCallback<CLASS>(instance, handler);
	}

	/** Handle an incoming message by dispatching to the appropriate handler.
	 *
	 *  @param msg The massage to handle.
	 */
	virtual void handleMessage(Ref<NetworkMessage> const &msg);

	/** Remove the handler for the specified message type.
	 *
	 *  @param id The message id, i.e. MESSAGE_CLASS::_getId().
	 */
	void disconnect(NetworkMessage::Id id);

	/** Dispatch a message to the appropriate handler.
	 *
	 *  @param msg The massage to handle.
	 *  @return True if the message was handled, false otherwise.  The default
	 *    handler is not called.
	 */
	bool dispatch(Ref<NetworkMessage> const &msg) const;

private:
	typedef TaggedRecord::Id MessageId;
	typedef HashMap<TaggedRecord::Id, Ref<BaseCallback> >::Type DispatchMap;

	void _registerHandler(std::string const &name, const MessageId id, Ref<BaseCallback> const &handler);
	inline Ref<BaseCallback> getCallback(const MessageId id) const;

	DispatchMap m_DispatchMap;
	Ref<MessageQueue> m_Queue;
	Ref<BaseCallback> m_DefaultHandler;
};


CSP_NAMESPACE_END

#endif // __CSPLIB_NET_DISPATCHHANDLER_H__

