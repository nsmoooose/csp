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
 * @file AeroDynamics.cpp
 *
 **/
# if defined(_MSC_VER) && (_MSC_VER <= 1300)
#define NOMINMAX
#endif

#include "AeroDynamics.h"
#include "CSPSim.h"
#include "Log.h"

#include <SimData/InterfaceRegistry.h>
#include <SimData/Math.h>
#include <SimData/Quaternion.h>

#include <algorithm>

using simdata::RadiansToDegrees;
using simdata::DegreesToRadians;


double	const g = 9.806; // acceleration due to gravity, m/s^2

unsigned short const maxIteration = 150;
unsigned short const minIteration = 100;


SIMDATA_REGISTER_INTERFACE(AeroDynamics)

AeroDynamics::AeroDynamics(): 
	DynamicalSystem(13),
	m_ExtraForceBody(0.0,0.0,0.0), 
	m_ExtraMomentBody(0.0, 0.0, 0.0) 
{
	m_depsilon = 0.0;
	m_GearDynamics = 0;
	m_Bound = false;
	m_Distance = 0.0;
	m_AirSpeed = 0.0;
	initialize();
}

AeroDynamics::~AeroDynamics() {
}

void AeroDynamics::pack(simdata::Packer& p) const {
	Object::pack(p);
	
	p.pack(m_WingSpan);
	p.pack(m_WingChord);
	p.pack(m_WingArea);
	p.pack(m_stallAOA);
	
	p.pack(m_DeMax);
	p.pack(m_DeMin);
	p.pack(m_DaMax);
	p.pack(m_DaMin);
	p.pack(m_DrMax);
	p.pack(m_DrMin);

	p.pack(m_GMin);
	p.pack(m_GMax);

	p.pack(m_CD0);
	p.pack(m_CD_a);
	p.pack(m_CD_de);
	
	p.pack(m_CL0);
	p.pack(m_CL_a);
	p.pack(m_CL_adot);
	p.pack(m_CL_q);
	p.pack(m_CL_de);
	
	p.pack(m_CM0);
	p.pack(m_CM_a);
	p.pack(m_CM_adot);
	p.pack(m_CM_q);
	p.pack(m_CM_de);
	
	p.pack(m_CY_beta);
	p.pack(m_CY_p);
	p.pack(m_CY_r);
	p.pack(m_CY_da);
	p.pack(m_CY_dr);
	
	p.pack(m_CI_beta);
	p.pack(m_CI_p);
	p.pack(m_CI_r);
	p.pack(m_CI_da);
	p.pack(m_CI_dr);

	p.pack(m_Cn_beta);
	p.pack(m_Cn_p);
	p.pack(m_Cn_r);
	p.pack(m_Cn_da);
	p.pack(m_Cn_dr);
}

void AeroDynamics::unpack(simdata::UnPacker& p) {
	Object::unpack(p);

	p.unpack(m_WingSpan);
	p.unpack(m_WingChord);
	p.unpack(m_WingArea);
	p.unpack(m_stallAOA);
	
	p.unpack(m_DeMax);
	p.unpack(m_DeMin);
	p.unpack(m_DaMax);
	p.unpack(m_DaMin);
	p.unpack(m_DrMax);
	p.unpack(m_DrMin);

	p.unpack(m_GMin);
	p.unpack(m_GMax);

	p.unpack(m_CD0);
	p.unpack(m_CD_a);
	p.unpack(m_CD_de);
	
	p.unpack(m_CL0);
	p.unpack(m_CL_a);
	p.unpack(m_CL_adot);
	p.unpack(m_CL_q);
	p.unpack(m_CL_de);
	
	p.unpack(m_CM0);
	p.unpack(m_CM_a);
	p.unpack(m_CM_adot);
	p.unpack(m_CM_q);
	p.unpack(m_CM_de);
	
	p.unpack(m_CY_beta);
	p.unpack(m_CY_p);
	p.unpack(m_CY_r);
	p.unpack(m_CY_da);
	p.unpack(m_CY_dr);
	
	p.unpack(m_CI_beta);
	p.unpack(m_CI_p);
	p.unpack(m_CI_r);
	p.unpack(m_CI_da);
	p.unpack(m_CI_dr);

	p.unpack(m_Cn_beta);
	p.unpack(m_Cn_p);
	p.unpack(m_Cn_r);
	p.unpack(m_Cn_da);
	p.unpack(m_Cn_dr);
}


