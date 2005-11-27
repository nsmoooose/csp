// Combat Simulator Project - CSPSim
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file LocalUpdate.cpp
 *
 **/

#include <csp/csplib/util/LocalUpdate.h>

#include <algorithm>
#include <cassert>


CSP_NAMESPACE

/** Internal helper class for managing update callbacks.  Stores the callback
 *  and tracks update intervals.
 */
class LocalUpdate::PriorityHandler {
public:
	PriorityHandler(Callback const &handler, double interval): m_Handler(handler), m_Next(-1), m_Last(-1), m_Interval(interval) {
		assert(interval > 0.0);
		if (m_Interval <= 0) m_Interval = 0.001;
	}

	bool update(double t) {
		bool remove = false;
		double delay = m_Interval;
		if (m_Next >= 0) {
			const double elapsed_time = t - m_Last;
			const double delta = m_Handler(elapsed_time);
			if (delta > 0) delay = delta; else if (delta < 0) remove = true;
		}
		m_Last = t;
		m_Next = t + delay;
		return !remove;
	}

	double next() const { return m_Next; }
	double interval() const { return m_Interval; }

	struct Order {
		inline bool operator()(PriorityHandler const *lhs, PriorityHandler const *rhs) { return lhs->next() > rhs->next(); }
	};

private:
	Callback m_Handler;
	double m_Next;
	double m_Last;
	double m_Interval;
};


LocalUpdate::~LocalUpdate() {
	for (unsigned i = 0; i < m_Queue.size(); ++i) delete m_Queue[i];
}

void LocalUpdate::_addHandler(Callback const &handler, double interval) {
	m_Queue.push_back(new PriorityHandler(handler, interval));
	std::push_heap(m_Queue.begin(), m_Queue.end(), PriorityHandler::Order());
}

void LocalUpdate::dispatch() {
	assert(!m_Queue.empty());
	while (m_Queue.front()->next() <= m_Time) {
		std::pop_heap(m_Queue.begin(), m_Queue.end(), PriorityHandler::Order());
		if (!m_Queue.back()->update(m_Time)) {
			delete m_Queue.back();
			m_Queue.pop_back();
		} else {
			std::push_heap(m_Queue.begin(), m_Queue.end(), PriorityHandler::Order());
		}
	}
	if (!m_Queue.empty()) m_Next = m_Queue.front()->next();
}

CSP_NAMESPACE_END

