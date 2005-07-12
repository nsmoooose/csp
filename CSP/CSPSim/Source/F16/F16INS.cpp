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


#include <System.h>
#include <KineticsChannels.h>
#include <F16/F16Channels.h>

#include <SimData/Quat.h>
#include <SimData/Vector3.h>


class F16INS: public System {
public:
	SIMDATA_DECLARE_OBJECT(F16INS)

	F16INS(): m_ResponseTime(0.0) { }
	virtual ~F16INS() { }

	virtual void registerChannels(Bus* bus) {
		b_INSAttitude = bus->registerLocalDataChannel<simdata::Quat>(bus::F16::INSAttitude, simdata::Quat());
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
		simdata::Quat &a0 = b_INSAttitude->value();
		simdata::Quat const &a1 = b_Attitude->value();
		a0.w() = a0.w() * g + a1.w() * f;
		a0.x() = a0.x() * g + a1.x() * f;
		a0.y() = a0.y() * g + a1.y() * f;
		a0.z() = a0.z() * g + a1.z() * f;
		return 0.0;
	}

	DataChannel<simdata::Quat>::Ref b_INSAttitude;
	DataChannel<simdata::Quat>::CRef b_Attitude;
	DataChannel<simdata::Vector3>::CRef b_Position;
	DataChannel<simdata::Vector3>::CRef b_Velocity;

	double m_ResponseTime;
};


SIMDATA_XML_BEGIN(F16INS)
	SIMDATA_DEF("ins_attitude_response_time", m_ResponseTime, false)
SIMDATA_XML_END

