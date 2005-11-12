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
 * @file DispatchManager.cpp
 *
 */


#include <SimNet/DispatchManager.h>
#include <SimNet/DispatchTarget.h>
#include <SimNet/NetLog.h>


namespace simnet {


DispatchManager::DispatchManager(MessageQueue::Ref queue, unsigned cache_size)
	: m_Queue(queue), m_Cache(new DispatchCache(cache_size))
{
}

bool DispatchManager::dispatch(DispatchTarget *target, NetworkMessage::Ref const &msg) {
	BaseCallback::Ref callback;
	if (m_Cache->findHandler(msg, callback)) {
		SIMNET_LOG(DEBUG, MESSAGE) << "Found handler in cache";
		if (!callback) return false;
		callback->call(msg, m_Queue);
		return true;
	} else {
		SIMNET_LOG(DEBUG, MESSAGE) << "Did not find handler in cache";
		m_Message = msg;
		bool result = target->dispatch(this);
		if (!result) {
			m_Cache->insertNoHandler(m_Message);
		}
		m_Message = 0;
		return result;
	}
}

void DispatchManager::invalidateCache() {
	m_Cache->invalidate();
}

} // namespace simnet

