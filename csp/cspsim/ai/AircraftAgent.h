#pragma once
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
 * @file AircraftAgent.h
 *
 **/

#include <csp/cspsim/System.h>

#include <csp/csplib/data/Date.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/util/TimeStamp.h>

namespace csp {
namespace ai {

class AircraftControl;
class Task;

class AircraftAgent: public System {
public:
	CSP_DECLARE_OBJECT(AircraftAgent)

	AircraftAgent();
	virtual ~AircraftAgent();

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);
	virtual double onUpdate(double dt);
	virtual void getInfo(InfoList &info) const;

protected:
	DataChannel<double>::RefT b_PitchInput;
	DataChannel<double>::RefT b_RollInput;
	DataChannel<double>::RefT b_RudderInput;
	DataChannel<double>::RefT b_LeftBrakeInput;
	DataChannel<double>::RefT b_RightBrakeInput;
	DataChannel<double>::RefT b_ThrottleInput;
	DataChannel<double>::RefT b_AirbrakeInput;

	Ref<AircraftControl> m_AircraftControl;
	Ref<Task> m_Task;
};

} // end namespace ai
} // end namespace csp
