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


#include <csp/cspsim/System.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/f16/F16Channels.h>

#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/Vector3.h>

CSP_NAMESPACE

class F16INS: public System {
public:
	CSP_DECLARE_OBJECT(F16INS)

	F16INS(): m_ResponseTime(0.0) { }
	virtual ~F16INS() { }

	virtual void registerChannels(Bus* bus) {
		b_INSAttitude = bus->registerLocalDataChannel<Quat>(bus::F16::INSAttitude, Quat());
	}

	virtual void importChannels(Bus* bus) {
		b_Attitude = bus->getChannel(bus::Kinetics::Attitude);
		b_Position = bus->getChannel(bus::Kinetics::Position);
		b_Velocity = bus->getChannel(bus::Kinetics::Velocity);
	}

protected:
	virtual double onUpdate(double dt) {
		double f = dt / (dt + m_ResponseTime);
		double g = 1.0 - f;
		Quat &a0 = b_INSAttitude->value();
		Quat const &a1 = b_Attitude->value();
		a0.w() = a0.w() * g + a1.w() * f;
		a0.x() = a0.x() * g + a1.x() * f;
		a0.y() = a0.y() * g + a1.y() * f;
		a0.z() = a0.z() * g + a1.z() * f;
		return 0.0;
	}

	DataChannel<Quat>::RefT b_INSAttitude;
	DataChannel<Quat>::CRefT b_Attitude;
	DataChannel<Vector3>::CRefT b_Position;
	DataChannel<Vector3>::CRefT b_Velocity;

	double m_ResponseTime;
};


CSP_XML_BEGIN(F16INS)
	CSP_DEF("ins_attitude_response_time", m_ResponseTime, false)
CSP_XML_END

CSP_NAMESPACE_END