void AeroDynamics::initialize()
{
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics: Initialize()...");
	
	m_ElevatorInput = 0.0;
	m_ElevatorScale = 0.0;

	m_Elevator = 0.0;
	m_Aileron = 0.0;
	m_Rudder = 0.0;
	m_ThrustForce = simdata::Vector3::ZERO;
	
	m_qOrientation = simdata::Quaternion(1,0,0,0);

	m_AngularAccelBody = simdata::Vector3::ZERO;
	m_LinearAccelBody = simdata::Vector3::ZERO;
	m_AngularAccelLocal = simdata::Vector3::ZERO;
	m_LinearAccelLocal = simdata::Vector3::ZERO;
	
	m_GravityAcceLocal =  - g * simdata::Vector3::ZAXIS;    // gravity force is always towards earth.
	m_LiftForce = simdata::Vector3::ZERO;
	m_DragForce = simdata::Vector3::ZERO;

	m_alpha = 0.0;
	m_alpha0 = 0.0;
	m_alphaDot = 0.0;
   
	m_ForcesBody = simdata::Vector3::ZERO;
	m_MomentsBody = simdata::Vector3::ZERO;
	m_gForce = 1.0;

	m_Maxi = maxIteration;
	m_dprevdt = 0.016;
}


void AeroDynamics::convertXML() {
	// angle data are given in degree
	m_DeMax = DegreesToRadians(m_DeMax);
	m_DeMin = DegreesToRadians(m_DeMin);
	m_DaMax = DegreesToRadians(m_DaMax);
	m_DaMin = DegreesToRadians(m_DaMin);
	m_DrMax = DegreesToRadians(m_DrMax);
	m_DrMin = DegreesToRadians(m_DrMin);
	m_stallAOA = DegreesToRadians(m_stallAOA);
}


void AeroDynamics::postCreate()
{
	Object::postCreate();
	m_AspectRatio = m_WingSpan * m_WingSpan / m_WingArea;
	m_CD_i = 1.0 / (0.9 * G_PI * m_AspectRatio);
	m_HalfWingArea = 0.5 * m_WingArea;
	m_depsilon = std::min(m_GMax,fabs(m_GMin));
	setNumericalMethod(new RungeKuttaCK(this, false));
}


void AeroDynamics::bindObject(simdata::Vector3 &position, simdata::Vector3 &velocity, simdata::Vector3 &angular_velocity,
								simdata::Quaternion &orientation) {
	m_Position = &position;
	m_Velocity = &velocity;
	m_AngularVelocity = &angular_velocity;
	m_Orientation = &orientation;
	m_Bound = true;
}

void AeroDynamics::bindGearDynamics(GearDynamics &gearDynamics) {
	m_GearDynamics = &gearDynamics;
	m_GearDynamics->bindObject(m_PositionLocal, m_VelocityBody, m_AngularVelocityBody, m_qOrientation, m_NearGround, m_Height);
}

void AeroDynamics::bindGroundCollisionDynamics(GroundCollisionDynamics &groundCollisionDynamics) {
	m_GroundCollisionDynamics = &groundCollisionDynamics;
	m_GroundCollisionDynamics->bindObject(m_PositionLocal,m_VelocityBody,m_AngularVelocityBody,m_qOrientation, m_NearGround, m_Height);
	m_GroundCollisionDynamics->bindWeight(m_WeightBody);
}

void AeroDynamics::bindEngineDynamics(EngineDynamics &engineDynamics) {
	m_EngineDynamics = &engineDynamics;
	m_EngineDynamics->bindObject(m_PositionLocal,m_VelocityBody,m_AngularVelocityBody,m_qOrientation, m_NearGround, m_Height);
}

void AeroDynamics::setControlSurfaces(double aileron, double elevator, double rudder) {
	m_Aileron = aileron;
	m_ElevatorInput = elevator;
	m_Elevator = elevator;
	m_Rudder = rudder;
}

void AeroDynamics::setInertia(double mass, simdata::Matrix3 const &I) 
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
	m_InertiaInverse = I.Inverse();
}

void AeroDynamics::setBoundingRadius(double radius) 
{
	m_Radius = radius;
}

void run_test(AeroDynamics &a, unsigned short int &m_Maxi, 
	simdata::Vector3 &p,
	simdata::Vector3 &v,
	simdata::Vector3 &av,
	simdata::Quaternion &o)
{
	int run;
	double dt = 0.000500;
	simdata::Vector3 _p = p;
	simdata::Vector3 _av = av;
	simdata::Quaternion _o = o;

	for (run = 0; run < 16; run++) {
		a.setControlSurfaces(0.1, 0.35, 0.0);
		a.setThrust(97000.0);
		v = simdata::Vector3(0.0, 150.0, 0.0);
		o = _o;
		av = _av;
		p = _p;
		int i, n = int(10 / dt);
		for (i = 0; i < n; i++) {
			m_Maxi = 100;
			a.doSimStep(dt);
		}
		printf("%d %6d %8.3f\n", run, int(dt*1e+6), v.LengthSquared());
		dt = dt * 1.5;
	}
}

