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
 * @file DispatchManager.h
 *
 * Classes for dispatching messages through a class and/or object
 * hierarchy.  Objects receiving messages subclass DispatchTarget,
 * and use the SIMNET_DISPATCH macros to associate handlers with
 * specific message types.  Internally, DispatchTarget subclasses
 * use virtual method calls to find the appropriate handler for a
 * given message.
 *
 * DispatchManager passes messages to DispatchTargets, and caches
 * the most common object/handler pairs for immediate dispatch.
 * This eliminates the (relatively) slow virtual-method dispatch
 * process for the vast majority of messages.
 *
 * When using DispatchManager, care must be taken to ensure that
 * cached message handlers are valid.  Any changes to the objects
 * that could invalidate handler instances must trigger a call to
 * DispatchManager::invalidateCache().  For example, events that
 * (potentially) trigger such changes could pass through a central
 * point has access to the DispatchManager and can proactively
 * invalidate the cache.
 */

#ifndef __SIMNET_DISPATCHMANAGER_H__
#define __SIMNET_DISPATCHMANAGER_H__

#include <SimNet/Callbacks.h>
#include <SimNet/DispatchCache.h>


namespace simnet {


class DispatchTarget;


/** DispatchManager passes messages to DispatchTargets, and caches
 *  the most common object/handler pairs for immediate dispatch.
 */
class DispatchManager: public simdata::Referenced {
public:
	typedef simdata::Ref<DispatchManager> Ref;

	/** Construct a new DispatchManager.
	 *
	 *  @param queue a MessageQueue that is provided to handlers to send
	 *    responses.
	 *  @param cache_size the number of recently accessed handlers to cache.
	 */
	DispatchManager(MessageQueue::Ref queue, unsigned cache_size);

	/** Dispatch a message to a specific handler.
	 *
	 *  This is a quasi-internal method used by the SIMNET_DISPATCH macro.
	 *  Do not call this method directly.
	 */
	template <class CLASS, class MSG>
	inline bool handle(CLASS *instance, typename MessageCallback<CLASS, MSG>::Method handler) {
		// hmm, first time i've encountered 14.2.4 of the 1997 c++ draft.  keyword
		// "template" required after -> to resolve ambiguity.
		m_Cache->template insert<CLASS, MSG>(instance, handler, m_Message);
		(instance->*handler)(NetworkMessage::FastCast<MSG>(m_Message), m_Queue);
		return true;
	}

	/** Dispatch a message to a DispatchTarget.
	 *
	 *  @param target a DispatchTarget to receive the message.  It is the caller's
	 *    responsibility to determining which DispatchTarget should receive a given
	 *    message (typically by examining the routing_data field of NetworkMessage).
	 *  @param msg the message to dispatch.
	 *  @return true if the message was handled, false otherwise.
	 */
	bool dispatch(DispatchTarget *target, NetworkMessage::Ref const &msg);

	/** Returns the id of the message currently being dispatched.
	 *
	 *  This is a quasi-internal method used by the SIMNET_DECLARE_DISPATCH
	 *  macro.  Do not call this method directly.
	 */
	inline NetworkMessage::Id id() { return m_Message.valid() ? m_Message->getId() : 0; }

	/** Discard all cached handlers.
	 *
	 *  Call this method before any event that may cause a message handler
	 *  to change.  Some examples:
	 *   - when a new DispatchTarget is added
	 *   - when an existing DispatchTarget is destroyed
	 *   - when an existing DispatchTarget adds or removes an internal DispatchTarget
	 *     member that handles messages for the parent instance.
	 *  When in doubt, invalidate the cache.  Failure to do so when needed can result
	 *  in incorrect dispatch and/or memory corruption.
	 */
	void invalidateCache();

private:
	MessageQueue::Ref m_Queue;
	DispatchCache::Ref m_Cache;
	NetworkMessage::Ref m_Message;
};

}  // namespace simnet

#endif // __SIMNET_DISPATCHMANAGER_H__


