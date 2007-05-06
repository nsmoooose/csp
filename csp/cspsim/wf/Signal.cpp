// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file Signal.cpp
 *
 **/

#include <csp/cspsim/wf/Signal.h>

CSP_NAMESPACE

namespace wf {

Signal::Signal() {
}

Signal::~Signal() {
}

void Signal::connect(Slot* slot) {
	m_InvocationList.push_back(slot);
}

void Signal::disconnect(Slot* slot) {
	for(int index=m_InvocationList.size()-1;index >= 0;--index) {
		if(m_InvocationList.at(index).get() == slot) {
			m_InvocationList.erase(m_InvocationList.begin()+index);
		}
	}
}

void Signal::emit(SignalData* data) {
	SlotVector::iterator slot = m_InvocationList.begin();
	for(;slot != m_InvocationList.end();++slot) {
		(*slot)->notify(data);
	}
}

void Signal::dispose() {
	m_InvocationList.clear();
}

} // namespace wf

CSP_NAMESPACE_END
