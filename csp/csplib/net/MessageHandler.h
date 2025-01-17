#pragma once
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
 * @file MessageHandler.h
 *
 */

#include <csp/csplib/net/NetworkMessage.h>
#include <csp/csplib/util/Ref.h>

namespace csp {



/** Abstract interface for handlers of received network messages.
 *
 *  Subclass MessageHandler and add your handler to a PacketDecoder
 *  to process incoming messages.
 *  @ingroup net
 */
class CSPLIB_EXPORT MessageHandler: public Referenced {
public:
	/** Called by PacketDecoder to handle an incoming message.
	 */
	virtual void handleMessage(Ref<NetworkMessage> const &message)=0;

	virtual ~MessageHandler() { }

	/** Convenience factory for generating MessageHandler callbacks.
	 *  Using a templated static method allows the compiler to infer
	 *  the template type, as opposed to the CallbackHandler class
	 *  which requires explicit specialization.
	 */
	template <class CLASS>
	static inline Ref<MessageHandler> Callback(CLASS *instance, void (CLASS::*method)(Ref<NetworkMessage> const &));

};


/** Specialized message handler for method callbacks.  Use MessageHandler::Callback
 *  to create new callback handlers.  Note that CallbackHandler is just one of many
 *  MessageHandler specializations.  It is declared in MessageHandler.h purely for
 *  convenience.
 *  @ingroup net
 */
template <class CLASS>
class CallbackHandler: public MessageHandler {
public:
	typedef void (CLASS::*Method)(Ref<NetworkMessage> const &);
	CallbackHandler(CLASS *instance, Method method): m_Instance(instance), m_Method(method) { }
	virtual void handleMessage(Ref<NetworkMessage> const &message) { (m_Instance->*m_Method)(message); }
private:
	CLASS *m_Instance;
	Method m_Method;
};


template <class CLASS>
inline Ref<MessageHandler> MessageHandler::Callback(CLASS *instance, void (CLASS::*method)(Ref<NetworkMessage> const &)) {
	return new CallbackHandler<CLASS>(instance, method);
}


} // namespace csp
