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
 * @file PhysicsModel.h
 *
 **/


#ifndef __PHYSICSMODEL_H__
#define __PHYSICSMODEL_H__

#include <SimData/Matrix3.h>
#include <SimData/Vector3.h>
#include <SimData/Quat.h>
#include <SimData/Ref.h>

#include "DynamicalSystem.h"

class BaseDynamics;
class GroundCollisionDynamics;


/**
 * class PhysicsModel - Base class for implementing classical mechanics 
 * models. 
 *
 * PhysicsModel classes contain multiple BaseDynamics instances, each
 * contributing to the total force and moment that act on the object.
 * The PhysicsModel integrates the equations of motion to solve for
 * the position and velocity of the object as functions of time.
 *
 */
class PhysicsModel: public simdata::Referenced, protected DynamicalSystem {  
public:	
	PhysicsModel(unsigned short dimension);
	virtual ~PhysicsModel();

	/**
	 * Bind the model to the kinematic parameters of an object.
	 */
	void bindObject(simdata::Vector3 &position, 
                        simdata::Vector3 &velocity, 
                        simdata::Vector3 &angular_velocity, 
                        simdata::Quat &orientation);

	/**
	 * Integrate the equations of motion over the specified time
	 * interval.
	 */
	virtual void doSimStep(double dt) = 0;

	/**
	 * Specify the inertial properties of the object being simulated.
	 */
	void setInertia(double mass, simdata::Matrix3 const &I);

	/**
	 * Specify the bounding radius of the object being simulated
	 * (used for collision tests).
	 */
	void setBoundingRadius(double radius);

	/**
	 * Specify the altitude of the ground directly beneath the object.
	 * (typically used for ground-vehicle collisions).
	 */
	void setGroundZ(double groundz) { m_GroundZ = groundz; }

	/**
	 * Specify the normal (unit) vector of the ground directly beneath
	 * the object.
	 */
	void setGroundN(simdata::Vector3 const &groundn) { 
		// FIXME: just testing to see if smoothing normal transitions
		// will reduce bad physics at terrain triangle boundaries.  if
		// it is promising, we need to tie the time constant to the frame
		// rate.
		// first test: seems to help with convex transitions.  to deal
		// with concave sections probably requires multiple elevation
		// tests.
		m_GroundN = 0.98 * m_GroundN + 0.02 * groundn; 
	}

	/**
	 * Add a dynamics simulation component.  Each BaseDynamics class computes
	 * a part of the total force and moment that acts on the object.
	 */
	void addDynamics(BaseDynamics *);

protected:
	// should it be a priority queue instead?
	std::vector<BaseDynamics*> m_Dynamics;
	GroundCollisionDynamics *m_GroundCollisionDynamics;
	double const m_Damping;

	/**
	 * Extract kinematic variables from the vector field variable.
	 */
	void YToBody(std::vector<double> const &y);

	/**
	 * Assemble the vector field variable from individual kinematic variables.
	 */
	std::vector<double> const &bodyToY(simdata::Vector3 const &p,
	                                   simdata::Vector3 const &v,
	                                   simdata::Vector3 const &w,
	                                   simdata::Quat const &q);

	/**
	 * Convert from local (global) to body coordinates.
	 */
	simdata::Vector3 localToBody(const simdata::Vector3 & vec);

	/**
	 * Convert from body to local (global) coordinates.
	 */
	simdata::Vector3 bodyToLocal(const simdata::Vector3 & vec);

	/**
	 * Convert kinematic variables from body to local (global) coordinates.
	 */
	void physicsBodyToLocal();

	/**
	 * Check the object bounding sphere against the ground and set
	 * related flags and parameters.
	 */
	virtual void updateNearGround(double dt);

	/**
	 * Update the aerodynamic parameters, such as air density
	 * and wind speed, for the current object position.
	 */
	virtual void updateAeroParameters(double dt);
	
	// internally: X = right, Y = nose, Z = up
	// externally: X = nose, Y = right, Z = down (for XML input)
	
	simdata::Quat m_qOrientation;     // orientation in earth coordinates

	simdata::Vector3 m_ForcesBody;          // total force in body coordinates
	simdata::Vector3 m_MomentsBody;         // (L,M,N) total moment (torque) in body coordinates

	simdata::Vector3 m_PositionBody;        // position after integration step in body coordinates
	simdata::Vector3 m_PositionLocal;       // position in earth coordinates
	simdata::Vector3 m_VelocityLocal;       // velocity in earth coordinates
	simdata::Vector3 m_VelocityBody;        // (U,V,W) velocity in body coordinates
	simdata::Vector3 m_AngularVelocityLocal;// angular velocity in earth coordinates
	simdata::Vector3 m_AngularVelocityBody; // (P,Q,R) angular velocity in body coordinates
	simdata::Vector3 m_AngularAccelBody;    // (Pdot, Qdot, Rdot)
	simdata::Vector3 m_LinearAccelBody;     // (Udot, Vdot, Wdot)

	// kinematic parameter pointers to dynamic object
	simdata::Vector3 *m_Position;
	simdata::Vector3 *m_Velocity;
	simdata::Vector3 *m_AngularVelocity;
	simdata::Quat *m_Orientation;

	// View following from fuel consumption ...
	double m_Mass;
	double m_MassInverse;
	simdata::Matrix3 m_Inertia;        // mass moment of inertia in standard coordinates (constant)
	simdata::Matrix3 m_InertiaInverse; // inverse of moment of inertia matrix (constant)
	double m_Gravity;
	simdata::Vector3 m_WeightLocal, m_WeightBody;

	// ground collision related parameters
	bool m_NearGround;
	simdata::Vector3 m_NormalGround;
	double m_Height;
	double m_Radius;                                       // bounding sphere radius
	double m_GroundZ;                                      // ground position
	simdata::Vector3 m_GroundN;                            // ground normal

	// aero parameters
	double m_qBar;
	simdata::Vector3 m_WindBody;
	double m_Distance;  // for tracking motion through turbulence fields
};

#endif  // __PHYSICSMODEL_H__

