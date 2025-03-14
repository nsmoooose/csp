#pragma once
// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file LandingGearChannels.h
 *
 **/

#include <string>

namespace csp {
namespace bus {

struct LandingGear {
	static const char *WOW;
	static const char *FullyExtended;
	static const char *FullyRetracted;
	static const char *GearExtendSelected;
	static const char *GearCommand;
	static std::string selectWOW(std::string const &gearname);
	static std::string selectFullyExtended(std::string const &gearname);
	static std::string selectFullyRetracted(std::string const &gearname);
	static std::string selectAntilockBrakingActive(std::string const &gearname);
	static std::string selectBrakeCommand(std::string const &gearname);
	static std::string selectSteeringCommand(std::string const &gearname);
};

} // namespace bus
} // namespace csp
