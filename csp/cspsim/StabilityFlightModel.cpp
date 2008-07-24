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
 * @file StabilityFlightModel.cpp
 *
 **/

#include <csp/cspsim/StabilityFlightModel.h>

#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/Quat.h>

namespace csp {

CSP_XML_BEGIN(StabilityFlightModel)
	CSP_DEF("stall_aoa", m_stallAOA, true)

	CSP_DEF("cd_m_a", m_CD_m_a, true)
	CSP_DEF("cd_de", m_CD_de, true)
	CSP_DEF("cd_db", m_CD_db, false)

	CSP_DEF("cl_m_a", m_CL_m_a, true)
	CSP_DEF("cl_adot", m_CL_adot, false)
	CSP_DEF("cl_q", m_CL_q, true)
	CSP_DEF("cl_de", m_CL_de, true)

	CSP_DEF("cy_b", m_CY_b, true)
	CSP_DEF("cy_p", m_CY_p, true)
	CSP_DEF("cy_r", m_CY_r, true)
	CSP_DEF("cy_da", m_CY_da, true)
	CSP_DEF("cy_dr", m_CY_dr, true)

	CSP_DEF("cm_0", m_Cm_0, true)
	CSP_DEF("cm_a", m_Cm_a, true)
	CSP_DEF("cm_adot", m_Cm_adot, false)
	CSP_DEF("cm_q", m_Cm_q, true)
	CSP_DEF("cm_de", m_Cm_de, true)

	CSP_DEF("ci_b", m_Ci_beta, true)
	CSP_DEF("ci_p", m_Ci_p, true)
	CSP_DEF("ci_r", m_Ci_r, true)
	CSP_DEF("ci_da", m_Ci_da, true)
	CSP_DEF("ci_dr", m_Ci_dr, true)

	CSP_DEF("cn_b", m_Cn_beta, true)
	CSP_DEF("cn_p", m_Cn_p, true)
	CSP_DEF("cn_r", m_Cn_r, true)
	CSP_DEF("cn_da", m_Cn_da, true)
	CSP_DEF("cn_dr", m_Cn_dr, true)
CSP_XML_END


StabilityFlightModel::StabilityFlightModel(): m_stallAOA(0.0), m_CD_db(0.0), m_CD_i(0.0), m_CL_adot(0.0), m_Cm_adot(0.0) {
}

void StabilityFlightModel::convertXML() {
	FlightModel::convertXML();
	m_stallAOA = toRadians(m_stallAOA); // stall angle is specified in degrees
}

Vector3 StabilityFlightModel::calculateLiftVector() {
	double q = m_AngularVelocityBody.x();

	m_CL =
		(m_CL_m_a[m_Mach][m_Alpha]) +
		(m_CL_q * q +  m_CL_adot * m_AlphaDot) * m_WingChord * m_Inv2V +
		(m_CL_de * m_Elevator);

	if (m_stallAOA > 0) {
		if (m_Alpha > m_stallAOA) {
			double c = m_Alpha - m_stallAOA;
			m_CL = std::max(0.0, m_CL - c * c * 3.0);
		} else if (m_Alpha < -m_stallAOA) {
			double c = m_Alpha + m_stallAOA;
			m_CL = std::min(0.0, m_CL + c * c * 3.0);
		}
	}

	double CL = m_CL * m_GE;
	return Vector3(0.0, CL * sin(m_Alpha), CL * cos(m_Alpha));
}

/*
okay... you'll want to have tables for e and you'll also want to make CD0 into CDmin and have induced drag as
(CL - CLmin,drag)^2/pi*e*AR.  So you'll have e for each CL you define and a two constant values (for constant Mach number)
for CDmin and CLmin,drag.  You'd probably want to make AR variable too in case you decide to model swing wing aircraft.
You could even model the effects of wintip missiles. :-) */

Vector3 StabilityFlightModel::calculateDragVector() {
	double CL = m_CL - 0.04; //m_CL_md;

	m_CD =
		(m_CD_m_a[m_Mach][m_Alpha]) +
		(m_CD_de * fabs(m_Elevator)) +
		(m_CD_db * fabs(m_Airbrake)) +
		(m_CD_i * CL * CL);

	return Vector3(0.0, -m_CD * cos(m_Alpha), m_CD * sin(m_Alpha));
}

Vector3 StabilityFlightModel::calculateSideVector() {
	double r = -m_AngularVelocityBody.z();
	m_CY =
		(m_CY_b * m_Beta) +
		(m_CY_dr * m_Rudder) +
		(m_CY_r * r);
	return Vector3(m_CY * cos(m_Beta), - m_CY * sin(m_Beta), 0.0);
}

double StabilityFlightModel::calculateRollMoment() const {
	double p = m_AngularVelocityBody.y();
	double r = -m_AngularVelocityBody.z();
	return (
		(m_Ci_beta * m_Beta) +
		(m_Ci_da * m_Aileron) +
		(m_Ci_p * p + m_Ci_r * r)* m_WingSpan * m_Inv2V +
		(m_Ci_dr * m_Rudder)
	) * m_qBarS * m_WingSpan;
}

double StabilityFlightModel::calculatePitchMoment() const {
	double q = m_AngularVelocityBody.x();
	return (
		(m_Cm_0) +
		(m_Cm_a * sin(m_Alpha)) +
		(m_Cm_q * q + m_Cm_adot * m_AlphaDot) * m_WingChord * m_Inv2V +
		(m_Cm_de * m_Elevator)
	) * m_qBarS * m_WingChord;
}

double StabilityFlightModel::calculateYawMoment() const {
	double p = m_AngularVelocityBody.y();
	double r = -m_AngularVelocityBody.z();
	return - (
		(m_Cn_beta * sin(m_Beta)) +
		(m_Cn_p * p + m_Cn_r * r) * m_WingSpan * m_Inv2V +
		(m_Cn_da * m_Aileron + m_Cn_dr * m_Rudder)
	) * m_qBarS *  m_WingSpan;
}

void StabilityFlightModel::calculateForceAndMoment(Vector3 &force, Vector3 &moment) {
	force = (calculateLiftVector() + calculateDragVector() + calculateSideVector()) * m_qBarS;
	moment.set(calculatePitchMoment(), calculateRollMoment(), calculateYawMoment());
}

} // namespace csp

