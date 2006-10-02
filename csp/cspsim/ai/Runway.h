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


// TODO move out of cspsim/ai?

#ifndef __CSPSIM_AI_RUNWAY_H__
#define __CSPSIM_AI_RUNWAY_H__

#include <csp/csplib/util/Referenced.h>
#include <csp/csplib/data/Matrix3.h>
#include <csp/csplib/data/Vector3.h>

CSP_NAMESPACE

struct Runway: public Referenced {
	Runway(): valid(false) {}
	void init() {
		direction.normalize();
		z.normalize();
		Vector3 y = direction ^ z;
		glide = Matrix3::rotate(PI - glideslope, y) * direction;
		valid = true;
	}
	Vector3 aimpoint;
	Vector3 direction;
	Vector3 glide;
	Vector3 z;
	double length;
	double width;
	double glideslope;
	bool valid;
};

CSP_NAMESPACE_END

#endif  // __CSPSIM_AI_RUNWAY_H__

