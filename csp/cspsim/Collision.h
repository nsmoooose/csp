// Combat Simulator Project
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file Collision.h
 *
 **/

#ifndef __COLLISION_H__
#define __COLLISION_H__

#include <csp/cspsim/BaseDynamics.h>

#include <csp/csplib/data/Matrix3.h>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/data/Vector3.h>


namespace csp {

/**
 * Dynamics class for vehicle-ground collisions.
 *
 * Models collisions of vehicle "contact" points with the ground.  These
 * contact points are specified in the vehicle ObjectModel and should be placed
 * at the external points of the model (e.g. wingtips, tail tip, nose, etc).
 * Collisions are treated as a damped harmonic oscillators, with additional
 * sliding friction.  Hard impacts induce rapid energy loss, and should be
 * coupled with vehicle damage modeling.
 */
class GroundCollisionDynamics: public BaseDynamics {
	std::vector<Vector3> m_Forces;
	std::vector<Vector3> m_Extension;
	std::vector<Vector3> m_Contacts;
	double m_Mass;
	double m_ContactSpring;
	double m_SpringConstant, m_Friction, m_ImpactDamping;
	double const m_ImpactSpeedTolerance;
	bool m_NeedsImpulse, m_HasContact;

	DataChannel<double>::CRefT b_Mass;
	DataChannel<Matrix3>::CRefT b_InertiaInverse;
	DataChannel<Vector3>::CRefT b_CenterOfMassOffset;
	DataChannel<Vector3>::CRefT b_GroundN;
	DataChannel<double>::CRefT b_GroundZ;
	DataChannel<bool>::CRefT b_NearGround;

protected:
	virtual void importChannels(Bus *bus);
	virtual void registerChannels(Bus *bus);

public:
	CSP_DECLARE_OBJECT(GroundCollisionDynamics)

	/**
	 * Construct a new ground collision dynamics instance.
	 *
	 * @mass the (approximate) total vehicle mass, which is
	 *       used only to limit extremely violent collisions.
	 * @contacts the vehicle contact points
	 */
	GroundCollisionDynamics(); //double mass, std::vector<Vector3> const &contacts);

	/**
	 * Set the dynamic properties of the ground.
	 *
	 * @param spring_constant the stiffness of the ground under compresion
	 * @param friction the coefficent of sliding friction
	 * @param impact_damping the damping coefficient for the ground under compression
	 */
	void setGroundProperties(double spring_constant, double friction, double impact_damping);

	/**
	 * Compute the total contact force and moment.
	 */
	void computeForceAndMoment(double x);

	/**
	 * Returns true if a collision is sufficiently violent to require
	 * additional external damping (e.g. by directly attenuating the
	 * vehicle's velocity.
	 */
	bool needsImpulse() const { return m_NeedsImpulse; }

	/**
	 * Returns true if any contact points are touching the ground.
	 */
	bool hasContact() const { return m_HasContact; }

	/**
	 * Get the model position in global coordinates.  This is the physical
	 * location of the aircraft model origin, as opposed to m_Position which
	 * tracks the center of mass.
	 */
	Vector3 getModelPosition() const {
		return *m_Position - fromBody(b_CenterOfMassOffset->value());
	}
};

} // namespace csp

#endif // __COLLISION_H__

