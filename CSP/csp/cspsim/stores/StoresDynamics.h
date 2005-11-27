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
 * @file StoresDynamics.h
 *
 **/


#ifndef __CSPSIM_STORESDYNAMICS_H__
#define __CSPSIM_STORESDYNAMICS_H__


#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/data/Matrix3.h>


/** Dynamical properties model for stores carried by a vehicle.
 *
 * The stores management system initializes an instance of this class by adding
 * various drag and mass sources when the stores configuration changes.
 * DynamicObject coordinates updates to this object and manages mass and
 * inertial changes, while FlightDynamics (or any other BaseDynamics class)
 * uses this instance to compute drag at each timestep.
 *
 * Computations performed at each timestep need to be fast, so the update process
 * should precompute as much information as possible (e.g., merge drag tables for
 * all stores to allow total drag and drag moment to be computed with a couple
 * lookups).  Currently drag is modeled as a linear function of dynamic pressure,
 * but more sophisticated models that depend on mach and angle of attack for
 * different stores types could easily be implemented.
 */
class StoresDynamics {
public:
	StoresDynamics() { reset(); }

	/** Remove all dynamic properties.  Call prior to calling addMassAndIneritia and addDrag for all stores.
	 */
	void reset() {
		mass = 0.0;
		drag = 0.0;
		center_of_mass = simdata::Vector3::ZERO;
		center_of_drag = simdata::Vector3::ZERO;
		inertia = simdata::Matrix3::IDENTITY;
		valid = true;
	}

	/** Compute aggregate dynamic properties.  Call after calling addMassAndInertia and addDrag for all stores.
	 */
	void compute() {
		if (mass > 0) center_of_mass /= mass;
		if (drag > 0) center_of_drag /= drag;
		valid = true;
	}

	/** Add mass and inertia for a single source.  The position is the center of mass position
	 *  of the store in body coordinates.  The unit_inertia field is the moment of inertia per
	 *  unit mass in body coordinates (must take the orientation of the store into account),
	 *  neglecting inertia due to the offset of the store relative to the aircraft origin.
	 */
	void addMassAndInertia(double mass_, simdata::Matrix3 const &unit_inertia, simdata::Vector3 const &position) {
		valid = false;
		mass += mass_;
		center_of_mass += mass_ * position;
		inertia += mass_ * (unit_inertia - makeInertiaOffset(position));
	}

	/** Add drag properties for a single source.  The position is the center of mass position
	 *  of the store in body coordintates.
	 */
	void addDrag(double coefficient, simdata::Vector3 const &position) {
		valid = false;
		drag += coefficient;
		center_of_drag += coefficient * position;
		// TODO reduce drag due to drafting.  if two stores are colinear along the y-axis, the
		// leading store reduces the drag of the trailing store.  obviously this cannot be
		// computed accurately in the simulation, but a useful approximation would be to use
		// only the larger coefficient.  determining colinearity within some tolerance, say
		// 20 cm, is a little tricky to do efficiently however.  probably RackData should provide
		// hints for colinear mount points.  at the very least the hardpoint index should be
		// passed to this function to restrict the search.
	}

	/** Compute the drag force and moment from all stores.
	 */
	void computeDragForceAndMoment(double qbar, double Cd, double alpha, simdata::Vector3 &force, simdata::Vector3 &moment) const {
		assert(valid);
		force = (qbar * drag * Cd) * simdata::Vector3(0.0, -cos(alpha), sin(alpha));
		moment = center_of_drag ^ force;
	}

	/** Compute m_ij = o_i * o_j - o^2
	 */
	static simdata::Matrix3 makeInertiaOffset(simdata::Vector3 const &offset) {
		double d = offset.length2();
		double xx = offset.x() * offset.x();
		double xy = offset.x() * offset.y();
		double xz = offset.x() * offset.z();
		double yy = offset.y() * offset.y();
		double yz = offset.y() * offset.z();
		double zz = offset.z() * offset.z();
		return simdata::Matrix3(xx - d, xy, xz, xy, yy - d, yz, xz, yz, zz - d);
	}

	double getMass() const { return mass; }
	double getDrag() const { return drag; }
	simdata::Vector3 const &getCenterOfMass() const { assert(valid); return center_of_mass; }
	simdata::Vector3 const &getCenterOfDrag() const { assert(valid); return center_of_drag; }
	simdata::Matrix3 const &getInertia() const { return inertia; }

private:
	double mass;
	double drag;
	simdata::Vector3 center_of_mass;
	simdata::Matrix3 inertia;
	simdata::Vector3 center_of_drag;
	bool valid;
};


#endif // __CSPSIM_STORESDYNAMICS_H__

