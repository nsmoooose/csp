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
#include <algorithm>

#include "AeroDynamics.h"
#include "LogStream.h"

#include <SimData/InterfaceRegistry.h>
#include <SimData/Math.h>
#include <SimData/Quaternion.h>

using simdata::RadiansToDegrees;
using simdata::DegreesToRadians;

double	const	g	= 9.806; // acceleration due to gravity, m/s^2

unsigned short const maxIteration = 150;
unsigned short const minIteration = 100;


SIMDATA_REGISTER_INTERFACE(AeroDynamics)


AeroDynamics::AeroDynamics(): DynamicalSystem(13) {
	m_depsilon = 0.0;
	m_Bound = false;
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

	postProcess();
}


void AeroDynamics::initialize()
{
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics: Initialize()...");
	
	m_ElevatorInput = 0.0;
	m_ElevatorScale = 0.0;

	m_Elevator = 0.0;
	m_Aileron = 0.0;
	m_Rudder = 0.0;
	m_Thrust = 0.0;
	
	qOrientation = simdata::Quaternion(1,0,0,0);

	m_AngularAccelBody = simdata::Vector3::ZERO;
	m_LinearAccelBody = simdata::Vector3::ZERO;
	m_AngularAccelLocal = simdata::Vector3::ZERO;
	m_LinearAccelLocal = simdata::Vector3::ZERO;
	
	m_GravityForce =  - g * simdata::Vector3::ZAXIS;    // gravity force is always towards earth.
	m_ThrustForce = simdata::Vector3::ZERO;
	m_LiftForce = simdata::Vector3::ZERO;
	m_DragForce = simdata::Vector3::ZERO;
	m_CurrentForceTotal = simdata::Vector3::ZERO;

	m_ForcesBody = simdata::Vector3::ZERO;

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


void AeroDynamics::postProcess()
{
	m_depsilon = std::min(static_cast<double>(m_GMax),fabs(m_GMin)) / 2.0;
	m_CD_i = m_WingArea / (0.9 * G_PI * m_WingSpan * m_WingSpan);
}


void AeroDynamics::bindObject(simdata::Vector3 &position, 
                              simdata::Vector3 &velocity, 
			      simdata::Vector3 &angular_velocity,
			      simdata::Quaternion &orientation)
{
	m_Position = &position;
	m_Velocity = &velocity;
	m_AngularVelocity = &angular_velocity;
	m_Orientation = &orientation;
	m_Bound = true;
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

void AeroDynamics::doSimStep(double dt)
{
/*
	static bool test = true;
	if (test) {
		test = false;
		run_test(*this, m_Maxi, *m_Position, *m_Velocity, *m_AngularVelocity, *m_Orientation);
	}
*/	
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics: DoSimStep");

	assert(m_Bound);

	m_PositionLocal = *m_Position;
	m_VelocityLocal = *m_Velocity;
	m_AngularVelocityLocal = *m_AngularVelocity;
	qOrientation = *m_Orientation;
	m_Speed = m_VelocityLocal.Length();

	// prevent singularities
	if (m_Speed < 1.0) m_Speed = 1.0;

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

	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics:       dt: " << dt );
	m_VelocityBody = LocalToBody(m_VelocityLocal);
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics:       V0: " << m_VelocityBody );
		
	m_dprevdt = dt;
	dt /= m_Maxi; 


	
    m_qBarFactor = 0.5 * CalculateAirDensity(m_PositionLocal.z) * m_WingArea;
	m_Gravity = CalculateGravity(m_PositionLocal.z);
	m_GravityWorld = - m_Mass * m_Gravity * simdata::Vector3::ZAXIS;

	for (unsigned short modelStepNumber = 0; modelStepNumber < m_Maxi; ++modelStepNumber) {	 

		m_AngularAccelBody = LocalToBody(m_AngularAccelLocal);
		m_VelocityBody = LocalToBody(m_VelocityLocal);
		m_AngularVelocityBody = LocalToBody(m_AngularVelocityLocal);
		
		// Calculate angle of attack, rate of AOA
		// alpha is 0 when w velocity (i.e. vertical velocity) is 0
		double m_alphaNew = - atan2( m_VelocityBody.z, m_VelocityBody.y ); 
		m_alphaDot = ( m_alphaNew - m_alpha ) / dt;

		// restrict m_alphaDot in vertical stalls
		if (m_alphaDot > 1.0) m_alphaDot = 1.0;
		if (m_alphaDot < -1.0) m_alphaDot = -1.0;

		m_alpha = m_alphaNew;
		
		// Calculate side angle
		// beta is 0 when v velocity (i.e. side velocity) is 0; note here v is m_VelocityBody.x
		m_beta  = atan2(m_VelocityBody.x, m_VelocityBody.y); 
		
		simdata::Vector3 angularAccelBody;
		simdata::Vector3 angularVelocityBody;
		simdata::Vector3 linearAccelBody;
		unsigned short GControlNumber = 0;

		//do // Testing some feedback control
		{
			m_ElevatorScale = (0.95 * m_ElevatorScale + 0.05 * ControlInputValue( m_gForce ));
			m_Elevator = m_ElevatorInput * m_ElevatorScale;
			
			// Dynamic pressure
			double qBarS = m_qBarFactor * m_Speed * m_Speed;
			
			// Calculate moment
			m_MomentsBody = CalculateMoments(qBarS);
			
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
			
			// Calculate force
			m_ForcesBody = CalculateForces(qBarS);  
			
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
		
		m_Speed = m_VelocityBody.Length();
		if (m_Speed < 1.0) m_Speed = 1.0;

		// Update local linear velocity
		// Calculate local velocity before changing coordinates
		simdata::Vector3 linearVelocityLocal = BodyToLocal(m_VelocityBody);
		simdata::Vector3 linearVelocityMeanLocal = m_VelocityLocal;
        linearVelocityMeanLocal += linearVelocityLocal;
		linearVelocityMeanLocal *= 0.5;
		m_VelocityLocal = linearVelocityLocal;
		
		m_AngularVelocityLocal = BodyToLocal(angularVelocityBody);

		// Integrate the rotation quaternion
		qOrientation += 0.25f * dt * ( qOrientation * (m_AngularVelocityBody + angularVelocityBody));
		// Now normalize the orientation quaternion:
		double mag = qOrientation.Magnitude();
		if (mag != 0.0) {
			qOrientation /= mag;
		}

		// Update angular velocity
		m_AngularVelocityBody = angularVelocityBody;
		
		// Update position in local frame
		m_PositionLocal += dt * linearVelocityMeanLocal;
		
	}

	*m_Position = m_PositionLocal;
	*m_Velocity = m_VelocityLocal;
	*m_AngularVelocity = m_AngularVelocityLocal;
	*m_Orientation = qOrientation;
		
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics:   Forces: " << m_ForcesBody );
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics:  Moments: " << m_MomentsBody );
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics:ang accel: " << m_AngularAccelBody);
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics: Ang Velo: " << m_AngularVelocityBody );
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics:   LinAcc: " << m_LinearAccelBody );
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics: Velocity: " << m_VelocityLocal);
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics:    Speed: " << m_Speed);
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics: AngOfAtt: " << RadiansToDegrees(m_alpha) );
//	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics:   VelDir: " << VelocityDirection );
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics: Position: " << m_PositionLocal );
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics:   Orient: " << qOrientation );
//	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics:  Euler A: " << m_EulerAngles );

}


// must calculate the resultant forces ...
simdata::Vector3 const& AeroDynamics::CalculateForces( double const p_qBarS )
{
	static simdata::Vector3 forceBody;
#if 0
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics: BodyVelocity: " << m_VelocityBody);
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics: local velocity: " << m_VelocityLocal);
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics: BodySpeed: " << m_Speed);
#endif
	
	// Calculate Aero Forces

	// Caution: calculate lift first
	// Lift
	forceBody = p_qBarS * LiftVector();

	//static int x = 0;
	//if (x++ % 1000 == 0) {
		//printf("%4.2f %8.2f %8.2f %8.2f\n", m_alpha*180/3.14, p_qBarS, forceBody.y, forceBody.z);
	//}

	// Evaluate g's
	m_gForce = m_MassInverse * forceBody.z / 9.81;

	// Drag and sideforce
	forceBody += p_qBarS * DragVector();
	forceBody += p_qBarS * SideVector();

	// Add thrust concentrated in y direction
	forceBody.y += m_Thrust;

	// Add gravity
	m_GravityBody = LocalToBody(m_GravityWorld);
	forceBody += m_GravityBody;
	
	return forceBody;
}


// ... and moments on the aircraft
simdata::Vector3 const& AeroDynamics::CalculateMoments( double const p_qBarS ) const
{
	static simdata::Vector3 moments;

	moments.x = CalculatePitchMoment(p_qBarS); // Pitch
	moments.y = CalculateRollMoment(p_qBarS);  // Roll
	moments.z = CalculateYawMoment(p_qBarS);   // Yaw

	return moments;
}


double AeroDynamics::CalculateLiftCoefficient() const
{ // Cl should be calculated before Cd (IF it is used in Cd as induced drag)
	double lift_coe;
	double alpha = m_alpha;

	// prevent driving this equation from from its range of validity
	if (alpha > 0.8) alpha = 0.8;
	if (alpha < -0.8) alpha = -0.8;
	
	lift_coe = ((m_CL0) + 
	            (m_CL_a * alpha) + 
	            (m_CL_q * m_AngularVelocityBody.x + m_CL_adot * m_alphaDot ) * m_WingChord / ( 2.0 * m_Speed) + 
		    (m_CL_de * m_Elevator)
		   );

	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics::CalculateLiftCoefficient() " << 
	                                lift_coe << " at alpha = " << RadiansToDegrees(m_alpha) << 
	                                ", Elevator: " << m_Elevator );

	return lift_coe;
}


simdata::Vector3 const& AeroDynamics::LiftVector()
{
	static simdata::Vector3 lift;
	lift.x = 0.0;

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
	
	lift.y = m_CL * sin(m_alpha);
	lift.z = m_CL * cos(m_alpha);

	return lift;
}


double AeroDynamics::CalculateDragCoefficient() const
{
	static int x = 0;
	double drag_coe;
	double alpha = m_alpha;
	
	// prevent driving this equation from from its range of validity
	if (alpha > 0.8) alpha = 0.8;
	if (alpha < -0.8) alpha = -0.8;
	
	drag_coe =  m_CD0 + m_CD_a * fabs(alpha) + m_CD_de * fabs(m_Elevator);
	//drag_coe =  -0.0194 + 0.0214 * alpha + (- 0.29 * m_Elevator + 0.0033 + 0.206 * alpha) * m_Elevator + (-0.699 - 0.903 * alpha) * alpha * alpha;
	//drag_coe = - drag_coe;

	double induced = m_CD_i * m_CL * m_CL;

	//if (x++ % 1000 == 0) {
	//	printf("%6.1f %6.2f %6.2f %6.2f %6.2f\n", m_gForce, drag_coe, induced, m_alpha, m_Elevator);
	//}

	drag_coe += induced;
	
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics::CalculateDragCoefficient() " << 
	                                drag_coe << " at alpha = " << RadiansToDegrees(m_alpha) << 
	                                ", Elevator: " << m_Elevator );

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

	side_coe = m_CY_beta * m_beta + m_CY_dr * m_Rudder - m_CY_r * m_AngularVelocityBody.z;

	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics::CalculateSideCoefficient() " << side_coe );

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
	         (m_CI_p * m_AngularVelocityBody.y - m_CI_r * m_AngularVelocityBody.z)* m_WingSpan / ( 2.0 * m_Speed) +
	         (m_CI_dr * m_Rudder)
	       ) * qbarS * m_WingSpan;
}


