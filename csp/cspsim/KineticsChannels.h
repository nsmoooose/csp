#pragma once
// Combat Simulator Project
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

#include <csp/cspsim/Export.h>

namespace csp {
namespace bus {

struct Kinetics {
	static CSPSIM_EXPORT const char *NearGround;
	static CSPSIM_EXPORT const char *ModelPosition;
	static CSPSIM_EXPORT const char *Position;  // center of mass position
	static CSPSIM_EXPORT const char *Velocity;
	static CSPSIM_EXPORT const char *AngularVelocity;
	static CSPSIM_EXPORT const char *AngularVelocityBody;
	static CSPSIM_EXPORT const char *AccelerationBody;
	static CSPSIM_EXPORT const char *Attitude;
	static CSPSIM_EXPORT const char *Mass;
	static CSPSIM_EXPORT const char *Inertia;
	static CSPSIM_EXPORT const char *InertiaInverse;
	static CSPSIM_EXPORT const char *GroundN;
	static CSPSIM_EXPORT const char *GroundZ;
	static CSPSIM_EXPORT const char *Heading;
	static CSPSIM_EXPORT const char *Roll;
	static CSPSIM_EXPORT const char *Pitch;
	static CSPSIM_EXPORT const char *CenterOfMassOffset;  // body coordinates
	static CSPSIM_EXPORT const char *StoresDynamics;
};

} // namespace bus
} // namespace csp
