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
 *   using namespace simnet;
 *   using namespace simdata;
 *
 *   class ObjectManager {
 *   public:
 *     void bindHandlers(DispatchHandler::Ref dispatch) {
 *       dispatch->registerHandler(this, &ObjectManager::onUpdate);
 *       dispatch->registerHandler(this, &ObjectManager::onDestroyed);
 *       dispatch->registerDefaultHandler(this, &ObjectManager::onUnknownMessage);
 *     }
 *
 *     // handler methods called when a message of the argument type is received.
 *     // the return value, if set, is automatically queued for transmission.
 *     NetworkMessage::Ref onUpdate(ObjectUpdateMessage::Ref update);
 *     NetworkMessage::Ref onDestroyed(ObjectDestroyedMessage::Ref update);
 *     NetworkMessage::Ref onUnknownMessage(NetworkMessage::Ref update);
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
 *     m_PacketDecoder = new simnet::PacketDecoder();
 *     m_PacketDecoder->addMessageHandler(m_DispatchHandler);
 *
 *     // network interface is manages inbound and outbound packets
 *     m_NetworkInterface = new simnet::NetworkInterface();
 *     m_NetworkInterface->setPacketSource(m_MessageQueue);
 *     m_NetworkInterface->addPacketHandler(m_PacketDecoder);
 *     // other interface initialiazation here...
 *   }
 *   @endcode
 *
 *   TODO(os) simplify the example above using PacketServer
 *
 */

#ifndef __SIMNET_DISPATCHHANDLER_H__
#define __SIMNET_DISPATCHHANDLER_H__


#include <SimNet/NetBase.h>
#include <SimNet/NetworkMessage.h>
#include <SimNet/MessageHandler.h>
#include <SimNet/MessageQueue.h>
#include <SimNet/Callbacks.h>

#include <SimData/hash_map.h>
#include <SimData/HashUtility.h>

#include <vector>


namespace simnet {


/** A MessageHandler that dispatches messages to callback handlers
 *  based on message id.
 */
class DispatchHandler: public simnet::MessageHandler {
public:
	typedef simdata::Ref<DispatchHandler> Ref;

	/** Construct a new DispatchHandler.
	 *
	 *  @param queue A MessageQueue that is passed to the message handlers for
	 *    sending responses.
	 */
	DispatchHandler(simnet::MessageQueue::Ref const &queue);
	virtual ~DispatchHandler();

	/** Register a new handler for a specific message type.
	 *
	 *  @param instance The instance that will handle the message.
	 *  @param handler The instance method that will handle the message.
	 */
	template <class CLASS, class MSG>
	void registerHandler(CLASS *instance, void (CLASS::*handler)(simdata::Ref<MSG> const &, MessageQueue::Ref const &)) {
		_registerHandler(MSG::_getName(), MSG::_getId(), new MessageCallback<CLASS, MSG>(instance, handler));
	}

	/** Set a default handler for messages that do not have explicit handlers.
	 *
	 *  @param instance The instance that will handle the message.
	 *  @param handler The instance method that will handle the message.
	 */
	template <class CLASS>
	void setDefaultHandler(CLASS *instance, void (CLASS::*handler)(simdata::Ref<NetworkMessage> const &, MessageQueue::Ref const &)) {
		m_DefaultHandler = new DefaultCallback<CLASS>(instance, handler);
	}

	/** Handle an incoming message by dispatching to the appropriate handler.
	 *
	 *  @param msg The massage to handle.
	 */
	virtual void handleMessage(NetworkMessage::Ref const &msg);

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
	bool dispatch(NetworkMessage::Ref const &msg) const;

private:
	typedef simdata::TaggedRecord::Id MessageId;
	typedef HASH_MAP<simdata::TaggedRecord::Id, BaseCallback::Ref, simdata::nohash64> DispatchMap;

	void _registerHandler(std::string const &name, const MessageId id, BaseCallback::Ref const &handler);
	inline BaseCallback::Ref getCallback(const MessageId id) const;

	DispatchMap m_DispatchMap;
	simnet::MessageQueue::Ref m_Queue;
	BaseCallback::Ref m_DefaultHandler;
};


} // namespace simnet

#endif // __SIMNET_DISPATCHHANDLER_H__

