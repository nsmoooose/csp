// Combat Simulator Project - CSPSim
// Copyright (C) 2003 The Combat Simulator Project
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
 * @file SynchronousUpdate.cpp
 *
 **/

#include "SynchronousUpdate.h"
#include "Log.h"


int UpdateProxy::update(double time) {
	if (!m_Target) return DEAD;
	double dt = m_Target->onUpdate(time - m_LastUpdateTime);
	if (dt < 0.0) {
		detachTarget();
		return DEAD;
	}
	m_LastUpdateTime = time;
	m_NextUpdateTime = time + dt;
	return ((dt > 0.0) ? DELAYED : IMMEDIATE);
}


UpdateProxy::Ref UpdateMaster::registerUpdate(UpdateTarget *target, double delay) {
	UpdateProxy::Ref proxy;
	if (target) {
		delay = std::max<double>(0.0, delay);
		proxy = new UpdateProxy(target, this, m_Time + delay);
		if (delay > 0.0) {
			m_DelayQueue.push_back(proxy);
			std::push_heap(m_DelayQueue.begin(), m_DelayQueue.end(), m_Priority);
		} else {
			m_ShortList.push_back(proxy);
		}
	}
	return proxy;
}


void UpdateMaster::update(double dt) {
	m_Time += dt;
	m_Transfer.clear();
	if (!m_ShortList.empty()) {
		UpdateList::iterator iter = m_ShortList.begin();
		UpdateList::iterator end = m_ShortList.end();
		UpdateList::iterator remove;
		while (iter != end) {
			int op = (*iter)->update(m_Time);
			switch (op) {
				case UpdateProxy::IMMEDIATE:
					++iter;
					break;
				case UpdateProxy::DELAYED:
					m_Transfer.push_back(*iter);
					// fall through
				case UpdateProxy::DEAD:
					remove = iter++;
					m_ShortList.erase(remove);
					break;
				default:
					break;
			}
		}
	}
	if (!m_DelayQueue.empty()) {
		while (m_DelayQueue.front()->nextUpdateTime() <= m_Time) {
			std::pop_heap(m_DelayQueue.begin(), m_DelayQueue.end(), m_Priority);
			int op = m_DelayQueue.back()->update(m_Time);
			switch (op) {
				case UpdateProxy::DELAYED:
					std::push_heap(m_DelayQueue.begin(), m_DelayQueue.end(), m_Priority);
					break;
				case UpdateProxy::IMMEDIATE:
					m_ShortList.push_back(m_DelayQueue.back());
					// fall through
				case UpdateProxy::DEAD:
					m_DelayQueue.pop_back();
					break;
				default:
					break;
			}
		}
	}
	if (!m_Transfer.empty()) {
		UpdateVector::iterator iter = m_Transfer.begin();
		UpdateVector::iterator end = m_Transfer.end();
		for (; iter != end; ++iter) {
			m_DelayQueue.push_back(*iter);
			std::push_heap(m_DelayQueue.begin(), m_DelayQueue.end(), m_Priority);
		}
	}
}

void UpdateTarget::detachUpdateProxy() { 
	m_UpdateProxy = 0; 
}

void UpdateTarget::disconnectFromUpdateMaster() {
	if (m_UpdateProxy.valid()) {
		m_UpdateProxy->targetSelfDetach();
	}
}

void UpdateTarget::registerUpdate(UpdateMaster *master) {
	// TODO disconnect when master == NULL?
	if (master) {
		if (m_UpdateProxy.valid()) m_UpdateProxy->targetSelfDetach();
		m_UpdateProxy = master->registerUpdate(this);
		CSP_LOG(APP, DEBUG, "Registering update with master (master=" << master << ", target=" << this << ")");
	}
}

UpdateTarget::~UpdateTarget() {
	if (m_UpdateProxy.valid()) m_UpdateProxy->targetSelfDetach();
}


UpdateMaster *UpdateTarget::getMaster() const { 
	if (!m_UpdateProxy) return 0;
	return m_UpdateProxy->getMaster();
}