double AeroDynamics::CalculatePitchMoment(double const qbarS) const 
{
	return ( (m_CM0) + 
	         (m_CM_a * m_alpha) +
	         (m_CM_q * m_AngularVelocityBody.x + m_CM_adot * m_alphaDot) * m_WingChord / ( 2.0 * m_Speed) +
	         (m_CM_de * m_Elevator)
	       ) * qbarS * m_WingChord;
}


double AeroDynamics::CalculateYawMoment(double const qbarS) const
{   
	return - ( (m_Cn_beta * m_beta) + 
	           (m_Cn_p * m_AngularVelocityBody.y - m_Cn_r * m_AngularVelocityBody.z) * m_WingSpan / ( 2.0 * m_Speed) +
	           (m_Cn_da * m_Aileron + m_Cn_dr * m_Rudder)
	         ) * qbarS *  m_WingSpan; // Yaw
} 


double AeroDynamics::CalculateDynamicPressure(double alt) const
{
	double qBar = CalculateAirDensity(alt) * m_Speed * m_Speed;
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics::CalculateDynamicPressure: qBar " << qBar);
	return qBar;
}


// Air density is a function of temperature, and altitude. Maybe also pressure and humidity.
// Return units in kg/m^3
double AeroDynamics::CalculateAirDensity(double const alt) const
{  
	// Mair = 28.966 10^(-3) kg / mol
	// rho0 = 1.206 : air density at 0 level
	// T0 = 288 K : air temperature at 0 level
	// R = 8.31
	// alpha = 1.4
	// coef = Mair * (1 - alpha) / (R * T0 * alpha) 
	// powerCoef = 1 / (alpha - 1) = 2.5
	// rho = rho1 * (1 + coef * g * z )^(1/(alpha - 1))
	// T1 = 217
	// rho1 = 0.3602
	// z1 = 11000
	// rho = rho1 * exp(-Mair * g / (R * T1) * (z - z1))

	double density;

	if ( alt < 11000.0 ) {
		density = 1.206 * pow(1.0 - 0.0000034616 * CalculateGravity(alt) * alt, 2.5);
	} else {
		density = 0.3602 * exp (- 0.00001606304 * CalculateGravity(alt) * (alt - 11000.0));
	}
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics: CalculateAirDensity: " << density << " at altitude: " << alt);
	return density;
}


