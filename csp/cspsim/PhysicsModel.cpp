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
 * @file PhysicsModel.cpp
 *
 * Integration of the equations of motion is performed over the state
 * vector y[13], which contains the following variables:
 *
 * Vector3 x (y[0:3])
 *   - displacement of the center of mass relative to its position at
 *     t = 0, in earth coordinates.
 *
 * Vector3 v (y[3:6])
 *   - velocity of the center of mass, in earth coordinates.
 *
 * Vector3 L (y[6:9])
 *   - angular momentum with respect to the center of mass, in earth
 *     coordinates.
 *
 * Quat q (y[9:13])
 *   - orientation of the body frame, such that q.rotate(body_axis) gives
 *     the orientation of body_axis in earth coordinates.  the body frame
 *     uses a right handed coordinate system with X=right, Y=forward, Z=up.
 *     so, for example, q.rotate(Vector3(0, 1, 0)) is the forward direction
 *     in earth coordinates.
 *
 * The time derivatives of these variables, computed by PhysicsModel::f(),
 * are:
 *
 *   x' = v
 *   v' = F/m
 *   L' = N
 *   q' = 1/2 q w_body = 1/2 q (I_body_inverse * q.invrotate(L))
 *
 * where F is the total force in earth coordinates, m is the mass, N is
 * the total torque about the center of mass, w_body is the angular velocity
 * in body coordinates, and I_body_inverse is the inverse of the inertia
 * tensor in body coordinates.  The mass and inertia tensor (in body
 * coordinates) are approximated as constant for the duration of integration,
 * which is typically on the order of 20 milliseconds.
 **/

#include <csp/cspsim/BaseDynamics.h>
#include <csp/cspsim/PhysicsModel.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/csplib/numeric/NumericalMethod.h>
#include <csp/csplib/data/ObjectInterface.h>

#include <iostream>

CSP_NAMESPACE

using bus::Kinetics;

CSP_XML_BEGIN(PhysicsModel)
CSP_XML_END


PhysicsModel::PhysicsModel():
	VectorField(cDimension),
	m_Attitude(Quat::IDENTITY),
	m_ForcesBody(Vector3::ZERO),
	m_MomentsBody(Vector3::ZERO),
	m_LinearAccelBody(Vector3::ZERO),
	m_NumericalMethod(NULL),
	m_StateVector0(cDimension),
	m_StateVector(cDimension),
	m_NeedsImpulse(false),
	m_AddGravity(true),
	m_AddPseudoForces(true)
{
	// TODO externalize numerical method and options, and decide
	// how to handle LOD.
	m_NumericalMethod = new numeric::RKCK_VS_VO;
	m_NumericalMethod->setVectorField(this);
	m_NumericalMethod->setPrecision(1e-3);
	m_NumericalMethod->setSteps(50);
}

PhysicsModel::~PhysicsModel() {
	delete m_NumericalMethod;
}

void PhysicsModel::addDynamics(Ref<BaseDynamics> dynamic) {
	m_Dynamics.push_back(dynamic);
	dynamic->bindKinematics(m_Position, m_VelocityBody, m_AngularVelocityBody, m_Attitude);
}

void PhysicsModel::enableGravity(bool enabled) {
	m_AddGravity = enabled;
}

void PhysicsModel::enablePseudoForces(bool enabled) {
	m_AddPseudoForces = enabled;
}

void PhysicsModel::postCreate() {
	System::postCreate();
	int n = getNumChildren();
	for (int i = 0; i < n; i++) {
		Ref<BaseDynamics> dynamics;
		dynamics = getChild(i);
		if (dynamics.valid()) {
			addDynamics(dynamics);
			CSPLOG(DEBUG, OBJECT) << "PhysicsModel::postCreate() adding dynamics: " << dynamics->getClassName();
		}
	}
}

Vector3 PhysicsModel::toBody(Vector3 const &vec) {
	return m_Attitude.invrotate(vec);
}

Vector3 PhysicsModel::fromBody(Vector3 const &vec) {
	return m_Attitude.rotate(vec);
}

bool PhysicsModel::flow(numeric::Vectord const& y0, numeric::Vectord &y, double t0, double dt) {
	if (!m_NumericalMethod->enhancedSolve(y0, y, t0, dt)) {
		double t_end = t0 + dt;
		while (t0 < t_end) {
			double t_next = std::min(t0 + 0.001, t_end);
			m_NumericalMethod->quickSolve(y0, y, t0, t_next);
			t0 = t_next;
		}
		return false;
	}
	return true;
}

void PhysicsModel::setStateVector(Vector3 const& position_local, Vector3 const& velocity, Vector3 const& angular_momentum, Quat const& attitude, numeric::Vectord &y) {
	y[0] = position_local.x();
	y[1] = position_local.y();
	y[2] = position_local.z();
	y[3] = velocity.x();
	y[4] = velocity.y();
	y[5] = velocity.z();
	y[6] = angular_momentum.x();
	y[7] = angular_momentum.y();
	y[8] = angular_momentum.z();
	y[9] = attitude.w();
	y[10] = attitude.x();
	y[11] = attitude.y();
	y[12] = attitude.z();
}

void PhysicsModel::initializeStateVector(numeric::Vectord &y) {
	Vector3 angular_momentum = fromBody(b_Inertia->value() * b_AngularVelocityBody->value());
	setStateVector(Vector3::ZERO, b_Velocity->value(), angular_momentum, m_Attitude, y);
}

