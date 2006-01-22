// Combat Simulator Project
// Copyright (C) 2006 The Combat Simulator Project
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
 * @file f16/MasterModes.cpp
 *
 **/

#include <csp/cspsim/f16/MasterModes.h>

CSP_NAMESPACE

namespace f16 {

const Enumeration MasterModes("NAV AA AG MSL DGFT S-J E-J");

const MasterMode NAV("NAV");
const MasterMode AA("AA");
const MasterMode AG("AG");
const MasterMode MSL("MSL");
const MasterMode DGFT("DGFT");
const MasterMode SJ("S-J");
const MasterMode EJ("E-J");

void MasterModeSelection::updateMode() {
	const MasterMode old = m_Mode;
	if (m_EmergencyJettison) {
		m_Mode = EJ;
	} else if (m_Missile) {
		m_Mode = MSL;
	} else if (m_Dogfight) {
		m_Mode = DGFT;
	} else if (m_SelectiveJettison) {
		m_Mode = SJ;
	} else {
		m_Mode = m_Base;
	}
	if (old != m_Mode) m_ChangeSignal.emit(m_Mode);
}

} // namespace f16

CSP_NAMESPACE_END

