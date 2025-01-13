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


#include <csp/cspsim/ai/TakeoffTask.h>
#include <csp/cspsim/ai/AircraftControl.h>
#include <csp/cspsim/ai/Runway.h>

namespace csp {
namespace ai {

TakeoffTask::TakeoffTask(): AircraftTask("Takeoff") {
	addHandler(READY, sigc::mem_fun(*this, &TakeoffTask::onReady), "READY");
	addHandler(THROTTLE_UP, sigc::mem_fun(*this, &TakeoffTask::onThrottleUp), "THROTTLE_UP");
	addHandler(RELEASE, sigc::mem_fun(*this, &TakeoffTask::onRelease), "RELEASE");
	addHandler(ROLL, sigc::mem_fun(*this, &TakeoffTask::onRoll), "ROLL");
	addHandler(CLIMB, sigc::mem_fun(*this, &TakeoffTask::onClimb), "CLIMB");
	next(READY);
}

void TakeoffTask::setRunway(Runway const *runway) {
	AircraftTask::setRunway(runway);
	m_RunwayHeading = atan2(runway->direction.x(), runway->direction.y());
}

void TakeoffTask::onReady() {
	if (initial()) {
		ai()->setLeftBrakeInput(1.0);
		ai()->setRightBrakeInput(1.0);
	}
	if (elapsed() > 2.0) next(THROTTLE_UP);
}

void TakeoffTask::onThrottleUp() {
	const double throttle = ai()->throttleInput();
	ai()->setThrottleInput(std::min(0.7, throttle + 0.1 * dt()));
	if (throttle >= 0.7) next(RELEASE);
}

void TakeoffTask::onRelease() {
	ai()->setThrottleInput(1.0);  // burner
	ai()->setBrakes(0.0);
	next(ROLL);
}

void TakeoffTask::onRoll() {
	// TODO externalize roll speed, take stores into account
	if (ai()->cas() > 75.0) {
		double error = toRadians(10.0) - ai()->alpha();
		ai()->setPitchInput(std::min(0.7, error * 10.0));
	}
	if (ai()->ralt() > 10.0) {
		ai()->raiseGear();
		return next(CLIMB);
	}
}

void TakeoffTask::onClimb() {
	if (ai()->alt() >= 200.0) succeed();
	ai()->flyPitchHeading(toRadians(20.0), m_RunwayHeading, dt());
}


void TakeoffTask::init() {
	assert(runway());
	assert(ai());
}

} // end namespace ai
} // end namespace csp