double AeroDynamics::CalculateGravity(double const p_altitude) const
{
	// g = G * Mearth / (Rearth + z)^2
	// G = 6,67259 10^(-11)
	// Mearth = 5.976 10^(24) kg
	// Rearth = 6 378 10^3 m

	double h = 1.0 + p_altitude / 6357000.0 ;
	double gravity = 9.802480776 / ( h * h );
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics: CalculateGravity: " << gravity << " at altitude " << p_altitude);
	return gravity;
}


simdata::Vector3 AeroDynamics::LocalToBody(const simdata::Vector3 & vec )
{
	return simdata::QVRotate( qOrientation.Bar(), vec );
}


simdata::Vector3 AeroDynamics::BodyToLocal(const simdata::Vector3 & vec )
{
	return simdata::QVRotate( qOrientation, vec );
}


simdata::Matrix3 AeroDynamics::MakeAngularVelocityMatrix(simdata::Vector3 u)
{
	return simdata::Matrix3(0.0f, -u.z, u.y, u.z, 0.0f, -u.x, -u.y, u.x, 0.0f);
}


/*
void AeroDynamics::setThrottle(double setting)
{ 
	m_Throttle = setting; 
	m_Thrust = ThrottleFactor * CalculateAirDensity(m_PositionLocal.z) * m_Throttle
	           * CalculateGravity(m_PositionLocal.z) * m_ThrustMax / ( 1.0 + m_Speed );
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics::setThrottle(): Throttle: " << 
	        m_Throttle << ", Thrust: " << m_Thrust );
}


void AeroDynamics::setAileron(double aileron) 
{ 
	m_Aileron = SetControl(ControlSensibility(aileron), m_DaMax, m_DaMin);
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics::setAileron(): " << m_Aileron );
}


void AeroDynamics::setElevator(double elevator) 
{
	m_Elevator = SetControl(ControlSensibility(elevator), m_DeMax, m_DeMin);
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics::setElevator(): " << m_Elevator );
}


void AeroDynamics::setRudder( double rudder ) 
{ 
	m_Rudder = SetControl(ControlSensibility(rudder), m_DrMax, m_DrMin);
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AeroDynamics::setRudder(): " << m_Rudder );
}

double AeroDynamics::getThrust() const
{
	return m_Thrust;
}
*/