bool AeroDynamics::isNearGround() {
	bool near_ground = false;
	//height = (m_PositionLocal.z - m_GroundZ)*ZAXIS * m_GroundN 
	double height = (m_PositionLocal.z - m_GroundZ)* m_GroundN.z;
	if (height < m_Radius + 5.0) {
		near_ground = true;
#if 0
		if (height < 0.0) {
			// way underground... crashes aren't modelled yet, so this
			// can happen.  just give force a large positive vz 
			// component to bring us back up.
			m_VelocityLocal.z = 100.0;
			simdata::Vector3 axis = Normalized(m_VelocityLocal) ^ simdata::Vector3::ZAXIS;
			double angle = acos(axis.Length());
			qOrientation.FromAngleAxis(angle, axis);
		}
#endif
	}
	return near_ground;
}

#if 0
void AeroDynamics::doSimStep(double dt)
{
	/*
	static bool test = true;
	if (test) {
	test = false;
	run_test(*this, m_Maxi, *m_Position, *m_Velocity, *m_AngularVelocity, *m_Orientation);
	}
	*/	
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics: DoSimStep");

	assert(m_Bound);

	m_PositionLocal = *m_Position;
	m_VelocityLocal = *m_Velocity;
	m_AngularVelocityLocal = *m_AngularVelocity;
	m_qOrientation = *m_Orientation;

	if ( dt - m_dprevdt < 0.0 ) {
		if ( m_Maxi < maxIteration ) {
			++m_Maxi;
		}
	} else {
		if ( m_Maxi > minIteration ) {
			--m_Maxi;
		}
	}

	if (dt < 0.01) dt = 0.01;

	bool near_ground = isNearGround();

	simdata::Vector3 Wind(0.0, 0.0, 0.0);
	simdata::Vector3 AirflowBody;

	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics:       dt: " << dt );
	m_VelocityBody = LocalToBody(m_VelocityLocal);
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics:       V0: " << m_VelocityBody );

	m_dprevdt = dt;
	dt /= m_Maxi; 

	m_Distance += m_AirSpeed * dt;

	m_qBarFactor = m_HalfWingArea;
	Atmosphere const *atmosphere = CSPSim::theSim->getAtmosphere();
	if (atmosphere != NULL) {
		double density = atmosphere->getDensity(m_PositionLocal.z);
		m_qBarFactor *= density;
		m_Gravity = atmosphere->getGravity(m_PositionLocal.z);
		Wind = atmosphere->getWind(m_PositionLocal);
		Wind += atmosphere->getTurbulence(m_PositionLocal, m_Distance);
	} else {
		m_qBarFactor *= 1.25;
		m_Gravity = 9.8;
	}

	m_WeightLocal = - m_Mass * m_Gravity * simdata::Vector3::ZAXIS;

	for (unsigned short modelStepNumber = 0; modelStepNumber < m_Maxi; ++modelStepNumber) {	 

		m_AngularAccelBody = LocalToBody(m_AngularAccelLocal);
		m_VelocityBody = LocalToBody(m_VelocityLocal);
		m_AngularVelocityBody = LocalToBody(m_AngularVelocityLocal);

		if (near_ground) {
			// approximate (generic) ground effect... TODO: paramaterize in xml.
			double x = m_PositionLocal.z - m_GroundZ; // + wing height relative to cg
			x /= m_WingSpan;
			if (x > 1.0) x = 1.0;
			m_GE = 0.49 + (1.0 - 0.5 * x) * x;
		} else {
			m_GE = 0.0;
		}

		AirflowBody = LocalToBody(m_VelocityLocal - Wind);
		m_AirSpeed = AirflowBody.Length();
		// prevent singularities
		if (m_AirSpeed < 1.0) m_AirSpeed = 1.0;

		// Calculate angle of attack, rate of AOA
		// alpha is 0 when w velocity (i.e. vertical velocity) is 0
		double m_alphaNew = - atan2(AirflowBody.z, AirflowBody.y); 
		m_alphaDot = ( m_alphaNew - m_alpha ) / dt;

		// restrict m_alphaDot in vertical stalls
		if (m_alphaDot > 1.0) m_alphaDot = 1.0;
		if (m_alphaDot < -1.0) m_alphaDot = -1.0;

		m_alpha = m_alphaNew;

		// Calculate side angle
		// beta is 0 when v velocity (i.e. side velocity) is 0; note here v is m_VelocityBody.x
		m_beta  = atan2(AirflowBody.x, AirflowBody.y); 

		simdata::Vector3 angularAccelBody;
		simdata::Vector3 angularVelocityBody;
		simdata::Vector3 linearAccelBody;
		unsigned short GControlNumber = 0;

		m_ExtraForceBody = m_ExtraMomentBody = simdata::Vector3::ZERO;

		//do // Testing some feedback control
		{
			m_ElevatorScale = (0.95 * m_ElevatorScale + 0.05 * ControlInputValue( m_gForce ));
			m_Elevator = m_ElevatorInput * m_ElevatorScale;

			// Dynamic pressure
			m_qBarS = m_qBarFactor * m_AirSpeed * m_AirSpeed;

			if (near_ground) {
				if (m_Gear) {
					double height = m_PositionLocal.z - m_GroundZ;
					simdata::Vector3 force, moment;
					m_Gear->doComplexPhysics(m_qOrientation, 
						m_VelocityBody, 
						m_AngularVelocityBody, 
						height, 
						m_GroundN,
						dt, 
						force, 
						moment);
					m_ExtraForceBody += force;
					m_ExtraMomentBody += moment;
				}//if (m_Gear)
				if (m_Contacts) {
					simdata::Vector3 bodyn = LocalToBody(m_GroundN);
					std::vector<simdata::Vector3>::const_iterator contact;
					simdata::Vector3 origin(0.0, 0.0, m_PositionLocal.z - m_GroundZ);
					// FIXME: skipping landing gear contacts which are temporary!
					for (contact = m_Contacts->begin()+3; contact != m_Contacts->end(); contact++) {
						simdata::Vector3 contact_local = BodyToLocal(*contact) + origin;
						double height = simdata::Dot(contact_local, m_GroundN);
						if (height < 0.0) {
							// for now, just act like a hard surface	
							simdata::Vector3 Vb = m_VelocityBody + (m_AngularVelocityBody^(*contact));
							simdata::Vector3 V = BodyToLocal(Vb);
							static const float groundK = 1.0e+7;
							static const float groundBeta = 1.0e+6;
							double impact = simdata::Dot(V, m_GroundN);
							// spring plus damping terms
							double scale = - (height*groundK + impact*fabs(impact)*groundBeta);
							// semiarbitrary force limit
							if (fabs(scale)>groundK) {
								// dissipate some extra energy
								double body_impact = simdata::Dot(m_VelocityBody, bodyn);
								if (impact < -10.0) {
									//m_VelocityBody *= 0.50;
									
									m_VelocityBody -= 0.25 * body_impact * bodyn;
									std::cout << "SLAM!!!!\n";
								} else {
									//m_VelocityBody *= 0.95;
									m_VelocityBody -= 0.05 * body_impact * bodyn;
									std::cout << "SLAM!\n";
								}
								if (scale > 0.0) {
									scale = groundK;
								} else {
									scale = -groundK;
								}
							}//if (fabs(scale)>groundK)
							simdata::Vector3 force = scale * bodyn;

							// sliding velocity 
							V -= impact * m_GroundN;
							// in body coordinates
							V = LocalToBody(V);
							if (height < -1.0) height = -1.0;
							scale = -2.0 * groundK * height;  // 2 G slide
							// reduce to zero as speed drops
							double v = V.Length();
							scale = scale / (0.1 + v);
							// limit to 4G max
							if (scale * v > m_Mass * 40.0) {
								scale = m_Mass * 40.0 / v;
							}
							force += -scale * V;

							m_ExtraForceBody += force;
							m_ExtraMomentBody += (*contact) ^ force;
						} //if (height < 0.0)
					} //for (contact
				} //if (m_Contacts)
			} //if (near_ground)

			// Calculate moment
			m_MomentsBody = getMoment();

			// First, calculate angular acceleration
			angularAccelBody = m_InertiaInverse * 
				(m_MomentsBody - (m_AngularVelocityBody^(m_Inertia * m_AngularVelocityBody)));

			// Angular acceleration mean
			simdata::Vector3 angularAccelMeanBody = angularAccelBody;
			angularAccelMeanBody += m_AngularAccelBody;
			angularAccelMeanBody *= 0.5;

			// Integrate angular acceleration gives angular velocity
			angularVelocityBody = dt * angularAccelMeanBody + m_AngularVelocityBody;
			// Games gems advice
			angularVelocityBody *= 0.999; 

			// Calculate aero (!) force
			m_ForcesBody = getForce();

			// Add gravity
	        m_WeightBody = LocalToBody(m_WeightLocal);
	        m_ForcesBody += m_WeightBody;

			// Add thrust concentrated in y direction
	        m_ForcesBody += m_ThrustForce;

			// Deduce linear acceleration
			linearAccelBody =  m_MassInverse * m_ForcesBody - (angularVelocityBody^m_VelocityBody);

			++GControlNumber;
		}
		// Exit when g force is convenable or too many loops
		//while ( ( m_gForce < m_GMin || m_gForce > m_GMax || m_alpha > m_stallAOA  ) 
		//	  && GControlNumber < 1 ); 


		// Update angular accelerations
		m_AngularAccelBody = angularAccelBody;
		m_AngularAccelLocal = BodyToLocal(m_AngularAccelBody);

		// Update linear acceleration body
		// Linear acceleration mean
		simdata::Vector3 linearAccelMeanBody = m_LinearAccelBody;
		linearAccelMeanBody += linearAccelBody;
		linearAccelMeanBody *= 0.5;
		m_LinearAccelBody = linearAccelBody;

		// Integrate linear velocity body
		m_VelocityBody += dt * linearAccelMeanBody;

		// Update local linear velocity
		// Calculate local velocity before changing coordinates
		simdata::Vector3 linearVelocityLocal = BodyToLocal(m_VelocityBody);
		simdata::Vector3 linearVelocityMeanLocal = m_VelocityLocal;
		linearVelocityMeanLocal += linearVelocityLocal;
		linearVelocityMeanLocal *= 0.5;
		m_VelocityLocal = linearVelocityLocal;

		m_AngularVelocityLocal = BodyToLocal(angularVelocityBody);

		// Integrate the rotation quaternion
		m_qOrientation += 0.25f * dt * ( m_qOrientation * (m_AngularVelocityBody + angularVelocityBody));
		// Now normalize the orientation quaternion:
		double mag = m_qOrientation.Magnitude();
		if (mag != 0.0) {
			m_qOrientation /= mag;
		}

		// Update angular velocity
		m_AngularVelocityBody = angularVelocityBody;

		// Update position in local frame
		m_PositionLocal += dt * linearVelocityMeanLocal;


	}//for (modelStepNumber)

	*m_Position = m_PositionLocal;
	*m_Velocity = m_VelocityLocal;
	*m_AngularVelocity = m_AngularVelocityLocal;
	*m_Orientation = m_qOrientation;

	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics:   Forces: " << m_ForcesBody );
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics:  Moments: " << m_MomentsBody );
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics:ang accel: " << m_AngularAccelBody);
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics: Ang Velo: " << m_AngularVelocityBody );
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics:   LinAcc: " << m_LinearAccelBody );
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics: Velocity: " << m_VelocityLocal);
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics:    Speed: " << m_AirSpeed);
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics: AngOfAtt: " << RadiansToDegrees(m_alpha) );
	//	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics:   VelDir: " << VelocityDirection );
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics: Position: " << m_PositionLocal );
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics:   Orient: " << m_qOrientation );
	//	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics:  Euler A: " << m_EulerAngles );

}
#endif

