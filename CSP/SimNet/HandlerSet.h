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
 * @file HandlerSet.h
 *
 */

#ifndef __SIMNET_HANDLERSET_H__
#define __SIMNET_HANDLERSET_H__

#include <vector>
#include <algorithm>

namespace simnet {

/** Class for managing a set of handlers.  Effectively this is just a
 *  thin wrapper around std::vector, with a few convenience methods
 *  for common operations.  std::vector was chosen to speed up handler
 *  calls, since iteration over a vector is significantly faster than
 *  maps and sets.
 */
template <class HANDLER>
class HandlerSet {
	typedef typename HANDLER::Ref HandlerRef;

	// std::set would be slighly cleaner but less efficient
	typedef std::vector<HandlerRef> Handlers;

	Handlers m_Handlers;

public:
	/** Add a handler to the set.  Will be called (in the order added), but
	 *  apply().
	 */
	void addHandler(HandlerRef handler) {
		assert(handler.valid());
		if (!hasHandler(handler)) m_Handlers.push_back(handler);
	}

	/** Remove a handler from the set.  All the performance implications of
	 *  removing elements from a std::vector apply.
	 *
	 *  @returns true if the handler was removed; false otherwise.
	 */
	bool removeHandler(HandlerRef handler) {
		typename Handlers::iterator iter = std::find(m_Handlers.begin(), m_Handlers.end(), handler);
		if (iter != m_Handlers.end()) {
			m_Handlers.erase(iter);
			return true;
		}
		return false;
	}

	/** Test if a handler is in the set.  All the performance implications of
	 *  finding elements from a std::vector apply.
	 *
	 *  @returns true if the handler was found; false otherwise.
	 */
	bool hasHandler(HandlerRef handler) {
		typename Handlers::iterator iter = std::find(m_Handlers.begin(), m_Handlers.end(), handler);
		return (iter != m_Handlers.end());
	}

	/** Apply a unary function object to all handlers.
	 *
	 *  @param callback a unary function object implementing void operator()(HANDLER&).
	 */
	template <class CALLBACK>
	inline void apply(CALLBACK &callback) {
		std::for_each(m_Handlers.begin(), m_Handlers.end(), callback);
	}

};

} // namespace simnet

#endif // __SIMNET_HANDLERSET_H__
