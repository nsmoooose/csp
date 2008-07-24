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


/**
 * @file f16/Constants.cpp
 *
 **/

#include <csp/cspsim/f16/Constants.h>

namespace csp {

namespace f16 {

namespace AirToGround {
	const Enumeration Submodes("CCIP CCRP DTOS LADD ULOFT MANUAL PRE VIS BORE STRAFE");

	 const Submode CCIP("CCIP");
	 const Submode CCRP("CCRP");
	 const Submode DTOS("DTOS");
	 const Submode LADD("LADD");
	 const Submode ULOFT("ULOFT");
	 const Submode MANUAL("MANUAL");
	 const Submode PRE("PRE");
	 const Submode VIS("VIS");
	 const Submode BORE("BORE");
	 const Submode STRAFE("STRAFE");
}

} // namespace f16

} // namespace csp

