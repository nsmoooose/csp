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
 * @file FlightModel.cpp
 *
 **/

/*
	TODO

	change control surface deflection sense to match lit. convensions

*/

#include "CSPSim.h"
#include "FlightModel.h"

#include <SimCore/Util/Log.h>
#include <SimData/Math.h>
#include <SimData/ObjectInterface.h>
#include <SimData/Quat.h>


using simdata::toDegrees;
using simdata::toRadians;


SIMDATA_XML_BEGIN(FlightModel)
	SIMDATA_DEF("wing_span", m_WingSpan, true)
	SIMDATA_DEF("wing_chord", m_WingChord, true)
	SIMDATA_DEF("wing_area", m_WingArea, true)
	SIMDATA_DEF("stall_aoa", m_stallAOA, true)

	SIMDATA_DEF("cd0", m_CD0, true)
	SIMDATA_DEF("cd_a", m_CD_a, true)
	SIMDATA_DEF("cd_de", m_CD_de, true)
	SIMDATA_DEF("cd_db", m_CD_db, true)
	SIMDATA_DEF("cd_m_a", m_CD_m_a, false)

	SIMDATA_DEF("cl0", m_CL0, true)
	SIMDATA_DEF("cl_a", m_CL_a, true)
	SIMDATA_DEF("cl_adot", m_CL_adot, true)
	SIMDATA_DEF("cl_q", m_CL_q, true)
	SIMDATA_DEF("cl_de", m_CL_de, true)
	SIMDATA_DEF("cl_m_a", m_CL_m_a, false)

	SIMDATA_DEF("cm0", m_CM0, true)
	SIMDATA_DEF("cm_a", m_CM_a, true)
	SIMDATA_DEF("cm_adot", m_CM_adot, true)
	SIMDATA_DEF("cm_q", m_CM_q, true)
	SIMDATA_DEF("cm_de", m_CM_de, true)
	SIMDATA_DEF("delta_cm_de", m_Delta_CM_de,false)

	SIMDATA_DEF("cy_beta", m_CY_beta, true)
	SIMDATA_DEF("cy_p", m_CY_p, true)
	SIMDATA_DEF("cy_r", m_CY_r, true)
	SIMDATA_DEF("cy_da", m_CY_da, true)
	SIMDATA_DEF("cy_dr", m_CY_dr, true)
	SIMDATA_DEF("cy_m_a", m_CY_m_a, false)

	SIMDATA_DEF("ci_beta", m_CI_beta, true)
	SIMDATA_DEF("ci_p", m_CI_p, true)
	SIMDATA_DEF("ci_r", m_CI_r, true)
	SIMDATA_DEF("ci_da", m_CI_da, true)
	SIMDATA_DEF("ci_dr", m_CI_dr, true)
	SIMDATA_DEF("cn_beta", m_Cn_beta, true)
	SIMDATA_DEF("cn_p", m_Cn_p, true)
	SIMDATA_DEF("cn_r", m_Cn_r, true)
	SIMDATA_DEF("cn_da", m_Cn_da, true)
	SIMDATA_DEF("cn_dr", m_Cn_dr, true)
SIMDATA_XML_END


FlightModel::FlightModel() {
}

FlightModel::~FlightModel() {
}

void FlightModel::convertXML() {
	// angle data are given in degree
	m_stallAOA = toRadians(m_stallAOA);
}

void FlightModel::postCreate() {
	Object::postCreate();
	m_AspectRatio = m_WingSpan * m_WingSpan / m_WingArea;
	m_CD_i = 1.0 / (0.9 * simdata::PI * m_AspectRatio);
	m_HalfWingArea = 0.5 * m_WingArea;
	m_AirSpeedM = 1.0f;
	m_Alpha_float = 0.0f;
}


simdata::Vector3 FlightModel::calculateLiftVector() {

	//m_CL = ((m_CL0) +
	//        (m_CL_a * m_Alpha) +
	//        (m_CL_q * m_AngularVelocityBody.x() + m_CL_adot * m_AlphaDot ) * m_WingChord * m_Inv2V +
	//        (m_CL_de * m_Elevator)
	//       );

	m_CL = m_CL_m_a[m_AirSpeedM][m_Alpha_float]
	     + (m_CL_q * m_AngularVelocityBody.x() +  m_CL_adot[m_AirSpeedM] * m_AlphaDot) * m_WingChord * m_Inv2V
	     +   m_CL_de * m_Elevator;

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

	/*
	m_CD =  m_CD0 + \
	        m_CD_de * fabs(m_Elevator) + \
	        m_CD_db * fabs(m_Airbrake) + \
	        m_CD_i * CL * CL;
	*/

	m_CD =  m_CD_m_a[m_AirSpeedM][m_Alpha_float]
	     +  m_CD_de * fabs(m_Elevator) + \
	        m_CD_db * fabs(m_Airbrake) + \
		m_CD_i * CL * CL;

	return simdata::Vector3(0.0, -m_CD * cos(m_Alpha), m_CD * sin(m_Alpha));
}

simdata::Vector3 FlightModel::calculateSideVector() {

	m_CY = m_CY_m_a[m_AirSpeedM][m_Alpha_float]*m_Beta + m_CY_dr * m_Rudder - m_CY_r * m_AngularVelocityBody.z();

	return simdata::Vector3(m_CY * cos(m_Beta), - m_CY * sin(m_Beta), 0.0);
}


simdata::Vector3 FlightModel::calculateForce() {
	return (calculateLiftVector() + calculateDragVector() + calculateSideVector()) * m_qBarS;
}

simdata::Vector3 FlightModel::calculateMoment() {
	simdata::Vector3 moment;
	moment.x() = calculatePitchMoment();
	moment.y() = calculateRollMoment();
	moment.z() = calculateYawMoment();
	return moment;
}

double FlightModel::calculateRollMoment() const {
	return ( (m_CI_beta * m_Beta) +
	         (m_CI_da * m_Aileron) +
	         (m_CI_p * m_AngularVelocityBody.y() - m_CI_r * m_AngularVelocityBody.z())* m_WingSpan * m_Inv2V +
	         (m_CI_dr * m_Rudder)
	       ) * m_qBarS * m_WingSpan;
}

double FlightModel::calculatePitchMoment() const {
	return ( (m_CM0) +
	         (m_CM_a * m_Alpha) +
	         (m_CM_q * m_AngularVelocityBody.x() + m_CM_adot * m_AlphaDot) * m_WingChord * m_Inv2V +
	         (m_CM_de + m_Delta_CM_de[m_AirSpeedM]) * m_Elevator
	       ) * m_qBarS * m_WingChord;
}


double FlightModel::calculateYawMoment() const {
	return - ( (m_Cn_beta * m_Beta) +
	           (m_Cn_p * m_AngularVelocityBody.y() - m_Cn_r * m_AngularVelocityBody.z()) * m_WingSpan * m_Inv2V +
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




