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
 * @file FlightModel.cpp
 *
 **/

/*
	TODO

	change control surface deflection sense to match lit. convensions

*/

# if defined(_MSC_VER) && (_MSC_VER <= 1300)
#define NOMINMAX
#endif

#include <algorithm>

#include "CSPSim.h"
#include "Log.h" 
#include "FlightModel.h"
#include "FlightDynamics.h"

#include <SimData/Math.h>
#include <SimData/Quaternion.h>


using simdata::RadiansToDegrees;
using simdata::DegreesToRadians;


SIMDATA_REGISTER_INTERFACE(FlightModel)

FlightModel::FlightModel()
{
}

FlightModel::~FlightModel() {
}

void FlightModel::pack(simdata::Packer& p) const {
	Object::pack(p);
	
	p.pack(m_WingSpan);
	p.pack(m_WingChord);
	p.pack(m_WingArea);
	p.pack(m_stallAOA);
	
	p.pack(m_CD0);
	p.pack(m_CD_a);
	p.pack(m_CD_de);
	p.pack(m_CD_db);
	
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

void FlightModel::unpack(simdata::UnPacker& p) {
	Object::unpack(p);

	p.unpack(m_WingSpan);
	p.unpack(m_WingChord);
	p.unpack(m_WingArea);
	p.unpack(m_stallAOA);
	
	p.unpack(m_CD0);
	p.unpack(m_CD_a);
	p.unpack(m_CD_de);
	p.unpack(m_CD_db);
	
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

void FlightModel::convertXML() {
	// angle data are given in degree
	m_stallAOA = DegreesToRadians(m_stallAOA);
}

void FlightModel::postCreate() {
	Object::postCreate();
	m_AspectRatio = m_WingSpan * m_WingSpan / m_WingArea;
	m_CD_i = 1.0 / (0.9 * G_PI * m_AspectRatio);
	m_HalfWingArea = 0.5 * m_WingArea;
}

FlightDynamics *FlightModel::newFlightDynamics() {
	FlightDynamics *fd = new FlightDynamics;
	fd->setFlightModel(this);
	return fd;
}

simdata::Vector3 FlightModel::calculateLiftVector() { 

	m_CL = ((m_CL0) + 
	        (m_CL_a * m_Alpha) + 
	        (m_CL_q * m_AngularVelocityBody.x + m_CL_adot * m_AlphaDot ) * m_WingChord * m_Inv2V + 
	        (m_CL_de * m_Elevator)
	       );

	if (fabs(m_Alpha) > m_stallAOA) {
		double c = fabs(m_Alpha) - m_stallAOA;
		m_CL = std::max(0.0, m_CL - c * c * 3.0);
	}

	double CL = m_CL * m_GE;

	return simdata::Vector3(0.0, CL * sin(m_Alpha), CL * cos(m_Alpha));
}

/*
okay... you'll want to have tables for e and you'll also want to make CD0 into CDmin and have induced drag as 
(CL - CLmin,drag)^2/pi*e*AR.  So you'll have e for each CL you define and a two constant values (for constant Mach number) 
for CDmin and CLmin,drag.  You'd probably want to make AR variable too in case you decide to model swing wing aircraft.  
You could even model the effects of wintip missiles. :-) */

simdata::Vector3 FlightModel::calculateDragVector() {

	double CL = m_CL - 0.04; //m_CL_md;

	m_CD =  m_CD0 + \
	        m_CD_de * fabs(m_Elevator) + \
	        m_CD_db * fabs(m_Airbrake) + \
		m_CD_i * CL * CL;

	return simdata::Vector3(0.0, -m_CD * cos(m_Alpha), m_CD * sin(m_Alpha));
}

simdata::Vector3 FlightModel::calculateSideVector() {

	m_CY = m_CY_beta * m_Beta + m_CY_dr * m_Rudder - m_CY_r * m_AngularVelocityBody.z;

	return simdata::Vector3(m_CY * cos(m_Beta), m_CY * sin(m_Beta), 0.0);
}


simdata::Vector3 FlightModel::calculateForce() {
	return (calculateLiftVector() + calculateDragVector() + calculateSideVector()) * m_qBarS;
}

simdata::Vector3 FlightModel::calculateMoment() {
	simdata::Vector3 moment;
	moment.x = calculatePitchMoment(); 
	moment.y = calculateRollMoment(); 
	moment.z = calculateYawMoment(); 
	return moment;
}

double FlightModel::calculateRollMoment() const {
	return ( (m_CI_beta * m_Beta) + 
	         (m_CI_da * m_Aileron) +
	         (m_CI_p * m_AngularVelocityBody.y - m_CI_r * m_AngularVelocityBody.z)* m_WingSpan * m_Inv2V +
	         (m_CI_dr * m_Rudder)
	       ) * m_qBarS * m_WingSpan;
}

double FlightModel::calculatePitchMoment() const {
	return ( (m_CM0) + 
	         (m_CM_a * m_Alpha) +
	         (m_CM_q * m_AngularVelocityBody.x + m_CM_adot * m_AlphaDot) * m_WingChord * m_Inv2V +
	         (m_CM_de * m_Elevator)
	       ) * m_qBarS * m_WingChord;
}


double FlightModel::calculateYawMoment() const {   
	return - ( (m_Cn_beta * m_Beta) + 
	           (m_Cn_p * m_AngularVelocityBody.y - m_Cn_r * m_AngularVelocityBody.z) * m_WingSpan * m_Inv2V +
	           (m_Cn_da * m_Aileron + m_Cn_dr * m_Rudder)
	         ) * m_qBarS *  m_WingSpan; 
} 


void FlightModel::updateGroundEffect(double height) {
	// TODO paramaterize in xml.
	if (height < m_WingSpan) {
		double x = std::max(0.0, height / m_WingSpan) + 0.1;
		//m_GE = 0.49 + (1.0 - 0.5 * height) * height;
		m_GE = 0.9835 + 0.020 / x / x;
	} else {
		m_GE = 1.0;
	}
}