// must calculate the resultant forces ...
simdata::Vector3 AeroDynamics::getForce()
{
	static simdata::Vector3 forcesBody;
#if 0
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics: BodyVelocity: " << m_VelocityBody);
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics: local velocity: " << m_VelocityLocal);
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics: BodySpeed: " << m_AirSpeed);
#endif
	
	// Calculate Aero Forces

	// Caution: calculate lift first to get vertical load factor
	// Lift
	forcesBody = m_qBarS * LiftVector();

	// Evaluate g's
	m_gForce = m_MassInverse * forcesBody.z / 9.81;

	// Drag and sideforce
	forcesBody += m_qBarS * (DragVector() + SideVector());

	// Add in extra forces and reset
	forcesBody += m_ExtraForceBody;

	return forcesBody;
}


// ... and moments on the aircraft
simdata::Vector3 AeroDynamics::getMoment()
{
	static simdata::Vector3 momentsBody;

	momentsBody.x = CalculatePitchMoment(m_qBarS); // Pitch
	momentsBody.y = CalculateRollMoment(m_qBarS);  // Roll
	momentsBody.z = CalculateYawMoment(m_qBarS);   // Yaw

	momentsBody += m_ExtraMomentBody;

	return momentsBody;
}


double AeroDynamics::CalculateLiftCoefficient() const
{ // Cl should be calculated before Cd (IF it is used in Cd as induced drag)
	double alpha = m_alpha;

	// prevent driving this equation from from its range of validity
	if (alpha > 0.8) alpha = 0.8;
	if (alpha < -0.8) alpha = -0.8;
	
	double Cl = ((m_CL0) + 
	        (m_CL_a * alpha) + 
	        (m_CL_q * m_AngularVelocityBody.x + m_CL_adot * m_alphaDot ) * m_WingChord / ( 2.0 * m_AirSpeed) + 
	        (m_CL_de * m_Elevator)
	       );

	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics::CalculateLiftCoefficient() " << 
	                                Cl << " at alpha = " << RadiansToDegrees(m_alpha) << 
	                                ", Elevator: " << m_Elevator );

	return Cl;
}


