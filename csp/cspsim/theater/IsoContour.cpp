// Combat Simulator Project
// Copyright (C) 2004-2005 The Combat Simulator Project
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
 * @file IsoContour.cpp
 *
 **/

#include <csp/cspsim/theater/IsoContour.h>
#include <csp/csplib/data/ObjectInterface.h>

namespace csp {

CSP_XML_BEGIN(IsoContour)
CSP_XML_END

CSP_XML_BEGIN(RectangularCurve)
	CSP_DEF("width", m_Width, true)
	CSP_DEF("height", m_Height, true)
CSP_XML_END

CSP_XML_BEGIN(Circle)
	CSP_DEF("radius", m_Radius, true)
CSP_XML_END

} // namespace csp

