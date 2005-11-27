// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2003, 2005 The Combat Simulator Project
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
 * @file KineticsChannels.h
 *
 **/


#ifndef __CSPSIM_KINETICS_CHANNELS_H__
#define __CSPSIM_KINETICS_CHANNELS_H__

#include <csp/csplib/util/Namespace.h>

CSP_NAMESPACE

namespace bus {

struct Kinetics {
	static const char *NearGround;
	static const char *ModelPosition;
	static const char *Position;  // center of mass position
	static const char *Velocity;
	static const char *AngularVelocity;
	static const char *AngularVelocityBody;
	static const char *AccelerationBody;
	static const char *Attitude;
	static const char *Mass;
	static const char *Inertia;
	static const char *InertiaInverse;
	static const char *GroundN;
	static const char *GroundZ;
	static const char *Heading;
	static const char *Roll;
	static const char *Pitch;
	static const char *CenterOfMassOffset;  // body coordinates
	static const char *StoresDynamics;
};

} // namespace bus

CSP_NAMESPACE_END

#endif // __CSPSIM_KINETICS_CHANNELS_H__