simdata::Vector3 const &AeroDynamics::LiftVector()
{
	static simdata::Vector3 lift;

	m_CL = CalculateLiftCoefficient();

	if ( fabs(m_alpha) > m_stallAOA)
	{
		double c = fabs(m_alpha) - m_stallAOA;
		m_CL -= c * c * 3.0;
		if (m_CL < 0.0) m_CL = 0.0;
		//c *= c;
		//lift.y -= c;
		//lift.z /= 1.0 + c;
	}
	
	lift.x = 0.0;
	lift.y = m_CL * sin(m_alpha);
	lift.z = m_CL * cos(m_alpha);

	return lift;
}


double AeroDynamics::CalculateDragCoefficient() const
{
	//static int x = 0;
	double drag_coe;
	double alpha = m_alpha;
	
	// prevent driving this equation from from its range of validity
	if (alpha > 0.8) alpha = 0.8;
	if (alpha < -0.8) alpha = -0.8;
	
	drag_coe =  m_CD0 + m_CD_a * fabs(alpha) + m_CD_de * fabs(m_Elevator);
	//drag_coe =  -0.0194 + 0.0214 * alpha + (- 0.29 * m_Elevator + 0.0033 + 0.206 * alpha) * m_Elevator + (-0.699 - 0.903 * alpha) * alpha * alpha;
	//drag_coe = - drag_coe;

	m_GE = calculateGroundEffectCoefficient();

	double induced = m_GE * m_CD_i * m_CL * m_CL;
	if (induced < 0.0) induced = 0.0;

	//if (x++ % 1000 == 0) {
	//	printf("%6.1f %6.2f %6.2f %6.2f %6.2f\n", m_gForce, drag_coe, induced, m_alpha, m_Elevator);
	//}

	drag_coe += induced;
	
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics::CalculateDragCoefficient() " << 
	                                drag_coe << " at alpha = " << RadiansToDegrees(m_alpha) << 
	                                ", Elevator: " << m_Elevator << ", Induced: " << induced << ", GE: " << m_GE << ", m_CL: " << m_CL );

	return drag_coe;
}


