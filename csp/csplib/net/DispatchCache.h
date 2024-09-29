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
 * @file DispatchCache.h
 *
 * Class for caching frequently used message dispatch callbacks.
 */

#include <csp/csplib/net/Callbacks.h>
#include <csp/csplib/util/Cache.h>
#include <csp/csplib/util/HashUtility.h>


namespace csp {


/** DispatchCache provides a cache of most-frequently-called message disptach
 *  handlers.  The handlers are indexed by (target id, message id) pairs.  The
 *  cache stores a bound handler callback for each pair.  Thus a cache hit
 *  provides immediate access to the desired callback function.  This is
 *  particularly useful in cases where ordinary dispatch resolution is slow,
 *  such as dispatch in a deep object hierarchy, without requiring large
 *  lookup maps.  @see DispatchManager for one use case.
 *  @ingroup net
 */
class CSPLIB_EXPORT DispatchCache: public Referenced {

	typedef uint64 CacheKey;
	typedef MostRecentlyUsedCache<CacheKey, Ref<BaseCallback> > MRUCache;

	/** Construct a cache key by XORing the target and message ids.  Message
	 *  ids are sparse 64-bit hashes, while target ids are dense 24-bit values.
	 */
	static inline uint64 makeCacheKey(uint64 target_id, uint64 message_id) {
		return (target_id ^ message_id);
	}

	MRUCache m_Cache;

public:
	/** Construct a new DispatchCache.
	 *
	 *  @param size the size of the most-recently-used cache.
	 */
	DispatchCache(unsigned size): m_Cache(size) { }

	/** Find a cached handler for a given message.
	 *
	 *  @param msg The message to handle (has both target and message ids)
	 *  @param handler the bound callback to handle the message.  This value
	 *    will only be set if a handler for the specified message was found
	 *    in the cache.  The handler may be null, which indicates that
	 *    previous dispatch attempts for this message failed.
	 *  @return True for cache hits, false otherwise.
	 */
	bool findHandler(Ref<NetworkMessage> const &msg, Ref<BaseCallback> &handler) {
		const CacheKey key = makeCacheKey(msg->getRoutingData(), msg->getId());
		return m_Cache.find(key, handler);
	}

	/** Insert a new handler into the cache.
	 *
	 *  @param instance The instance handling the message.
	 *  @param handler The method handling the message, with signature void (CLASS::*)(MSG const &);
	 *  @param msg The message being handled.
	 */
	template <class CLASS, class MSG>
	void insert(CLASS *instance, typename MessageCallback<CLASS, MSG>::Method handler, Ref<NetworkMessage> const &msg) {
		const CacheKey key = makeCacheKey(msg->getRoutingData(), MSG::_getId());
		m_Cache.insert(key, new MessageCallback<CLASS, MSG>(instance, handler));
	}

	/** Insert a NULL handler into the cache.  This is used to indicate that there
	 *  is no handler for the given message.
	 *
	 *  @param msg The message being handled.
	 */
	void insertNoHandler(Ref<NetworkMessage> const &msg) {
		const CacheKey key = makeCacheKey(msg->getRoutingData(), msg->getId());
		m_Cache.insert(key, 0);
	}

	/** Invalidate (clear) the cache.
	 */
	inline void invalidate() { m_Cache.clear(); }

};


} // namespace csp