/*
void AeroDynamics::setVelocity(const simdata::Vector3 & velo) 
{ 
	m_VelocityLocal = velo; 
}
*/

double AeroDynamics::CIVbasis(double p_t) const
{
	double cIV = p_t * p_t  * ( 3.0 - 2.0 * p_t);
	return cIV;
}


double AeroDynamics::ControlInputValue(double p_gForce) const
{ // to reduce G, decrease deflection control surface
	double control = 1.0;
	double m_GMin = -3.0, m_GMax = 9.0;

	if (p_gForce > m_GMax && m_ElevatorInput > 0.0) return 0.0;
	if (p_gForce < m_GMin && m_ElevatorInput < 0.0) return 0.0;
	if ( m_alpha > m_stallAOA && m_ElevatorInput > 0.0) return 0.0;
	if ( p_gForce > m_GMax - m_depsilon && m_ElevatorInput > 0.0) {
		control = CIVbasis((p_gForce - m_GMax) / m_depsilon);
	} else {
		if ( p_gForce < m_GMin + m_depsilon && m_ElevatorInput < 0.0) {
			control = CIVbasis((m_GMin - p_gForce) / m_depsilon);
		}
	}
	
	return control;
}


void AeroDynamics::updateAngles(double h)
{
	m_alpha = - atan2( m_VelocityBody.z, m_VelocityBody.y ); 
	m_alphaDot = ( m_alpha - m_alpha0 ) / h;
	// restrict m_alphaDot in vertical stalls
	if (m_alphaDot > 1.0) m_alphaDot = 1.0;
	if (m_alphaDot < -1.0) m_alphaDot = -1.0;
	
	// Calculate side angle
	// beta is 0 when v velocity (i.e. side velocity) is 0; note here v is m_VelocityBody.x
	m_beta  = atan2(m_VelocityBody.x, m_VelocityBody.y);
}

