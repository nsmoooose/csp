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
 * @file AircraftPhysicsModel.cpp
 *
 **/
#include "AircraftPhysicsModel.h"
#include "Atmosphere.h"
#include "BaseDynamics.h"
#include "Collision.h"
#include "CSPSim.h"
#include "NumericalMethod.h"
#include "Profile.h"

#include <algorithm>

using Vector::Vectord;

SIMDATA_REGISTER_INTERFACE(AircraftPhysicsModel)


AircraftPhysicsModel::AircraftPhysicsModel():
	PhysicsModel(13) {
		setNumericalMethod(new RKCK_VS_VO(this));
}

Vectord const &AircraftPhysicsModel::f(double x, Vectord &y) {
	// dot(p,v,w,q) = f(p,v,w,q)

	// bind(y,p,v,w,q)
	YToBody(y);
	m_Attitude.set(y[10],y[11],y[12],y[9]);
	double mag = m_Attitude.length();
	if (mag != 0.0) {
		m_Attitude /= mag;
	}
	m_PositionLocal = b_Position->value() + bodyToLocal(m_PositionBody);
	y[9]  = m_Attitude.w(); y[10] = m_Attitude.x(); y[11] = m_Attitude.y(); y[12] = m_Attitude.z();

	m_ForcesBody = m_MomentsBody = simdata::Vector3::ZERO;
	m_WeightBody = localToBody(m_WeightLocal);

/* XXX
	if (m_GroundCollisionDynamics.valid() && (m_NearGround || m_GroundCollisionDynamics->hasContact())) {
		m_GroundCollisionDynamics->computeForceAndMoment(x);
		if (m_GroundCollisionDynamics->hasContact()) {
			m_ForcesBody += m_GroundCollisionDynamics->getForce();
			m_MomentsBody += m_GroundCollisionDynamics->getMoment();
		}
	}
*/						

	std::vector< simdata::Ref<BaseDynamics> >::iterator bd = m_Dynamics.begin();
	std::vector< simdata::Ref<BaseDynamics> >::const_iterator bdEnd = m_Dynamics.end();
	for (;bd != bdEnd; ++bd) {
		(*bd)->computeForceAndMoment(x);
		simdata::Vector3 force = (*bd)->getForce();
		simdata::Vector3 moment = (*bd)->getMoment();
		if (force.valid() && moment.valid()) {
			m_ForcesBody += force;
			m_MomentsBody += moment;
		} else {
			CSP_LOG(OBJECT, ERROR, "AircraftPhysicsModel: overflow in dynamics class '" << (*bd)->getClassName() << "'");
			std::cout << "AircraftPhysicsModel: overflow in dynamics class '" << (*bd)->getClassName() << "'\n";
		}
		if ((*bd)->needsImpulse()) m_NeedsImpulse = true;
	}

	// Add weight; null moment
	m_ForcesBody += m_WeightBody;

	// linear acceleration body: calculate v' = F/m - w^v
	m_LinearAccelBody =  m_ForcesBody / b_Mass->value() - (m_AngularVelocityBody ^ m_VelocityBody);
	

	// angular acceleration body: calculate Iw' = M - w^Iw
	m_AngularAccelBody = b_InertiaInverse->value() *
	                     (m_MomentsBody - (m_AngularVelocityBody^(b_Inertia->value() * m_AngularVelocityBody)));
	
	// quaternion derivative and w in body coordinates: q' = 0.5 * q * w
	simdata::Quat qprim = 0.5 * m_Attitude * m_AngularVelocityBody;
	
	// p' = v
	m_dy[0] = y[3]; m_dy[1] = y[4]; m_dy[2] = y[5];
	// v'
	m_dy[3] = m_LinearAccelBody.x(); m_dy[4] = m_LinearAccelBody.y(); m_dy[5] = m_LinearAccelBody.z();
	// w'
	m_dy[6] = m_AngularAccelBody.x(); m_dy[7] = m_AngularAccelBody.y(); m_dy[8] = m_AngularAccelBody.z();
	// q'
	m_dy[9]  = qprim.w(); m_dy[10] = qprim.x(); m_dy[11] = qprim.y(); m_dy[12] = qprim.z();
	
	return m_dy;
}

void AircraftPhysicsModel::doSimStep(double dt) {	

	if (dt == 0.0) dt = 0.017;
	//unsigned short n = std::min<unsigned short>(6,static_cast<unsigned short>(180 * dt)) + 1;
	unsigned short n = 15*std::min<unsigned short>(1,static_cast<unsigned short>(210*dt)) + 1;
	double dtlocal = dt/n;
	std::for_each(m_Dynamics.begin(),m_Dynamics.end(),InitializeSimulationStep(dtlocal));

	Atmosphere const *atmosphere = CSPSim::theSim->getAtmosphere();
	double gravity;
	if (atmosphere) {
		gravity = atmosphere->getGravity(m_PositionLocal.z());
	} else {
		gravity = 9.806;
	}

	assert(b_Mass.valid());
	assert(b_Position.valid());
	assert(b_Velocity.valid());
	assert(b_AngularVelocity.valid());
	assert(b_Attitude.valid());
	m_WeightLocal = - b_Mass->value() * gravity * simdata::Vector3::ZAXIS;
	m_PositionLocal	= b_Position->value();
	m_VelocityLocal	= b_Velocity->value();
	m_AngularVelocityLocal = b_AngularVelocity->value();
	m_Attitude = b_Attitude->value();

	///XXX updateNearGround(dt);
	///XXX updateAeroParameters(dt);

	for (unsigned short i = 0; i<n; ++i) {
		
		m_VelocityBody = localToBody(m_VelocityLocal);
		
		m_AngularVelocityBody = localToBody(m_AngularVelocityLocal);

		Vectord y0 = bodyToY(simdata::Vector3::ZERO,m_VelocityBody,m_AngularVelocityBody,m_Attitude);

		std::for_each(m_Dynamics.begin(),m_Dynamics.end(),PreSimulationStep(dtlocal));

		m_NeedsImpulse = false;

		// solution
		Vectord y = flow(y0, 0.0, dtlocal);

		// update all variables
		// Caution: never permute the following lines
		// solution to body data members
		YToBody(y);


/* XXX
		// correct an eventual underground position
		if (m_GroundCollisionDynamics.valid() && m_GroundCollisionDynamics->needsImpulse()) {
			double scale = 1.0 - std::min(1.0, dtlocal * 100.0);
			m_VelocityBody *= scale;
		}
*/
		if (m_NeedsImpulse) {
			double scale = 1.0 - std::min(1.0, dtlocal * 100.0);
			m_VelocityBody *= scale;
		}

		std::for_each(m_Dynamics.begin(),m_Dynamics.end(),PostSimulationStep(dtlocal));

		// convert position, linear velocity and angular velocity from body to local coordinates
		physicsBodyToLocal();

		// update attitude and normalize the quaternion
		m_Attitude.set(y[10],y[11],y[12],y[9]);
		double mag = m_Attitude.length();
		if (mag	!= 0.0)
			m_Attitude /= mag;
		
		b_Position->value() = m_PositionLocal;
	}

	// returns vehicle data members
	b_Velocity->value() = m_VelocityLocal;
	b_AngularVelocity->value() = m_AngularVelocityLocal;
	b_Attitude->value() = m_Attitude;	
}

