// Combat Simulator Project
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
 * @file F16INS.cpp
 *
 **/

#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/f16/F16Channels.h>
#include <csp/cspsim/f16/F16INS.h>

#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/Vector3.h>

namespace csp {

CSP_XML_BEGIN(F16INS)
	CSP_DEF("ins_attitude_response_time", m_ResponseTime, false)
CSP_XML_END

F16INS::F16INS() : m_ResponseTime(0.0) { }
F16INS::~F16INS() { }

void F16INS::registerChannels(Bus* bus) {
	b_INSAttitude = bus->registerLocalDataChannel<Quat>(bus::F16::INSAttitude, Quat());
}

void F16INS::importChannels(Bus* bus) {
	b_Attitude = bus->getChannel(bus::Kinetics::Attitude);
	b_Position = bus->getChannel(bus::Kinetics::Position);
	b_Velocity = bus->getChannel(bus::Kinetics::Velocity);
}

/** 
 * @todo Implement an INS error model (for background, see NAWCWPNS TM 8128, "Basic Inertial Navigation," http://www.fas.org/spp/military/program/nav/basicnav.pdf).
 */
double F16INS::onUpdate(double dt) {
	double f = 1.0 - m_ResponseTime / (dt + m_ResponseTime);
	Quat &a0 = b_INSAttitude->value();
	Quat const &a1 = b_Attitude->value();
	a0.slerp(f, a0, a1);
	return 0.0;
}

} // namespace csp

