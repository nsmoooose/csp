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
 * @file PrimaryAeroDynamics.cpp
 *
 **/
# if defined(_MSC_VER) && (_MSC_VER <= 1300)
#define NOMINMAX
#endif

#include <algorithm>

#include "CSPSim.h"
#include "Log.h" 
#include "PrimaryAeroDynamics.h"

#include <SimData/Math.h>
#include <SimData/Quat.h>


using simdata::RadiansToDegrees;
using simdata::DegreesToRadians;


SIMDATA_REGISTER_INTERFACE(PrimaryAeroDynamics)

PrimaryAeroDynamics::PrimaryAeroDynamics():
	m_depsilon(0.0),
	m_ElevatorScale(0.0),
	m_ElevatorInput(0.0),
	m_Aileron(0.0),
	m_Elevator(0.0),
	m_Rudder(0.0),
	m_alpha(0.0),
	m_alpha0(0.0),
	m_alphaDot(0.0),
	m_beta(0.0),
	m_gForce(1.0),
	m_AirSpeed(0.0)
	//m_Distance(0.0)
{
}

PrimaryAeroDynamics::~PrimaryAeroDynamics() {
}

void PrimaryAeroDynamics::pack(simdata::Packer& p) const {
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

void PrimaryAeroDynamics::unpack(simdata::UnPacker& p) {
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

void PrimaryAeroDynamics::convertXML() {
	// angle data are given in degree
	m_DeMax = DegreesToRadians(m_DeMax);
	m_DeMin = DegreesToRadians(m_DeMin);
	m_DaMax = DegreesToRadians(m_DaMax);
	m_DaMin = DegreesToRadians(m_DaMin);
	m_DrMax = DegreesToRadians(m_DrMax);
	m_DrMin = DegreesToRadians(m_DrMin);
	m_stallAOA = DegreesToRadians(m_stallAOA);
}

void PrimaryAeroDynamics::postCreate() {
	Object::postCreate();
	m_AspectRatio = m_WingSpan * m_WingSpan / m_WingArea;
	m_CD_i = 1.0 / (0.9 * G_PI * m_AspectRatio);
	m_HalfWingArea = 0.5 * m_WingArea;
	m_depsilon = 0.5 * std::min(m_GMax,fabs(m_GMin));
}

void PrimaryAeroDynamics::setControlSurfaces(double aileron, double elevator, double rudder) {
	m_Aileron = aileron;
	m_ElevatorInput = elevator;
	m_Elevator = elevator;
	m_Rudder = rudder;
}

void PrimaryAeroDynamics::setMassInverse(double massInverse) {
	m_MassInverse = massInverse;
}

void PrimaryAeroDynamics::initializeSimulationStep(double dt) {
	BaseDynamics::initializeSimulationStep(dt);
	//double u = (0.034 + dt)/(1.0 - atan(m_AirSpeed) / G_PI);
	double u = 0.05 + dt;
	m_ElevatorScale	= (1.0 - u) * m_ElevatorScale + u * controlInputValue( m_gForce );
	m_Elevator = m_ElevatorInput * m_ElevatorScale;
	// initial conditions

	/* XXX now handled by PhysicModel
	Atmosphere const *atmosphere = CSPSim::theSim->getAtmosphere();
	if (atmosphere)	{
		simdata::Vector3 const &p = *m_PositionLocal;
		double density = atmosphere->getDensity(p.z);
		m_qBarFactor *=	density;
		m_WindLocal	= atmosphere->getWind(p);
		m_WindLocal	+= atmosphere->getTurbulence(p, m_Distance);
	} else {
		m_qBarFactor *=	1.25;
	}
	m_Distance += m_AirSpeed * dt;
	// moved here from update(); (so less frequent updates)
	m_AirSpeed = m_AirflowBody->Length();
	// prevent singularities
	if (m_AirSpeed < 1.0) m_AirSpeed = 1.0;
	*/
}


void PrimaryAeroDynamics::computeForceAndMoment(double x) {
	m_AirflowBody =	*m_VelocityBody - *m_WindBody; //simdata::QVRotate(m_qOrientation->Bar(), m_WindLocal);
	m_AirSpeed = m_AirflowBody.length();
	// prevent singularities
	if (m_AirSpeed < 1.0) m_AirSpeed = 1.0;
	updateAngles(x);

	//////////////////////////
	// internal frame is used
	/////////////////////////

	m_qBarS = m_HalfWingArea * (*m_qBar) * m_AirSpeed * m_AirSpeed;

	m_Force = m_qBarS * liftVector();

	// Evaluate g's
	// FIXME: m_MassInverse is constant? ;-)
	m_gForce = m_MassInverse * m_Force.z / 9.81;
	// Drag and sideforce
	m_Force += m_qBarS * (dragVector() + sideVector());
	m_Moment.x = calculatePitchMoment(); // Pitch
	m_Moment.y = calculateRollMoment();  // Roll
	m_Moment.z = calculateYawMoment();   // Yaw
}

void PrimaryAeroDynamics::postSimulationStep(double dt) {
	updateAngles(dt);
	m_alpha0 = m_alpha;
}

void PrimaryAeroDynamics::calculateLiftCoefficient() { 
	// Cl should be calculated before Cd (IF it is used in Cd as induced drag)
	double alpha = m_alpha;

	// prevent driving this equation from from its range of validity
	if (alpha > 0.8) alpha = 0.8;
	if (alpha < -0.8) alpha = -0.8;
	
	m_CL = ((m_CL0) + 
	        (m_CL_a * alpha) + 
	        (m_CL_q * m_AngularVelocityBody->x + m_CL_adot * m_alphaDot ) * m_WingChord / ( 2.0 * m_AirSpeed) + 
	        (m_CL_de * m_Elevator)
	       );

	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics::calculateLiftCoefficient() " << 
	                                m_CL << " at alpha = " << RadiansToDegrees(m_alpha) << 
	                                ", Elevator: " << m_Elevator );
}

simdata::Vector3 const &PrimaryAeroDynamics::liftVector() {
	static simdata::Vector3 lift;

	calculateLiftCoefficient();

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

void PrimaryAeroDynamics::calculateDragCoefficient() {
	double alpha = m_alpha;
	
	// prevent driving this equation from from its range of validity
	if (alpha > 0.8) alpha = 0.8;
	if (alpha < -0.8) alpha = -0.8;
	
	m_CD =  m_CD0 + m_CD_a * fabs(alpha) + m_CD_de * fabs(m_Elevator);
	//drag_coe =  -0.0194 + 0.0214 * alpha + (- 0.29 * m_Elevator + 0.0033 + 0.206 * alpha) * m_Elevator + (-0.699 - 0.903 * alpha) * alpha * alpha;
	//drag_coe = - drag_coe;

	calculateGroundEffectCoefficient();

	double induced = m_GE * m_CD_i * m_CL * m_CL;
	if (induced < 0.0) induced = 0.0;

	/*
	static int x = 0;
	if (x++ % 1000 == 0) {
		printf("Cd=%6.3f I=%6.3f Cdi=%6.3f Cl=%6.3f a=%6.2f ge=%6.3f\n", m_CD, induced, m_CD_i, m_CL, m_alpha, m_GE);
	}	
	*/

	m_CD += induced;
	
	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics::calculateDragCoefficient() " << 
	                                m_CD << " at alpha = " << RadiansToDegrees(m_alpha) << 
	                                ", Elevator: " << m_Elevator << ", Induced: " << induced << ", GE: " << m_GE << ", m_CL: " << m_CL );
}

simdata::Vector3 const& PrimaryAeroDynamics::dragVector() {
	static simdata::Vector3 drag;
	
	calculateDragCoefficient();
	
	drag.x = 0.0;
	drag.y = - m_CD * cos(m_alpha);
	drag.z = m_CD * sin(m_alpha);

	return drag;
}

void PrimaryAeroDynamics::calculateSideCoefficient() {
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

	m_CY = m_CY_beta * beta + m_CY_dr * m_Rudder - m_CY_r * m_AngularVelocityBody->z;

	CSP_LOG(PHYSICS, DEBUG, "AeroDynamics::calculateSideCoefficient() " << m_CY );
}

simdata::Vector3 const&  PrimaryAeroDynamics::sideVector() {
	static simdata::Vector3 side;
		
	calculateSideCoefficient();
	
	side.x = m_CY * cos(m_beta);
	side.y = - m_CY * sin(m_beta);
	//side.x = Cs * sin(m_beta);
	//side.y = - Cs * cos(m_beta);
	side.z = 0.0;

	return side;
}

double PrimaryAeroDynamics::calculateRollMoment() const {
	return ( (m_CI_beta * m_beta) + 
	         (m_CI_da * m_Aileron) +
	         (m_CI_p * m_AngularVelocityBody->y - m_CI_r * m_AngularVelocityBody->z)* m_WingSpan / ( 2.0 * m_AirSpeed) +
	         (m_CI_dr * m_Rudder)
	       ) * m_qBarS * m_WingSpan;
}


double PrimaryAeroDynamics::calculatePitchMoment() const {
	return ( (m_CM0) + 
	         (m_CM_a * m_alpha) +
	         (m_CM_q * m_AngularVelocityBody->x + m_CM_adot * m_alphaDot) * m_WingChord / ( 2.0 * m_AirSpeed) +
	         (m_CM_de * m_Elevator)
	       ) * m_qBarS * m_WingChord;
}


double PrimaryAeroDynamics::calculateYawMoment() const {   
	return - ( (m_Cn_beta * m_beta) + 
	           (m_Cn_p * m_AngularVelocityBody->y - m_Cn_r * m_AngularVelocityBody->z) * m_WingSpan / ( 2.0 * m_AirSpeed) +
	           (m_Cn_da * m_Aileron + m_Cn_dr * m_Rudder)
	         ) * m_qBarS *  m_WingSpan; // Yaw
} 

double PrimaryAeroDynamics::controlIVbasis(double p_t) const {
	double cIV = p_t * p_t  * ( 3.0 - 2.0 * p_t);
	return cIV;
}

double PrimaryAeroDynamics::controlInputValue(double p_gForce) const { 
	// to reduce G, decrease deflection control surface
	if (p_gForce > m_GMax && m_ElevatorInput > 0.0) return 0.0;
	if (p_gForce < m_GMin && m_ElevatorInput < 0.0) return 0.0;
	if (m_alpha > m_stallAOA && m_ElevatorInput > 0.0) return 0.0;
	if (p_gForce > m_GMax - m_depsilon && m_ElevatorInput > 0.0) {
		return controlIVbasis((m_GMax - p_gForce) / m_depsilon);
	} 
	if ( p_gForce < m_GMin + m_depsilon && m_ElevatorInput < 0.0) {
		return controlIVbasis((p_gForce - m_GMin) / m_depsilon);
	}
	return 1.0;
}

void PrimaryAeroDynamics::updateAngles(double h) {
	m_alpha = - atan2( m_AirflowBody.z, m_AirflowBody.y ); 
	if (h > 0.0) {
		m_alphaDot = ( m_alpha - m_alpha0 ) / h;
	} // else keep previous value

	// restrict m_alphaDot in vertical stalls
	if (m_alphaDot > 1.0) m_alphaDot = 1.0;
	if (m_alphaDot < -1.0) m_alphaDot = -1.0;
	
	// Calculate side angle
	// beta is 0 when v velocity (i.e. side velocity) is 0; note here v is m_AirflowBody.x
	m_beta  = atan2(m_AirflowBody.x, m_AirflowBody.y);
}

void PrimaryAeroDynamics::calculateGroundEffectCoefficient() {
// approximate (generic) ground effect... TODO: paramaterize in xml.
	m_GE = 1.0;
	if (*m_NearGround) {
		double h = *m_Height; // + wing height relative to cg
		h /= m_WingSpan;
		if (h > 1.0) 
			h = 1.0;
		m_GE = 0.49 + (1.0 - 0.5 * h) * h;
	}
}





