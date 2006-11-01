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

CSP_NAMESPACE

namespace wf {

struct Size {
	Size() : m_W(0.0), m_H(0.0) {}
	Size(double w, double h) : m_W(w), m_H(h) {}
	virtual ~Size() {}

	double m_W, m_H;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_SIZE_H__
