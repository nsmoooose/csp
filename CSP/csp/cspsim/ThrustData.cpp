// Combat Simulator Project
// Copyright (C) 2002, 2005 The Combat Simulator Project
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
 * @file ThrustData.cpp
 *
 **/

#include <csp/cspsim/ThrustData.h>
#include <csp/csplib/data/ObjectInterface.h>

CSP_NAMESPACE

CSP_XML_BEGIN(ThrustData)
	CSP_DEF("idle_thrust", m_IdleThrust, true)
	CSP_DEF("mil_thrust", m_MilitaryThrust, true)
	CSP_DEF("ab_thrust", m_AfterburnerThrust, true)
CSP_XML_END


ThrustData::ThrustData(): m_HasAfterburner(false) {
}

ThrustData::~ThrustData() {
}

double ThrustData::getMil(double altitude, double mach) const {
	return m_MilitaryThrust[static_cast<float>(altitude)][static_cast<float>(mach)];
}

double ThrustData::getIdle(double altitude, double mach) const {
	return m_IdleThrust[static_cast<float>(altitude)][static_cast<float>(mach)];
}

double ThrustData::getAb(double altitude, double mach) const {
	return m_HasAfterburner ? m_AfterburnerThrust[static_cast<float>(altitude)][static_cast<float>(mach)] : getMil(altitude, mach);
}

void ThrustData::postCreate() {
	Object::postCreate();
	m_HasAfterburner = !m_AfterburnerThrust.empty();
}

CSP_NAMESPACE_END

