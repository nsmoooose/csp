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
 * @file Objective.cpp
 *
 **/


#include <csp/cspsim/theater/Objective.h>
#include <csp/csplib/data/ObjectInterface.h>

namespace csp {

CSP_XML_BEGIN(Objective)
	CSP_DEF("strategic_value_modifier", m_StrategicValueModifier, true)
CSP_XML_END


Objective::Objective() {
}

Objective::~Objective() {
}

} // namespace csp

