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
 * @file Callbacks.h
 *
 * Callback types and closures for message handling.
 */

#ifndef __SIMNET_CALLBACKS_H__
#define __SIMNET_CALLBACKS_H__


#include <SimNet/NetBase.h>
#include <SimNet/NetworkMessage.h>
#include <SimNet/MessageHandler.h>
#include <SimNet/MessageQueue.h>


namespace simnet {


class BaseCallback: public simdata::Referenced {
public:
	typedef simdata::Ref<BaseCallback> Ref;
	virtual void call(NetworkMessage::Ref msg, MessageQueue::Ref const &) = 0;
};


template <class CLASS>
class DefaultCallback: public BaseCallback {
	typedef NetworkMessage::Ref (CLASS::*Method)(simdata::Ref<NetworkMessage>, MessageQueue::Ref const &);
	CLASS *m_Instance;
	Method m_Method;

public:
	DefaultCallback(CLASS *instance, Method method): m_Instance(instance), m_Method(method) {}
	virtual void call(NetworkMessage::Ref const &msg, MessageQueue::Ref const &queue) {
		(m_Instance->*m_Method)(msg, queue);
	}
};


template <class CLASS, class MSG>
class MessageCallback: public BaseCallback {
public:
	typedef CLASS Class;
	typedef void (CLASS::*Method)(typename MSG::Ref const &, MessageQueue::Ref const &);
	MessageCallback(CLASS *instance, Method method): m_Instance(instance), m_Method(method) {}
	virtual void call(NetworkMessage::Ref msg, MessageQueue::Ref const &queue) {
		(m_Instance->*m_Method)(NetworkMessage::FastCast<MSG>(msg), queue);
	}
private:
	CLASS *m_Instance;
	Method m_Method;
};


/** Templated callback for handling network messages.
 */
template <class CLASS, class MESSAGE>
class CallbackHandler: public MessageHandler {
public:
	typedef void (CLASS::*Callback)(typename MESSAGE::Ref const &);
	CallbackHandler(CLASS *instance, Callback callback): m_Instance(instance), m_Callback(callback) { }
	inline void call(NetworkMessage::Ref const &message) {
		(m_Instance->*m_Callback)(NetworkMessage::FastCast<MESSAGE>(message));
	}
	virtual void handleMessage(NetworkMessage::Ref const &message) { call(message); }
private:
	CLASS *m_Instance;
	Callback m_Callback;
};



} // namespace simnet

#endif // __SIMNET_CALLBACKS_H__

