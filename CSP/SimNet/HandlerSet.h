// Combat Simulator Project - FlightSim Demo
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

template <class HANDLER>
class HandlerSet {
	typedef typename HANDLER::Ref HandlerRef;
	// std::set would be slighly cleaner but less efficient
	typedef std::vector<HandlerRef> Handlers;
	Handlers m_Handlers;

public:
	void addHandler(HandlerRef handler) {
		assert(handler.valid());
		if (!hasHandler(handler)) m_Handlers.push_back(handler);
	}

	bool removeHandler(HandlerRef handler) {
		typename Handlers::iterator iter = std::find(m_Handlers.begin(), m_Handlers.end(), handler);
		if (iter != m_Handlers.end()) m_Handlers.erase(iter);
	}

	bool hasHandler(HandlerRef handler) {
		typename Handlers::iterator iter = std::find(m_Handlers.begin(), m_Handlers.end(), handler);
		return (iter != m_Handlers.end());
	}

	template <class CALLBACK>
	inline void apply(CALLBACK &callback) {
		std::for_each(m_Handlers.begin(), m_Handlers.end(), callback);
	}
};

} // namespace simnet

#endif // __SIMNET_HANDLERSET_H__
