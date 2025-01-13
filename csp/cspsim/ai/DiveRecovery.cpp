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

#include <csp/cspsim/ai/DiveRecovery.h>
#include <csp/cspsim/ai/AircraftControl.h>
#include <csp/csplib/util/Math.h>
#include <iostream> // XXX

namespace csp {
namespace ai {

DiveRecovery::DiveRecovery(): AircraftTask("DiveRecovery") {
	addHandler(DIVE_RECOVERY, sigc::mem_fun(*this, &DiveRecovery::onDiveRecovery), "DIVE_RECOVERY");
	next(DIVE_RECOVERY);
}

void DiveRecovery::onDiveRecovery() {
	static const double corner_velocity = 200.0;  // TODO externalize
	bool upright = fabs(ai()->roll()) < toRadians(90.0);
	bool fast = ai()->cas() > corner_velocity + 10.0;

	// TODO consider emergency jettison?

	ai()->setAirbrakeInput(fast ? 1.0 : 0.0);
	ai()->fastRoll(0.0, dt());
	ai()->holdAirspeed(corner_velocity, dt());  // TODO allow AB
	ai()->setPitchInput(upright ? 1.0 : 0.0);

	if (ai()->vv() > 0.0) {
		std::cout << "dive recovery complete\n";
		succeed();
	}
	// TODO impact -> fail()
}

} // end namespace ai
} // end namespace csp
