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

#include <csp/csplib/util/Math.h>
#include <csp/csplib/util/StringTools.h>

namespace csp {

inline double MeanSiderealTime(double time, double longitude) {
	double GMST0 = toRadians(282.9404 + 356.0470 + 180.0) + toRadians(4.70935E-5 + 0.9856002585) * time;
	return time * toRadians(360.0) + GMST0 + longitude;
}

inline std::string FormatSiderealTime(double angle) {
	double hours = toDegrees(angle) * 24 / 360.0;
	return stringprintf("%02d:%02d:%02d", int(hours) % 24, int(hours * 60) % 60, int(hours * 3600) % 60);
}

} // namespace csp
