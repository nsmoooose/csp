#pragma once
/* Combat Simulator Project
 * Copyright 2004 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/**
 * @file Cache.h
 * @brief Provides a templated cache of most recently used key-value
 *        pairs.
 *
 * Example usage:
 *
 * @code
 * MostRecentlyUsedCache<int, std::string> cache(5);  // 5 elements max
 *
 * cache.insert(8, "is enough");
 * cache.insert(3, "is company");
 * cache.insert(7, "lucky");
 * cache.insert(100, "years in a century");
 * cache.insert(42, "The Answer to Life, the Universe, and Everything");
 * cache.insert(10, "fingers"); // now 6 element, so last used (8) is dropped
 *
 * std:string value;
 *
 * assert(cache.find(8, value) == false);    // 8 not in cache
 * assert(cache.find(7, value) == true);     // fast hash_map lookup -- found
 * assert(value == "lucky");
 * cache.insert(4, "quarters");              // last used (3) is dropped
 * assert(cache.find(3, value) == false);    // 3 not in cache
 * cache.insert(52, "weeks");                // last used (100) is dropped
 * assert(cache.find(7, value) == true);     // 7 still in cache!
 * assert(value == "lucky");
 * assert(cache.find(100, value) == false);  // 100 not in cache
 *
 * // cache order is now 7, 52, 4, 10, 42 (oldest)
 * @endcode
 *
 */

#include <cassert>
#include <list>

#include <csp/csplib/util/HashUtility.h>
#include <csp/csplib/util/Properties.h>


namespace csp {


/** A cache of most recently used (MRU) key-value pairs.
 *
 *  Provides hash-key based lookups O(1) from a subset of a larger collection
 *  of items.  The subset is chosen to include the most frequently accessed
 *  items in the collection.
 */
template <typename KEY, typename TYPE, typename HASH_COMPARE = HashCompare<KEY> >
class MostRecentlyUsedCache: NonCopyable {
	typedef TYPE DataType;
	typedef KEY CacheKey;
	typedef std::list<CacheKey> MostRecentlyUsedList;
	typedef std::pair<typename MostRecentlyUsedList::iterator, TYPE> CacheElement;
	typedef typename HashMap<CacheKey, CacheElement, HASH_COMPARE>::Type CacheMap;
	
	mutable CacheMap m_CacheMap;
	mutable MostRecentlyUsedList m_MRU;
	const unsigned m_Limit;

public:

	/** Create a new cache.
	 *
	 *  @param limit The maximum number of elements the cache will hold.
	 */
	MostRecentlyUsedCache(unsigned limit) : m_Limit(limit) { }

	/** Find an item in the cache.  If the item is found it is
	 *  promoted to the most-recently used.
	 *
	 *  @param key The key of the item to insert.
	 *  @param data The item data, if found.
	 *  @return True if the item was found and copies the item to the data parameter.
	 */
	bool find(CacheKey const &key, DataType &data) const {
		typename CacheMap::iterator iter = m_CacheMap.find(key);
		if (iter == m_CacheMap.end()) {
			return false;
		}
		typename MostRecentlyUsedList::iterator mru = iter->second.first;
		if (mru != m_MRU.begin()) {
			m_MRU.push_front(*mru);
			m_MRU.erase(mru);
			iter->second.first = m_MRU.begin();
		}
		data = iter->second.second;
		return true;
	}

	/** Insert an item into the cache.  If the item already exists in the
	 *  cache it is promoted to the most-recently-used.  If the new cache
	 *  size is greater than the limit set in the constructor, the least
	 *  recently used item is dropped from the cache.
	 *
	 *  @param key The key of the item to insert.
	 *  @param data the item data.
	 */
	void insert(CacheKey const &key, DataType const &data) {
		m_MRU.push_front(key);
		const CacheElement element(m_MRU.begin(), data);
		const typename CacheMap::value_type value(key, element);
		std::pair<typename CacheMap::iterator, bool> receipt = m_CacheMap.insert(value);
		if (!receipt.second) {
			m_MRU.erase(receipt.first->second.first);
			receipt.first->second.first = m_MRU.begin();
		}
		if (m_CacheMap.size() > m_Limit) {
			typename CacheMap::iterator iter = m_CacheMap.find(m_MRU.back());
			assert(iter != m_CacheMap.end());
			m_CacheMap.erase(iter);
			m_MRU.pop_back();
		}
	}

	/** Remove an item from the cache.
	 *
	 *  @param key The key of the item to remove.
	 *  @return true if the item was found, false otherwise.
	 */
	bool erase(CacheKey const &key) {
		typename CacheMap::iterator iter = m_CacheMap.find(key);
		if (iter == m_CacheMap.end()) return false;
		typename MostRecentlyUsedList::iterator mru = iter->second.first;
		m_MRU.erase(mru);
		m_CacheMap.erase(iter);
		return true;
	}

	/** Remove all items from the cache.
	 */
	void clear() {
		m_CacheMap.clear();
		m_MRU.clear();
	}

};


} // namespace csp
