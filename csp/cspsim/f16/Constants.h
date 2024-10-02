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


/**
 * @file f16/Constants.h
 *
 **/

#include <csp/csplib/data/Enum.h>
#include <csp/cspsim/f16/MasterModes.h>

namespace csp {

namespace f16 {

namespace AirToGround {
	extern const Enumeration Submodes;
	typedef Enum<Submodes> Submode;

	extern const Submode CCIP;
	extern const Submode CCRP;
	extern const Submode DTOS;
	extern const Submode LADD;
	extern const Submode ULOFT;
	extern const Submode MANUAL;
	extern const Submode PRE;
	extern const Submode VIS;
	extern const Submode BORE;
	extern const Submode STRAFE;
}

} // namespace f16

} // namespace csp
