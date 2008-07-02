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
 * @file Size.h
 *
 **/

#ifndef __CSPSIM_WF_SIZE_H__
#define __CSPSIM_WF_SIZE_H__

#include <csp/csplib/util/Namespace.h>

namespace csp {

namespace wf {

struct Size {
	Size() : width(0.0), height(0.0) {}
	Size(double w, double h) : width(w), height(h) {}
	virtual ~Size() {}

	double width, height;
};

} // namespace wf

} // namespace csp

#endif // __CSPSIM_WF_SIZE_H__
