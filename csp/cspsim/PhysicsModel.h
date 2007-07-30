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

#include <csp/csplib/data/Matrix3.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/data/Quat.h>
#include <csp/csplib/numeric/VectorField.h>
#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Ref.h>

#include <csp/cspsim/System.h>

CSP_NAMESPACE


class BaseDynamics;
namespace numeric { class NumericalMethod; }


/**
 * PhysicsModel is a system that implements rigid body dynamics of
 * an object.  It may contain multiple BaseDynamics instances, each
 * contributing to the total force and moment that act on the object.
 * The PhysicsModel integrates the equations of motion to solve for
 * the position, velocity, and attitude of the object as functions
 * of time.
 */
class PhysicsModel: public System, protected numeric::VectorField {
public:
	CSP_DECLARE_OBJECT(PhysicsModel)

	PhysicsModel();
	virtual ~PhysicsModel();

	/** Integrate the equations of motion over the specified time
	 *  interval.
	 */
	virtual void doSimStep(double dt);

	/** Add a dynamics simulation component.  Each BaseDynamics class computes
	 *  a part of the total force and moment that acts on the object.
	 */
	void addDynamics(Ref<BaseDynamics>);

	/** By default, gravity is automatically included in the force calculations.
	 *  This method can be used to disable gravity, primarily for testing.
	 */
	void enableGravity(bool enabled);

	/** By default, the Coriolis and centrifugal psuedoforces are included in the
	 *  dynamical calculations.  This method can be used to disable these forces,
	 *  primarily for testing.
	 */
	void enablePseudoForces(bool enabled);

protected:
	virtual void postCreate();

	/** Solve the equations of motion from time t0 to t0 + dt, given initial
	 *  conditions y0.  Returns false if the enhances solver failed and the
	 *  less accurate quick solver was used.
	 */
	virtual bool flow(numeric::Vectord const& y0, numeric::Vectord &y, double t0, double dt);

	/** Convert from earth to body coordinates.
	 */
	Vector3 toBody(const Vector3& vec);

	/** Convert from body to earth coordinates.
	 */
	Vector3 fromBody(const Vector3& vec);

	/** Compute the time derivative of the state vector y at time t relative
	 *  to the start of the timestep.
	 */
	virtual void f(double t, numeric::Vectord const& y, numeric::Vectord& dydt);

	/** Update the internal state variables from the specified state vector.
	 */
	virtual void updateFromStateVector(numeric::Vectord const &y);

	/** Assemble the state vector from internal state variables.
	 */
	virtual void initializeStateVector(numeric::Vectord &y);

	/** Load kinetic variables into a state vector.
	 */
	void setStateVector(Vector3 const& position_local, Vector3 const& velocity, Vector3 const& angular_momentum, Quat const& attitude, numeric::Vectord &y);

	/** Get the numerical method used to integrate the equations of motion.
	 */
	numeric::NumericalMethod* method() { return m_NumericalMethod; }

	/** Implement System interface to connect to the kinematic variables of
	 *  the object via bus channels.
	 */
	virtual void registerChannels(Bus*);
	virtual void importChannels(Bus*);

	std::vector< Ref<BaseDynamics> > m_Dynamics;

	Quat m_Attitude;                   // attitude in earth coordinates

	Vector3 m_ForcesBody;              // total force in body coordinates
	Vector3 m_MomentsBody;             // (L,M,N) total moment (torque) in body coordinates

	Vector3 m_Position;                // cm position in earth coordinates
	Vector3 m_PositionLocal;           // cm position in earth coordinates relative to the position at the start of the timestep
	Vector3 m_Velocity;                // velocity in earth coordinates
	Vector3 m_VelocityBody;            // (U,V,W) velocity in body coordinates
	Vector3 m_AngularVelocity;         // angular velocity in earth coordinates
	Vector3 m_AngularVelocityBody;     // (P,Q,R) angular velocity in body coordinates
	Vector3 m_AngularAccelBody;        // (Pdot, Qdot, Rdot)
	Vector3 m_LinearAccelBody;         // (Udot, Vdot, Wdot)

	// Other accelerations, in earth coordinates, computed at the start
	// of each timestep and approximated as constant for the duriation.
	// This includes gravity and pseudoforces related to the rotation
	// of the earth coordinate frame.
	Vector3 m_OtherAccelerations;

	// State variables updated at the end of each timestep.
	DataChannel<Vector3>::RefT b_Position;
	DataChannel<Vector3>::RefT b_Velocity;
	DataChannel<Vector3>::RefT b_AngularVelocity;
	DataChannel<Vector3>::RefT b_AngularVelocityBody;
	DataChannel<Vector3>::RefT b_AccelerationBody;
	DataChannel<Quat>::RefT b_Attitude;

	// Read only variables that are approximated as constant during
	// each timestep.
	DataChannel<double>::CRefT b_Mass;
	DataChannel<Matrix3>::CRefT b_Inertia;
	DataChannel<Matrix3>::CRefT b_InertiaInverse;

private:
	static const unsigned cDimension = 13;
	numeric::NumericalMethod* m_NumericalMethod;
	numeric::Vectord m_StateVector0;
	numeric::Vectord m_StateVector;
	bool m_NeedsImpulse;
	bool m_AddGravity;
	bool m_AddPseudoForces;
};

CSP_NAMESPACE_END

#endif  // __PHYSICSMODEL_H__

