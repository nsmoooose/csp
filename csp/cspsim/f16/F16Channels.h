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
 * @file F16Channels.h
 *
 **/


#ifndef __CSPSIM_F16_F16CHANNELS_H__
#define __CSPSIM_F16_F16CHANNELS_H__

#include <csp/csplib/util/Namespace.h>

namespace csp {

namespace bus {

struct F16 {
	static const char *AirbrakeLimit;
	static const char *AltFlaps;
	static const char *CatIII;
	static const char *ManualPitchOverride;
	static const char *ManualPitchOverrideActive;
	static const char *GearHandleUp;
	static const char *INSAttitude;
	static const char *StandbyGains;
	static const char *TakeoffLandingGains;
	static const char *TrailingEdgeFlapExtension;
	static const char *WheelSpin;
};

} // namespace bus

} // namespace csp

#endif // __CSPSIM_F16CHANNELS_H__

