// Combat Simulator Project
// Copyright (C) 2007 The Combat Simulator Project
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
 * @file DragProfile.h
 *
 **/


#ifndef __CSPSIM_DRAGPROFILE_H__
#define __CSPSIM_DRAGPROFILE_H__


#include <csp/csplib/data/LUT.h>
#include <csp/csplib/data/Object.h>

CSP_NAMESPACE

/** Aerodynamic drag profile for a vehicle store or projectile.  Currently individual
 *  stores do not use DragProfile directly.  When attached to an aircraft, all stores
 *  share a common DragProfile defined by the FlightDynamics instance.  Different
 *  store types define different reference areas that scale the common drag profile
 *  accordingly.
 *
 *  TODO replace with standard drag-index model.
 */
class DragProfile: public Object {
public:
	CSP_DECLARE_STATIC_OBJECT(DragProfile)

	/** Get the drag coefficient at the specified mach and angle of attack.
	 */
	double drag(double mach, double alpha) const {
		return m_Drag[static_cast<float>(mach)][static_cast<float>(alpha)];
	}

private:
	Table2 m_Drag;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_DRAGPROFILE_H__

