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

// TODO expand this definition, move out of cspsim/ai

#include <csp/csplib/util/Referenced.h>
#include <csp/csplib/data/Vector3.h>

namespace csp {
namespace ai {

class Waypoint: public Referenced {
public:
	Waypoint(double x, double y, double z, double speed): position(x, y, z), speed(speed) {}
	Vector3 position;
	double speed;
};

} // end namespace ai
} // end namespace csp