simdata::Vector3 const& AeroDynamics::DragVector() const
{
	static simdata::Vector3 drag;
	double Cd = CalculateDragCoefficient();
	
	drag.x = 0.0;
	drag.y = - Cd * cos(m_alpha);
	drag.z = Cd * sin(m_alpha);

	return drag;
}


double AeroDynamics::CalculateSideCoefficient() const
{
	double side_coe;
	double beta = m_beta;

/*
	// keep beta within about 30 degrees of the body axis
	if (beta > -0.5 && beta < 0.5) {
	} else {
		if (beta > 1.57) {
			beta = 3.1416 - beta;
			if (beta > 0.5) beta = 0.5;
		} else 
		if (beta > 0.5) {
			beta = 0.5;
		} else 
		if (beta < -1.57) {
			beta = -3.1416 - beta;
			if (beta < -0.5) beta = -0.5;
		} else
		if (beta < -0.5) {
			beta = -0.5;
		}
	}
*/

	side_coe = m_CY_beta * beta + m_CY_dr * m_Rudder - m_CY_r * m_AngularVelocityBody.z;

	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics::CalculateSideCoefficient() " << side_coe );

	return side_coe;
}


simdata::Vector3 const&  AeroDynamics::SideVector() const
{
	static simdata::Vector3 side;
	double Cs = CalculateSideCoefficient();
	
	side.x = Cs * cos(m_beta);
	side.y = - Cs * sin(m_beta);
	//side.x = Cs * sin(m_beta);
	//side.y = - Cs * cos(m_beta);
	side.z = 0.0;

	return side;
}


double AeroDynamics::CalculateRollMoment(double const qbarS) const
{
	return ( (m_CI_beta * m_beta) + 
	         (m_CI_da * m_Aileron) +
	         (m_CI_p * m_AngularVelocityBody.y - m_CI_r * m_AngularVelocityBody.z)* m_WingSpan / ( 2.0 * m_AirSpeed) +
	         (m_CI_dr * m_Rudder)
	       ) * qbarS * m_WingSpan;
}


double AeroDynamics::CalculatePitchMoment(double const qbarS) const 
{
	return ( (m_CM0) + 
	         (m_CM_a * m_alpha) +
	         (m_CM_q * m_AngularVelocityBody.x + m_CM_adot * m_alphaDot) * m_WingChord / ( 2.0 * m_AirSpeed) +
	         (m_CM_de * m_Elevator)
	       ) * qbarS * m_WingChord;
}


double AeroDynamics::CalculateYawMoment(double const qbarS) const
{   
	return - ( (m_Cn_beta * m_beta) + 
	           (m_Cn_p * m_AngularVelocityBody.y - m_Cn_r * m_AngularVelocityBody.z) * m_WingSpan / ( 2.0 * m_AirSpeed) +
	           (m_Cn_da * m_Aileron + m_Cn_dr * m_Rudder)
	         ) * qbarS *  m_WingSpan; // Yaw
} 

simdata::Vector3 AeroDynamics::LocalToBody(const simdata::Vector3 & vec )
{
	return simdata::QVRotate( m_qOrientation.Bar(), vec );
}


simdata::Vector3 AeroDynamics::BodyToLocal(const simdata::Vector3 & vec )
{
	return simdata::QVRotate( m_qOrientation, vec );
}