double const damping = 0.91;

std::vector<double> const &AeroDynamics::f(double x, std::vector<double> &y)
{   
	static std::vector<double> dy(13);

    // bind(y,p,v,w,q);
	m_PositionLocal = *m_Position + BodyToLocal(simdata::Vector3(y[0],y[1],y[2]));
	m_VelocityBody = simdata::Vector3(y[3],y[4],y[5]);
	m_AngularVelocityBody = damping * simdata::Vector3(y[6],y[7],y[8]);
	qOrientation = simdata::Quaternion(y[9],y[10],y[11],y[12]);
    double mag = qOrientation.Magnitude();
	if (mag != 0.0)
		qOrientation /= mag;
	y[9]  = qOrientation.w; y[10] = qOrientation.x; y[11] = qOrientation.y; y[12] = qOrientation.z;

	// p' = v
	dy[0] = y[3]; dy[1] = y[4]; dy[2] = y[5];

	
	// pass AOA, AOA dot, side angle
	updateAngles(x);

	m_Speed = m_VelocityBody.Length();
	if (m_Speed < 1.0) m_Speed = 1.0;
	double qBarS = m_qBarFactor * m_Speed * m_Speed;

	// linear acceleration body: calculate v' = F/m - w^v
    m_ForcesBody = CalculateForces(qBarS);
    m_LinearAccelBody =  m_MassInverse * m_ForcesBody - (m_AngularVelocityBody^m_VelocityBody);
	dy[3] = m_LinearAccelBody.x; dy[4] = m_LinearAccelBody.y; dy[5] = m_LinearAccelBody.z;
	
	// angular acceleration body: calculate Iw' = M - w^Iw
	m_MomentsBody = CalculateMoments(qBarS);
	m_AngularAccelBody = m_InertiaInverse * 
				                  (m_MomentsBody - (m_AngularVelocityBody^(m_Inertia * m_AngularVelocityBody)));
	dy[6] = m_AngularAccelBody.x; dy[7] = m_AngularAccelBody.y; dy[8] = m_AngularAccelBody.z;
	
	// quaternion derivative with w in body coordinates: q' = 0.5 * q * w
	simdata::Quaternion qprim = 0.5 * qOrientation * m_AngularVelocityBody;
	dy[9]  = qprim.w; dy[10] = qprim.x; dy[11] = qprim.y; dy[12] = qprim.z;
	
	return dy;
}

