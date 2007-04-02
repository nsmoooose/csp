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
 * @file Point.h
 *
 **/

#ifndef __CSPSIM_WF_POINT_H__
#define __CSPSIM_WF_POINT_H__

#include <csp/cspsim/wf/Size.h>

CSP_NAMESPACE

namespace wf {

struct Point {
	Point() : x(0.0), y(0.0) {}
	Point(double _x, double _y) : x(_x), y(_y) {}
	Point(const Point& point, const Size& size) : x(point.x + size.width), y(point.y - size.height) {}
	virtual ~Point() {}

	double x, y;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_POINT_H__