double AeroDynamics::CIVbasis(double p_t) const
{
	double cIV = p_t * p_t  * ( 3.0 - 2.0 * p_t);
	return cIV;
}


double AeroDynamics::ControlInputValue(double p_gForce) const { // to reduce G, decrease deflection control surface
	double control = 1.0;

	if (p_gForce > m_GMax && m_ElevatorInput > 0.0) return 0.0;
	if (p_gForce < m_GMin && m_ElevatorInput < 0.0) return 0.0;
	if ( m_alpha > m_stallAOA && m_ElevatorInput > 0.0) return 0.0;
	if ( p_gForce > m_GMax - m_depsilon && m_ElevatorInput > 0.0) {
		control = CIVbasis((m_GMax - p_gForce) / m_depsilon);
	} else {
		if ( p_gForce < m_GMin + m_depsilon && m_ElevatorInput < 0.0) {
			control = CIVbasis((p_gForce - m_GMin) / m_depsilon);
		}
	}
	
	return control;
}


void AeroDynamics::updateAngles(double h) {
	m_alpha = - atan2( m_AirflowBody.z, m_AirflowBody.y ); 
	m_alphaDot = ( m_alpha - m_alpha0 ) / h;
	// restrict m_alphaDot in vertical stalls
	if (m_alphaDot > 1.0) m_alphaDot = 1.0;
	if (m_alphaDot < -1.0) m_alphaDot = -1.0;
	
	// Calculate side angle
	// beta is 0 when v velocity (i.e. side velocity) is 0; note here v is m_AirflowBody.x
	m_beta  = atan2(m_AirflowBody.x, m_AirflowBody.y);
}

double AeroDynamics::calculateGroundEffectCoefficient() const {
// approximate (generic) ground effect... TODO: paramaterize in xml.
	double GE = 0.0;
	if (m_NearGround) {
		double h = m_PositionLocal.z - m_GroundZ; // + wing height relative to cg
		h /= m_WingSpan;
		if (h > 1.0) 
			h = 1.0;
		GE = 0.49 + (1.0 - 0.5 * h) * h;
	}
	return GE;
}

double const damping = 0.98;

