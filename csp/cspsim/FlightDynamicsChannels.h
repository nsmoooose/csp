#pragma once
// Combat Simulator Project
// Copyright (C) 2003-2005 The Combat Simulator Project
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
 * @file FlightDynamicsChannels.h
 *
 **/

namespace csp {

namespace bus {

struct FlightDynamics {
	static const char *Alpha;
	static const char *Beta;
	static const char *Airspeed;
	static const char *QBar;
	static const char *GForce;  // DEPRECATED; use "G" instead.
	static const char *G;  // normal G, includes gravity and rotation
	static const char *LateralG;  // includes gravity and rotation
	static const char *PressureAltitude;
	static const char *VerticalVelocity;
};

} // namespace bus

} // namespace csp