void PhysicsModel::updateFromStateVector(numeric::Vectord const &y) {
	m_PositionLocal.set(y[0], y[1], y[2]);  // earth coordinates, relative to initial position
	m_Position = m_PositionLocal + b_Position->value();

	m_Velocity.set(y[3], y[4], y[5]);  // earth coordinates
	m_VelocityBody = toBody(m_Velocity);

	m_Attitude.set(y[10], y[11], y[12], y[9]);
	m_Attitude.normalize();

	Vector3 angular_momentum(y[6], y[7], y[8]);  // earth coordinates
	m_AngularVelocityBody = b_InertiaInverse->value() * toBody(angular_momentum);
	m_AngularVelocity = fromBody(m_AngularVelocityBody);
}

void PhysicsModel::f(double x, numeric::Vectord const &y, numeric::Vectord& dydt) {
	updateFromStateVector(y);

	m_ForcesBody = Vector3::ZERO;
	m_MomentsBody = Vector3::ZERO;

	std::vector< Ref<BaseDynamics> >::iterator bd = m_Dynamics.begin();
	std::vector< Ref<BaseDynamics> >::const_iterator bdEnd = m_Dynamics.end();
	for (; bd != bdEnd; ++bd) {
		(*bd)->computeForceAndMoment(x);
		Vector3 force_body = (*bd)->getForce();
		Vector3 moment_body = (*bd)->getMoment();
		if (force_body.valid() && moment_body.valid()) {
			m_ForcesBody += force_body;
			m_MomentsBody += moment_body;
		} else {
			CSPLOG(ERROR, OBJECT) << "PhysicsModel: overflow in dynamics class '" << (*bd)->getClassName() << "'";
			std::cout << "PhysicsModel: overflow in dynamics class '" << (*bd)->getClassName() << "'\n";
			std::cout << "(f,m) = (" << force_body << ", " << moment_body <<")\n";
		}
		if ((*bd)->needsImpulse()) m_NeedsImpulse = true;
	}

	Vector3 &dxdt = m_Velocity;
	Vector3 dvdt = fromBody(m_ForcesBody) / b_Mass->value() + m_OtherAccelerations;
	Vector3 dLdt = fromBody(m_MomentsBody);
	// quaternion derivative: q' = 0.5 * q * w_body
	Quat dqdt = 0.5 * m_Attitude * m_AngularVelocityBody;

	setStateVector(dxdt, dvdt, dLdt, dqdt, dydt);
}

void PhysicsModel::doSimStep(double dt) {
	if (dt <= 0.0) return;

	std::for_each(m_Dynamics.begin(), m_Dynamics.end(), InitializeSimulationStep(dt));

	// check one bus channel to ensure that importChannels has been called.
	assert(b_Attitude.valid());

	// set m_Attitude first so that toBody and fromBody work!
	m_Attitude = b_Attitude->value();

	initializeStateVector(m_StateVector0);
	updateFromStateVector(m_StateVector0);

	m_OtherAccelerations = Vector3::ZERO;
	if (m_AddGravity) {
		// TODO: adjust if ECEF coordinates are used.
		double g = -9.80 + m_Position.z() * (2.0 * 9.80 / 6.37e+6);
		m_OtherAccelerations.z() += g;
	}
	if (m_AddPseudoForces) {
		// TODO: need w_earth, which isn't readily available since the
		// coordinate system depends on external definitions (lat/lng
		// of the center of the theater).  this would be trivial if we
		// adopted ECEF coordinates!
		//m_OtherAccelerations += -2.0 * w_earth ^ m_Velocity;
	}

	std::for_each(m_Dynamics.begin(), m_Dynamics.end(), PreSimulationStep(dt));

	m_NeedsImpulse = false;

	// solution
	flow(m_StateVector0, m_StateVector, 0.0, dt);

	updateFromStateVector(m_StateVector);

	// compute derivatives at the endpoint, since there's no guarantee that the
	// solver will evaluate f(dt) last.  although any point within the interval
	// would work, using a consistent time relative to the step boundaries avoids
	// jitter.  the derivatives are only needed to evaluate b_AccelerationBody
	// below, although we may want to export angular acceleration as well at some
	// point.
	//
	// m_StateVector0 is used as a dummy variable here, since we don't use the
	// output of f() directly and there's no point in allocating an extra Vectord
	// for this purpose.
	f(dt, m_StateVector, m_StateVector0);

	if (m_NeedsImpulse) {
		double scale = 1.0 - std::min(1.0, dt * 5.0);
		m_Velocity *= scale;
	}

	b_Position->value() = m_Position;
	b_Velocity->value() = m_Velocity;
	b_Attitude->value() = m_Attitude;
	b_AngularVelocity->value() = m_AngularVelocity;
	b_AngularVelocityBody->value() = m_AngularVelocityBody;
	b_AccelerationBody->value() = m_ForcesBody / b_Mass->value() + toBody(m_OtherAccelerations);

	std::for_each(m_Dynamics.begin(), m_Dynamics.end(), PostSimulationStep(dt));
}

void PhysicsModel::registerChannels(Bus *) {
}

void PhysicsModel::importChannels(Bus *bus) {
	assert(bus != 0);
	assert(!b_Position);
	b_Position = bus->getSharedChannel(Kinetics::Position, true, true);
	b_Velocity = bus->getSharedChannel(Kinetics::Velocity, true, true);
	b_AngularVelocity = bus->getSharedChannel(Kinetics::AngularVelocity, true, true);
	b_AngularVelocityBody = bus->getSharedChannel(Kinetics::AngularVelocityBody, true, true);
	b_Attitude = bus->getSharedChannel(Kinetics::Attitude, true, true);
	b_AccelerationBody = bus->getSharedChannel(Kinetics::AccelerationBody, true, true);
	b_Mass = bus->getChannel(Kinetics::Mass);
	b_Inertia = bus->getChannel(Kinetics::Inertia);
	b_InertiaInverse = bus->getChannel(Kinetics::InertiaInverse);
}


CSP_NAMESPACE_END

