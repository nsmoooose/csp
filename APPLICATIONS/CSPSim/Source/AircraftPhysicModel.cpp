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
 * @file AircraftPhysicModel.cpp
 *
 **/
#include <algorithm>

#include "AircraftPhysicModel.h"
#include "BaseDynamics.h"
#include "Collision.h"
#include "CSPSim.h"

AircraftPhysicModel::AircraftPhysicModel():
	PhysicModel(13) {
	setNumericalMethod(new RungeKuttaCK(this, false));
}

std::vector<double> const &AircraftPhysicModel::_f(double x, std::vector<double> &y) {   
	// dot(p,v,w,q) = f(p,v,w,q)
	static std::vector<double> dy(13);

	// bind(y,p,v,w,q); 
	m_PositionLocal = *m_Position + bodyToLocal(simdata::Vector3(y[0],y[1],y[2]));
	m_VelocityBody = simdata::Vector3(y[3],y[4],y[5]);
	m_AngularVelocityBody = simdata::Vector3(y[6],y[7],y[8]);
	m_qOrientation = simdata::Quaternion(y[9],y[10],y[11],y[12]);
	double mag = m_qOrientation.Magnitude();
	if (mag != 0.0) {
		m_qOrientation /= mag;
	}
	y[9]  = m_qOrientation.w; y[10] = m_qOrientation.x; y[11] = m_qOrientation.y; y[12] = m_qOrientation.z;

	m_ForcesBody = m_MomentsBody = simdata::Vector3::ZERO;
	m_WeightBody = localToBody(m_WeightLocal);

	m_GroundCollisionDynamics->reset(x);

	if (m_GroundCollisionDynamics && m_NearGround) {
		m_GroundCollisionDynamics->update(x);
		if (m_GroundCollisionDynamics->hasContact()) {
			m_ForcesBody += m_GroundCollisionDynamics->getForce();
			m_MomentsBody += m_GroundCollisionDynamics->getMoment();
			if (m_GroundCollisionDynamics->hasImpulsion()) {
				m_VelocityBody += m_GroundCollisionDynamics->getLinearImpulsion();
				m_AngularVelocityBody += m_GroundCollisionDynamics->getAngularImpulsion(); 
			}
		}
	}
						
	std::vector<BaseDynamics*>::iterator bd = m_Dynamics.begin();
	std::vector<BaseDynamics*>::const_iterator bdEnd = m_Dynamics.end();
	for (;bd != bdEnd; ++bd) {
		(*bd)->update(x);
		simdata::Vector3 force = (*bd)->getForce();
		m_ForcesBody += force;
		m_MomentsBody += (*bd)->getMoment();
	}

	// Add weight; null moment
	m_ForcesBody += m_WeightBody;

	// linear acceleration body: calculate v' = F/m - w^v
	m_LinearAccelBody =  m_MassInverse * m_ForcesBody - (m_AngularVelocityBody^m_VelocityBody);

	// angular acceleration body: calculate Iw' = M - w^Iw
	m_AngularAccelBody = m_InertiaInverse * 
	                     (m_MomentsBody - (m_AngularVelocityBody^(m_Inertia * m_AngularVelocityBody)));
	
	// quaternion derivative and w in body coordinates: q' = 0.5 * q * w
	simdata::Quaternion qprim = 0.5 * m_qOrientation * m_AngularVelocityBody;

	// p' = v
	dy[0] = y[3]; dy[1] = y[4]; dy[2] = y[5];
	// v'
	dy[3] = m_LinearAccelBody.x; dy[4] = m_LinearAccelBody.y; dy[5] = m_LinearAccelBody.z;
	// w'
	dy[6] = m_AngularAccelBody.x; dy[7] = m_AngularAccelBody.y; dy[8] = m_AngularAccelBody.z;
	// q'
	dy[9]  = qprim.w; dy[10] = qprim.x; dy[11] = qprim.y; dy[12] = qprim.z;
	
	return dy;
}

void AircraftPhysicModel::doSimStep(double dt) {	
	if (dt == 0.0) dt = 0.017;
	unsigned short n = std::min<short>(6,static_cast<unsigned short>(180 * dt)) + 1;
	double dtlocal = dt/n;

	std::for_each(m_Dynamics.begin(),m_Dynamics.end(),InitializeSimulationStep(dtlocal));

	Atmosphere const *atmosphere = CSPSim::theSim->getAtmosphere();
	if (atmosphere)
		m_Gravity = atmosphere->getGravity(m_PositionLocal.z);
	else
		m_Gravity = 9.806;

	m_WeightLocal = - m_Mass * m_Gravity * simdata::Vector3::ZAXIS;

	m_PositionLocal	= *m_Position;
	m_VelocityLocal	= *m_Velocity;
	m_AngularVelocityLocal = *m_AngularVelocity;
	m_qOrientation = *m_Orientation;

	updateNearGround();

	for (unsigned short i = 0; i<n; ++i) {
		
		m_VelocityBody = localToBody(m_VelocityLocal);
		m_AngularVelocityBody =	localToBody(m_AngularVelocityLocal);

		std::vector<double> y0 = bodyToY(simdata::Vector3::ZERO,m_VelocityBody,m_AngularVelocityBody,m_qOrientation);

		std::for_each(m_Dynamics.begin(),m_Dynamics.end(),PreSimulationStep(dtlocal));

		// solution
		std::vector<double> y = flow(y0, 0, dtlocal);
		// update all variables
		// Caution: dont permute these	lines
		// solution to body data members
		YToBody(y);
		// correct an eventual underground position
		if (m_GroundCollisionDynamics->hasContact())
			m_PositionBody += m_GroundCollisionDynamics->getZCorrection();

		std::for_each(m_Dynamics.begin(),m_Dynamics.end(),PostSimulationStep(dtlocal));

		// convert body to local coordinates 
		physicsBodyToLocal();

		// update attitude and force for a unit quaternion
		m_qOrientation = simdata::Quaternion(y[9],y[10],y[11],y[12]);
		double mag = m_qOrientation.Magnitude();
		if (mag	!= 0.0)
			m_qOrientation /= mag;
		
		if (m_GroundCollisionDynamics->hasContact() && m_VelocityLocal.z < 0.0) {
			m_VelocityLocal.z *= 0.9;
		}
		*m_Position = m_PositionLocal;
	}

	// returns vehicle data members
	*m_Velocity = m_VelocityLocal;
	*m_AngularVelocity = m_AngularVelocityLocal;
	*m_Orientation = m_qOrientation;	
}
