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
 * @file Dispatch.h
 * @brief Macros for dispatching network messages through Object
 *   class hierarchies.
 *
 * Sample use:
 *
 * @code
 *
 * // sample base class, inherits from MessageDispatchBase
 * class Bar: public MessageDispatchBase {
 *
 *   // interface (omitted)
 *
 * };
 *
 *
 * // subclass defining a few message handlers
 * class Foo: public Bar {
 *
 * public:
 *
 *   // message dispatch logic (boilerplate)
 *   MESSAGE_DISPATCH(Foo, Bar)
 *     DISPATCH(UpdateMessage, handleUpdate)
 *     DISPATCH(ConfigMessage, handleConfig)
 *     DISPATCH(TrackingMessage, handleTracking)
 *   MESSAGE_DISPATCH_END
 *
 * protected:
 *
 *   // class message handlers.
 *   bool handleUpdate(csp::Ref<UpdateMessage>);
 *   bool handleConfig(csp::Ref<ConfigMessage>);
 *   bool handleTracking(csp::Ref<TrackingMessage>);
 *
 *   // optional handler called if normal dispatch fails.  should call
 *   // dispatchMessage() for all child objects that support messaging.
 *   virtual bool childDispatch(csp::Ref<TaggedMessage> const &);
 * };
 * @endcode
 *
 **/


#ifndef __CSPLIB_UTIL_DISPATCH_H__
#define __CSPLIB_UTIL_DISPATCH_H__


#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/net/TaggedRecord.h>

CSP_NAMESPACE

// classname is not used currently, but it may be handy to have in the future.
#define MESSAGE_DISPATCH(classname, superclass) \
	inline bool _parentDispatch(CSP(Ref)<CSP(TaggedRecord)> const &record) { \
		return superclass::dispatchMessage(record); \
	} \
	virtual bool dispatchMessage(CSP(Ref)<CSP(TaggedRecord)> const &record) { \
		switch (record->getId()) { \

#define DISPATCH(message, handler) \
	case message::_getId(): handler(record); return true;

#define END_MESSAGE_DISPATCH \
	return _parentDispatch(record); \
}


/** Base class for Objects supporting message dispatch.
 *
 *  Inherit from this class and use the MESSAGE_DISPATCH macros to
 *  bind handlers to specific message types.
 */
class MessageDispatcher {

public:
	virtual bool dispatchMessage(Ref<TaggedRecord> const &record) {
		return childDispatch(record);
	}

protected:
	virtual bool childDispatch(Ref<TaggedRecord> const &/*record*/) {
		return false;
	}

public:
	virtual ~MessageDispatcher() {}
};

CSP_NAMESPACE_END

#endif // __CSPLIB_UTIL_DISPATCH_H__