std::vector<double> const &AeroDynamics::_f(double x, std::vector<double> &y) {   
	// dot(p,v,w,q) = f(p,v,w,q)
	static std::vector<double> dy(13);

	// bind(y,p,v,w,q); 
	m_PositionLocal = *m_Position + BodyToLocal(simdata::Vector3(y[0],y[1],y[2]));
	m_VelocityBody = simdata::Vector3(y[3],y[4],y[5]);
	m_AngularVelocityBody = damping * simdata::Vector3(y[6],y[7],y[8]);
	m_qOrientation = simdata::Quaternion(y[9],y[10],y[11],y[12]);
	double mag = m_qOrientation.Magnitude();
	if (mag != 0.0) 
		m_qOrientation /= mag;
	y[9]  = m_qOrientation.w; y[10] = m_qOrientation.x; y[11] = m_qOrientation.y; y[12] = m_qOrientation.z;

	m_AirflowBody = m_VelocityBody - LocalToBody(m_WindLocal);
	m_AirSpeed = m_AirflowBody.Length();
	if (m_AirSpeed < 1.0) m_AirSpeed = 1.0;
	m_qBarS = m_qBarFactor * m_AirSpeed * m_AirSpeed;

	// pass AOA, AOA dot, side angle
	updateAngles(x);

	m_ForcesBody = m_MomentsBody = simdata::Vector3::ZERO;
	m_WeightBody = LocalToBody(m_WeightLocal);
	m_GroundCollisionDynamics->reset();

	// ground torque
	if (isNearGround()) {
		if (m_GearDynamics) {
			m_GearDynamics->update(x);
			m_ForcesBody += m_GearDynamics->getForce();
			m_MomentsBody += m_GearDynamics->getMoment();
		}//if (m_Gear)
		if (m_GroundCollisionDynamics) {
			m_GroundCollisionDynamics->update(x);
			if (m_GroundCollisionDynamics->hasContact()) {
				m_ForcesBody += m_GroundCollisionDynamics->getForce();
				m_MomentsBody += m_GroundCollisionDynamics->getMoment();
				if (m_GroundCollisionDynamics->hasImpulsion()) {
					m_VelocityBody += m_GroundCollisionDynamics->getLinearImpulsion();
					m_AngularVelocityBody += m_GroundCollisionDynamics->getAngularImpulsion(); 
				}
			}
		} //if (m_HasContactPoints)
	} // if (m_NearGround)

	// Add aerodynamic force
	m_ForcesBody += getForce();

	// Add weight
	m_ForcesBody += m_WeightBody;

	// Add thrust
	m_EngineDynamics->update(x);
	m_ForcesBody += m_EngineDynamics->getForce();

	// linear acceleration body: calculate v' = F/m - w^v
	m_LinearAccelBody =  m_MassInverse * m_ForcesBody - (m_AngularVelocityBody^m_VelocityBody);
	
	// Add aerodynamic moment
	m_MomentsBody += getMoment();

	// Add engine moment
	m_MomentsBody += m_EngineDynamics->getMoment();

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

std::vector<double> const &bind(simdata::Vector3 const &p,
                                simdata::Vector3 const &v,
                                simdata::Vector3 const &w,
                                simdata::Quaternion const &q) {
	static std::vector<double> y(13);
	y[0] = p.x; y[1] = p.y; y[2] = p.z;
	y[3] = v.x; y[4] = v.y; y[5] = v.z;
	y[6] = w.x; y[7] = w.y; y[8] = w.z;
	y[9] = q.w; y[10] = q.x; y[11] = q.y; y[12] = q.z;
	return y;
}

void AeroDynamics::bindToBody(std::vector<double> const &y) {
	m_PositionBody = simdata::Vector3(y[0],y[1],y[2]);
	m_VelocityBody = simdata::Vector3(y[3],y[4],y[5]);
	m_AngularVelocityBody = damping * simdata::Vector3(y[6],y[7],y[8]);
}
 
void AeroDynamics::BodyToLocal() {
	m_qOrientation = *m_Orientation;
	m_PositionLocal = *m_Position + BodyToLocal(m_PositionBody);
	m_VelocityLocal = BodyToLocal(m_VelocityBody);
	m_AngularVelocityLocal = BodyToLocal(m_AngularVelocityBody);
}

void AeroDynamics::doSimStep2(double dt) {	
	static double cdt = 0; 
	static unsigned	long c = 0;

	if (dt == 0.0) dt =	0.017;

	unsigned short const n = 180 * dt ;
	double dtlocal = dt/n;

	cdt	+= dt;
	c++;
	if (!(c	% 5000)) 
		std::cout << "average dt = " <<	cdt/c << "\n";

	double u = (0.034 + dt)/(1.0 - atan(m_AirSpeed) / G_PI);
	m_ElevatorScale	= (1.0 - u) * m_ElevatorScale + u * ControlInputValue( m_gForce );
	m_Elevator = m_ElevatorInput * m_ElevatorScale;

	// initial conditions

	m_qBarFactor = m_HalfWingArea;
	Atmosphere const *atmosphere = CSPSim::theSim->getAtmosphere();
	if (atmosphere)	{
		double density = atmosphere->getDensity(m_PositionLocal.z);
		m_qBarFactor *=	density;
		m_Gravity =	atmosphere->getGravity(m_PositionLocal.z);
		m_WindLocal	= atmosphere->getWind(m_PositionLocal);
		m_WindLocal	+= atmosphere->getTurbulence(m_PositionLocal, m_Distance);
	} else {
		m_qBarFactor *=	1.25;
		m_Gravity =	9.806;
	}

	m_Distance += m_AirSpeed * dt;

	m_WeightLocal = - m_Mass *	m_Gravity *	simdata::Vector3::ZAXIS;

	for	(unsigned short	i = 0; i<n; ++i) {
		m_PositionLocal	= *m_Position;
		m_VelocityLocal	= *m_Velocity;
		m_AngularVelocityLocal = *m_AngularVelocity;
		m_qOrientation = *m_Orientation;

		m_AirflowBody =	LocalToBody(m_VelocityLocal	- m_WindLocal);
		m_AirSpeed = m_AirflowBody.Length();
		// prevent singularities
		if (m_AirSpeed < 1.0) m_AirSpeed = 1.0;

		m_VelocityBody = LocalToBody(m_VelocityLocal);
		m_AngularVelocityBody =	LocalToBody(m_AngularVelocityLocal);
		updateAngles(dt);
		std::vector<double>	y0 = ::bind(simdata::Vector3::ZERO,	m_VelocityBody,	m_AngularVelocityBody, m_qOrientation);

		// solution
		std::vector<double>	y =	flow(y0, 0,	dtlocal);

		// update all variables
		// Caution:	don	t permute these	lines
		// bind solution to body data members
		bindToBody(y);
		// correct an eventual underground position
		if (m_GroundCollisionDynamics->hasContact())
			m_PositionBody += m_GroundCollisionDynamics->getZCorrection();

		updateAngles(dt);
		m_alpha0 = m_alpha;

		// convert body members to local coordinates 
		BodyToLocal();

		// update attitude and check for a unit quaternion
		m_qOrientation = simdata::Quaternion(y[9],y[10],y[11],y[12]);
		double mag = m_qOrientation.Magnitude();
		if (mag	!= 0.0)
			m_qOrientation /=	mag;

		// returns vehicle data members
		*m_Position	= m_PositionLocal;
		*m_Velocity	= m_VelocityLocal;
		*m_AngularVelocity = m_AngularVelocityLocal;
		*m_Orientation = m_qOrientation;	
	}
}


