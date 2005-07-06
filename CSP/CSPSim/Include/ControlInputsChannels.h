// Combat Simulator Project
// Copyright (C) 2003-2005 The Combat Simulator Project
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
 * @file ControlInputsChannels.h
 *
 **/


#ifndef __CONTROLINPUTS_CHANNELS_H__
#define __CONTROLINPUTS_CHANNELS_H__


namespace bus {

struct ControlInputs {
	static const char *PitchInput;
	static const char *RollInput;
	static const char *RudderInput;
	static const char *LeftBrakeInput;
	static const char *RightBrakeInput;
	static const char *BrakeInput;
	static const char *SteeringInput;
	static const char *ThrottleInput;
	static const char *AirbrakeInput;
};

}

#endif // __CONTROLINPUTS_CHANNELS_H__

