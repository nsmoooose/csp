// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file Rectangle.h
 *
 **/
#ifndef __CSPSIM_WF_RECTANGLE_H__
#define __CSPSIM_WF_RECTANGLE_H__

#include <csp/cspwf/Point.h>

namespace csp {

namespace wf {

struct Rectangle {
	typedef float Value_type;
	Value_type x0, y0, x1, y1;
	Rectangle() {}
	Rectangle(Value_type x0_, Value_type y0_, Value_type x1_, Value_type y1_): x0(x0_), y0(y0_), x1(x1_), y1(y1_) {}
	inline Value_type width() const { return x1 - x0; }
	inline Value_type height() const { return y1 - y0; }
	inline bool pointInRectangle(Point& p) {
		return p.x >= x0 && p.x <= x1 && p.y >= y0 && p.y <= y1;
	}
};

} // namespace wf

} // namespace csp

#endif // __CSPSIM_WF_RECTANGLE_H__
