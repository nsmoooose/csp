/* SimDataCSP: Data Infrastructure for Simulations
 * Copyright (C) 2002 Mark Rose <tm2@stm.lbl.gov>
 * 
 * This file is part of SimDataCSP.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <SimData/Math.h>
#include <SimData/Vector3.h>


NAMESPACE_SIMDATA


//
// Calculates the angle between two vectors (with orientation)
//
double angleBetweenTwoVectors(const Vector3 & v1, const Vector3 & v2)
{
	double l1 = v1.Length(), l = l1 * v2.Length();

	if (l == 0.0) return 0.0;

	double cosa = Dot(v1, v2) / l;
	Vector3 crossVec = v1^v2;

	double orientation = Dot(Vector3(0,0,1),crossVec);
	if (orientation == 0.0)
		orientation = Dot(Vector3(1,0,0), crossVec);
	if (orientation == 0.0)
		orientation = - Dot(Vector3(0,1,0), crossVec);
	// if the orientation is negative then the angle is negative.
	if (orientation > 0.0) {
		return acos(cosa);
	} else {
		return -acos(cosa);
	}
}


NAMESPACE_END // namespace simdata

