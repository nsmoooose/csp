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
 * @file PhysicModel.h
 *
 **/


#ifndef __PHYSICMODEL_H__
#define __PHYSICMODEL_H__

#include <SimData/Matrix3.h>
#include <SimData/Vector3.h>
#include <SimData/Quaternion.h>
#include <SimData/Ref.h>

#include "DynamicalSystem.h"

class BaseDynamics;
class GroundCollisionDynamics;

/**
 * class PhysicModel - base class: classical mechanic model implementation.
 *
 */
class PhysicModel: public simdata::Referenced, protected DynamicalSystem {  
public:	
	PhysicModel(unsigned short dimension);
	virtual ~PhysicModel();
	void PhysicModel::bindObject(simdata::Vector3 &position, simdata::Vector3 &velocity, simdata::Vector3 &angular_velocity,
							     simdata::Quaternion &orientation);
	virtual void doSimStep(double dt) = 0;
	void setInertia(double mass, simdata::Matrix3 const &I);
	void setBoundingRadius(double radius);
	void addDynamics(BaseDynamics *);
protected:
	// should it be a priority queue instead?
	std::vector<BaseDynamics*> m_Dynamics;
	GroundCollisionDynamics *m_GroundCollisionDynamics;
	double const m_Damping;

	void YToBody(std::vector<double> const &y);
	std::vector<double> const &bodyToY(simdata::Vector3 const &p,simdata::Vector3 const &v,simdata::Vector3 const &w,simdata::Quaternion const &q);
	simdata::Vector3 localToBody(const simdata::Vector3 & vec);
	simdata::Vector3 bodyToLocal(const simdata::Vector3 & vec);
	void physicsBodyToLocal();
	void updateNearGround();
	
	// internally: X = right, Y = nose, Z = up
	// externally: X = nose, Y = right, Z = down (for XML input)
	
	simdata::Quaternion m_qOrientation;     // orientation in earth coordinates

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

	// physic parameter pointers to dynamic object
	simdata::Vector3 *m_Position;
	simdata::Vector3 *m_Velocity;
	simdata::Vector3 *m_AngularVelocity;
	simdata::Quaternion *m_Orientation;

	// View following from fuel consumption ...
	double m_Mass;
	double m_MassInverse;
	simdata::Matrix3 m_Inertia;        // mass moment of inertia in standard coordinates (constant)
	simdata::Matrix3 m_InertiaInverse; // inverse of moment of inertia matrix (constant)
	double m_Gravity;
	simdata::Vector3 m_WeightLocal, m_WeightBody;

	bool m_NearGround;
	simdata::Vector3 m_NormalGround;
	double m_Height;
	double m_Radius;                                       // bounding sphere radius
	double m_GroundZ;                                      // ground position
	simdata::Vector3 m_GroundN;                            // ground normal
};

#endif  // __PHYSICMODEL_H__