std::vector<double> const &bind(simdata::Vector3 const &p,
								 simdata::Vector3 const &v,
								 simdata::Vector3 const &w,
								 simdata::Quaternion const &q)
{
	static std::vector<double> y(13);
	y[0] = p.x; y[1] = p.y; y[2] = p.z;
	y[3] = v.x; y[4] = v.y; y[5] = v.z;
	y[6] = w.x; y[7] = w.y; y[8] = w.z;
	y[9] = q.w; y[10] = q.x; y[11] = q.y; y[12] = q.z;
	return y;
}

void AeroDynamics::bindToBody(std::vector<double> const &y)
{
 m_PositionBody = simdata::Vector3(y[0],y[1],y[2]);
 m_VelocityBody = simdata::Vector3(y[3],y[4],y[5]);
 m_AngularVelocityBody = damping * simdata::Vector3(y[6],y[7],y[8]);
}
 
void AeroDynamics::BodyToLocal()
{
  qOrientation = *m_Orientation;
  m_PositionLocal = *m_Position + BodyToLocal(m_PositionBody);
  m_VelocityLocal = BodyToLocal(m_VelocityBody);
  m_AngularVelocityLocal = BodyToLocal(m_AngularVelocityBody);
}

void AeroDynamics::doSimStep2(double dt)
{  
	static std::vector<double> y0, y;

	if (dt == 0.0) dt = 0.01;
	
	m_PositionLocal = *m_Position;
	m_VelocityLocal = *m_Velocity;
	m_AngularVelocityLocal = *m_AngularVelocity;
	qOrientation = *m_Orientation;
	
	m_ElevatorScale = 0.9 * m_ElevatorScale + 0.1 * ControlInputValue( m_gForce );
	m_Elevator = (1.0 - dt) * m_Elevator  + dt * m_ElevatorInput * m_ElevatorScale;
	m_Speed = m_VelocityLocal.Length();
	// prevent singularities
	if (m_Speed < 1.0) m_Speed = 1.0;
	
	// initial conditions
	m_qBarFactor = 0.5 * CalculateAirDensity(m_PositionLocal.z) * m_WingArea;
	
	m_Gravity = CalculateGravity(m_PositionLocal.z);
	m_GravityWorld = - m_Mass * m_Gravity * simdata::Vector3::ZAXIS;
	
	m_VelocityBody = LocalToBody(m_VelocityLocal);
	m_AngularVelocityBody = LocalToBody(m_AngularVelocityLocal);
	updateAngles(dt);
	y0 = ::bind(simdata::Vector3::ZERO, m_VelocityBody, m_AngularVelocityBody, qOrientation);
	
	// solution
	y = RungeKutta(y0, 0, dt);
	
	if (y == y0) {
		std::cout << "rkqc has failed\n";
		y = rk4(y0, f(0, y0), 0, dt);
	}

	// update all variables
	// Caution: don t permute these lines
	bindToBody(y);    
	updateAngles(dt);
	m_alpha0 = m_alpha;
	BodyToLocal();
	qOrientation = simdata::Quaternion(y[9],y[10],y[11],y[12]);
	double mag = qOrientation.Magnitude();
	if (mag != 0.0)
		qOrientation /= mag;

	*m_Position = m_PositionLocal;
	*m_Velocity = m_VelocityLocal;
	*m_AngularVelocity = m_AngularVelocityLocal;
	*m_Orientation = qOrientation;	
}


/*
double AeroDynamics::SetControl(double const p_setting, double const & p_mMax, double const & p_mMin) const
{
	if ( p_setting >=0.0 ) {
		return p_setting * p_mMax;
	} else {
		return -p_setting * p_mMin;
	}
}
*/


/*
double AeroDynamics::ControlSensibility(double p_x) const
{ // smooth the joystick controls

	double z = p_x * p_x;
	double y = p_x * z * ( 2.0 - z );

	return y;
}
*/
