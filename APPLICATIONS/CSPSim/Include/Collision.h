// Combat Simulator Project - FlightSim Demo
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

#include <SimData/Matrix3.h>
#include <SimData/Ref.h>
#include <SimData/Vector3.h>

#include "BaseDynamics.h"



/**
 * Dynamics class for vehicle-ground collisions.
 *
 * Models collisions of vehicle "contact" points with the
 * ground.  These contact points are specified in the
 * vehicle ObjectModel and should be placed at the external
 * points of the model (e.g. wingtips, tail tip, nose, etc).
 * Collisions are treated as a damped harmonic oscillators, 
 * with additional sliding friction.  Hard impacts induce
 * rapid energy loss, and should be coupled with vehicle
 * damage modeling.
 */
class GroundCollisionDynamics: public simdata::Referenced, public BaseDynamics {
	std::vector<simdata::Vector3> m_Forces;
	std::vector<simdata::Vector3> m_Extension;
	std::vector<simdata::Vector3> const m_Contacts;
	double m_Mass;
	double m_ContactSpring;
	double m_SpringConstant, m_Friction, m_ImpactDamping;
	double const m_ImpactSpeedTolerance;
	bool m_NeedsImpulse, m_HasContact;
public:
	/** 
	 * Construct a new ground collision dynamics instance.
	 *
	 * @mass the (approximate) total vehicle mass, which is
	 *       used only to limit extremely violent collisions.
	 * @contacts the vehicle contact points
	 */
	GroundCollisionDynamics(double mass, std::vector<simdata::Vector3> const &contacts);

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
};

#endif // __COLLISION_H__

