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
 * @file PhysicsModel.h
 *
 **/


#ifndef __CSPSIM_PHYSICSMODEL_H__
#define __CSPSIM_PHYSICSMODEL_H__

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Ref.h>

#include <csp/csplib/data/Matrix3.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/data/Quat.h>

#include <csp/cspsim/System.h>
#include <csp/cspsim/DynamicalSystem.h>

CSP_NAMESPACE


class BaseDynamics;


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
class PhysicsModel: public System, protected DynamicalSystem {
public:
	PhysicsModel(size_type dimension);
	virtual ~PhysicsModel();

	/** Integrate the equations of motion over the specified time
	 *  interval.
	 */
	virtual void doSimStep(double dt) = 0;

	/** Add a dynamics simulation component.  Each BaseDynamics class computes
	 *  a part of the total force and moment that acts on the object.
	 */
	void addDynamics(Ref<BaseDynamics>);

protected:
	// should it be a priority queue instead?
	std::vector< Ref<BaseDynamics> > m_Dynamics;

	double const m_Damping;

	virtual void postCreate();

	/** Extract kinematic variables from the vector field variable.
	 */
	virtual void YToBody(numeric::Vectord const &y);

	/** Assemble the vector field variable from individual kinematic variables.
	 */
	virtual numeric::Vectord const &bodyToY(Vector3 const &p, Vector3 const &v, Vector3 const &w, Quat const &q);

	/** Convert from local (global) to body coordinates.
	 */
	Vector3 localToBody(const Vector3 & vec);

	/** Convert from body to local (global) coordinates.
	 */
	Vector3 bodyToLocal(const Vector3 & vec);

	/** Convert kinematic variables from body to local (global) coordinates.
	 */
	void physicsBodyToLocal();

	// internally: X = right, Y = nose, Z = up
	// externally: X = nose, Y = right, Z = down (for XML input)
	
	double m_CollisionRadius;

	Quat m_Attitude;                   // attitude in earth coordinates

	Vector3 m_ForcesBody;              // total force in body coordinates
	Vector3 m_MomentsBody;             // (L,M,N) total moment (torque) in body coordinates

	Vector3 m_PositionBody;            // cm position after integration step in body coordinates
	Vector3 m_PositionLocal;           // cm position in earth coordinates
	Vector3 m_VelocityLocal;           // velocity in earth coordinates
	Vector3 m_VelocityBody;            // (U,V,W) velocity in body coordinates
	Vector3 m_AngularVelocityLocal;    // angular velocity in earth coordinates
	Vector3 m_AngularVelocityBody;     // (P,Q,R) angular velocity in body coordinates
	Vector3 m_AngularAccelBody;        // (Pdot, Qdot, Rdot)
	Vector3 m_LinearAccelBody;         // (Udot, Vdot, Wdot)
	Vector3 m_CenterOfMassOffsetLocal; // offset from the model position to the center of mass in local coordinates

	double m_Gravity;
	Vector3 m_WeightLocal, m_WeightBody;

	DataChannel<Vector3>::RefT b_Position;
	DataChannel<Vector3>::RefT b_Velocity;
	DataChannel<Vector3>::RefT b_AngularVelocity;
	DataChannel<Vector3>::RefT b_AngularVelocityBody;
	DataChannel<Vector3>::RefT b_AccelerationBody;
	DataChannel<Quat>::RefT b_Attitude;

	DataChannel<double>::CRefT b_Mass;
	DataChannel<Matrix3>::CRefT b_Inertia;
	DataChannel<Matrix3>::CRefT b_InertiaInverse;
	DataChannel<Vector3>::CRefT b_CenterOfMassOffset;

	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

};

CSP_NAMESPACE_END

#endif  // __PHYSICSMODEL_H__

