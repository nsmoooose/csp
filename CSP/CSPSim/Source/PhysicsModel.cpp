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
 * @file PhysicsModel.cpp
 *
 **/

#include <BaseDynamics.h>
#include <Collision.h>
#include <CSPSim.h>
#include <PhysicsModel.h>
#include <KineticsChannels.h>


using bus::Kinetics;


PhysicsModel::PhysicsModel(size_type dimension):
	DynamicalSystem(dimension),
	m_Damping(1.0),
	m_Attitude(simdata::Quat::IDENTITY),
	m_ForcesBody(simdata::Vector3::ZERO),
	m_MomentsBody(simdata::Vector3::ZERO),
	m_AngularAccelBody(simdata::Vector3::ZERO),
	m_LinearAccelBody(simdata::Vector3::ZERO) /* ,
	m_NearGround(false),
	m_Height(0.0),
	m_qBar(0.0),
	m_WindBody(simdata::Vector3::ZERO),
	m_Distance(0.0) */
{
}

PhysicsModel::~PhysicsModel() {
}


void PhysicsModel::addDynamics(simdata::Ref<BaseDynamics> dynamic) {
/* XXX
	simdata::Ref<GroundCollisionDynamics> gcd = dynamic;
	if (gcd.valid()) {
		m_GroundCollisionDynamics = gcd;
	} else {
*/
		m_Dynamics.push_back(dynamic);
/* XXX
	}
*/
	dynamic->bindKinematics(m_PositionLocal, m_VelocityBody, m_AngularVelocityBody, m_Attitude);
	/* XXX
	dynamic->bindGroundParameters(m_NearGround, m_Height, m_NormalGround);
	dynamic->bindAeroParameters(m_qBar, m_WindBody);
	*/
}


void PhysicsModel::postCreate() {
	System::postCreate();
	int n = getNumChildren();
	for (int i=0; i<n; i++) {
		simdata::Ref<BaseDynamics> dynamics;
		dynamics = getChild(i);
		if (dynamics.valid()) {
			addDynamics(dynamics);
			CSP_LOG(OBJECT, DEBUG, "PhysicsModel::postCreate() adding dynamics: " << dynamics->getClassName());
		}
	}
}

simdata::Vector3 PhysicsModel::localToBody(simdata::Vector3 const &vec ) {
	return m_Attitude.invrotate(vec);
}


simdata::Vector3 PhysicsModel::bodyToLocal(simdata::Vector3 const &vec ) {
	return m_Attitude.rotate(vec);
}

Vector::Vectord const &PhysicsModel::bodyToY(simdata::Vector3 const &p,
                                simdata::Vector3 const &v,
                                simdata::Vector3 const &w,
                                simdata::Quat const &q) {
	static Vector::Vectord y(13);
	y[0] = p.x(); y[1] = p.y(); y[2] = p.z();
	y[3] = v.x(); y[4] = v.y(); y[5] = v.z();
	y[6] = w.x(); y[7] = w.y(); y[8] = w.z();
	y[9] = q.w(); y[10] = q.x(); y[11] = q.y(); y[12] = q.z();
	return y;
}

void PhysicsModel::YToBody(Vector::Vectord const &y) {
	m_PositionBody.set(y[0],y[1],y[2]);
	m_VelocityBody.set(y[3],y[4],y[5]);
	//m_AngularVelocityBody = m_Damping * simdata::Vector3(y[6],y[7],y[8]);
	m_AngularVelocityBody.set(y[6],y[7],y[8]);
}

void PhysicsModel::physicsBodyToLocal() {
	m_PositionLocal = b_Position->value() + bodyToLocal(m_PositionBody);
	m_VelocityLocal = bodyToLocal(m_VelocityBody);
	m_AngularVelocityLocal = bodyToLocal(m_AngularVelocityBody);
}


void PhysicsModel::registerChannels(Bus *) {
}

void PhysicsModel::importChannels(Bus *bus) {
	assert(bus!=0);
	b_Position = bus->getSharedChannel(Kinetics::Position, true, true);
	b_Velocity = bus->getSharedChannel(Kinetics::Velocity, true, true);
	b_AngularVelocity = bus->getSharedChannel(Kinetics::AngularVelocity, true, true);
	b_Attitude = bus->getSharedChannel(Kinetics::Attitude, true, true);
	b_AccelerationBody = bus->getSharedChannel(Kinetics::AccelerationBody, true, true);
	b_Mass = bus->getChannel(Kinetics::Mass);
	b_Inertia = bus->getChannel(Kinetics::Inertia);
	b_InertiaInverse = bus->getChannel(Kinetics::InertiaInverse);
}


/**
void PhysicsModel::bindObject(simdata::Vector3 &position,
                              simdata::Vector3 &velocity,
                              simdata::Vector3 &angular_velocity,
                              simdata::Quat &orientation) {
	m_Position = &position;
	m_Velocity = &velocity;
	m_AngularVelocity = &angular_velocity;
	m_Orientation = &orientation;
}

void PhysicsModel::setBoundingRadius(double radius) {
	m_Radius = radius;
}

void PhysicsModel::setInertia(double mass, simdata::Matrix3 const &I)
{
	m_Mass = mass;
	if (mass == 0.0) {
		m_MassInverse = 0.0;
	} else m_MassInverse = 1.0 / mass;
	// convert from standard literature coordinates (x = nose, y = right, z = down)
	// to our internal coordinate frame (x = right, y = nose, z = up)
	//simdata::Matrix3 R(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0);
	//m_Inertia = R * I * R.Inverse();
	m_Inertia = I;  // use CSP frame in XML
	m_InertiaInverse = I.getInverse();
}

void PhysicsModel::updateNearGround(double dt) {
	// XXX NormalGround and GroundN are redundant.... remove one of them!
	m_NormalGround = m_GroundN;
	m_Height = (m_PositionLocal.z() - m_GroundZ) * m_GroundN.z();
	if (m_Height < m_Radius) {
		m_NearGround = true;
	} else {
		m_NearGround = false;
	}
}

void PhysicsModel::updateAeroParameters(double dt) {
	Atmosphere const *atmosphere = CSPSim::theSim->getAtmosphere();
	if (atmosphere)	{
		m_qBar = atmosphere->getDensity(m_PositionLocal.z());
		simdata::Vector3 wind = atmosphere->getWind(m_PositionLocal);
		wind += atmosphere->getTurbulence(m_PositionLocal, m_Distance);
		m_WindBody = m_Attitude.invrotate(wind);
	} else {
		m_qBar = 1.25; // nominal sea-level air density
		m_WindBody = simdata::Vector3::ZERO;
	}
	double air_speed = m_WindBody.length();
	m_Distance += air_speed * dt;
}

*/
