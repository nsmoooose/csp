// Combat Simulator Project
// Copyright (C) 2007 The Combat Simulator Project
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
 * @file StoresDefinition.cpp
 *
 **/

#include <csp/cspsim/stores/Hardpoint.h>
#include <csp/cspsim/stores/StoresDefinition.h>
#include <csp/cspsim/stores/Stores.h>
#include <csp/csplib/data/ObjectInterface.h>

namespace csp {

CSP_XML_BEGIN(StoresDefinition)
	CSP_DEF("hardpoints", m_Hardpoints, true)
CSP_XML_END

bool StoresDefinition::getHardpointIndex(std::string const &name, unsigned &index) const {
	HardpointMap::const_iterator iter = m_HardpointMap.find(name);
	if (iter == m_HardpointMap.end()) return false;
	index = iter->second;
	return true;
}

void StoresDefinition::postCreate() {
	Object::postCreate();
	for (unsigned i = 0; i < m_Hardpoints.size(); ++i) {
		const bool duplicate = !m_HardpointMap.insert(HardpointMap::value_type(m_Hardpoints[i]->name(), i)).second;
		assert(!duplicate);
	}
}

} // namespace csp

