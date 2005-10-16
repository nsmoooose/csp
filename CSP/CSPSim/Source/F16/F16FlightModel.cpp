// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file F16FlightModel.cpp
 *
 **/

/*
   The internal sign conventions match the NASA 1979 paper except for the
   coordinate axes: NASA-X = y, NASA-Y = x, NASA-Z = -z.  Note the sign
   difference for the yaw axis!
*/

#include "F16/F16FlightModel.h"

#include <SimData/Math.h>
#include <SimData/ObjectInterface.h>


using simdata::toDegrees;
using simdata::toRadians;


SIMDATA_XML_BEGIN(F16FlightModel)
	SIMDATA_DEF("cd_m_a", m_CD_m_a, true)
	SIMDATA_DEF("cl_m_a", m_CL_m_a, true)
	SIMDATA_DEF("cl_x_m", m_CL_x_m, true)

	SIMDATA_DEF("cx_de_a_b", m_CX_de_a_b, true)
	SIMDATA_DEF("cx_lef_a_b", m_CX_lef_a_b, true)
	SIMDATA_DEF("cx_q_a", m_CX_q_a, true)
	SIMDATA_DEF("cx_q_lef_a", m_CX_q_lef_a, true)
	SIMDATA_DEF("cx_sb_a", m_CX_sb_a, true)
	SIMDATA_DEF("cx_tef_a", m_CX_tef_a, true)

	SIMDATA_DEF("cz_de_a_b", m_CZ_de_a_b, true)
	SIMDATA_DEF("cz_lef_a_b", m_CZ_lef_a_b, true)
	SIMDATA_DEF("cz_q_a", m_CZ_q_a, true)
	SIMDATA_DEF("cz_q_lef_a", m_CZ_q_lef_a, true)
	SIMDATA_DEF("cz_sb_a", m_CZ_sb_a, true)
	SIMDATA_DEF("cz_tef_a", m_CZ_tef_a, true)

	SIMDATA_DEF("cm_q_a", m_CM_q_a, true)
	SIMDATA_DEF("cm_de_a", m_CM_de_a, true)
	SIMDATA_DEF("cm_sb_a", m_CM_sb_a, true)
	SIMDATA_DEF("cm_tef_a", m_CM_tef_a, true)

	SIMDATA_DEF("cy_m", m_CY_m, true)
	SIMDATA_DEF("cy_p_a", m_CY_p_a, true)
	SIMDATA_DEF("cy_r_a", m_CY_r_a, true)
	SIMDATA_DEF("cy_p_lef_a", m_CY_p_lef_a, true)
	SIMDATA_DEF("cy_r_lef_a", m_CY_r_lef_a, true)
	SIMDATA_DEF("cy_a_b", m_CY_a_b, true)
	SIMDATA_DEF("cy_lef_a_b", m_CY_lef_a_b, true)
	SIMDATA_DEF("cy_dr_a_b", m_CY_dr_a_b, true)
	SIMDATA_DEF("cy_da_a_b", m_CY_da_a_b, true)
	SIMDATA_DEF("cy_da_lef_a_b", m_CY_da_lef_a_b, true)

	SIMDATA_DEF("ci_de_a_b", m_CI_de_a_b, true)
	SIMDATA_DEF("ci_da_a_b", m_CI_da_a_b, true)
	SIMDATA_DEF("ci_dr_a_b", m_CI_dr_a_b, true)
	SIMDATA_DEF("ci_p_a", m_CI_p_a, true)
	SIMDATA_DEF("ci_r_a", m_CI_r_a, true)

	SIMDATA_DEF("cn_r_a", m_Cn_r_a, true)
	SIMDATA_DEF("cn_r_lef_a", m_Cn_r_lef_a, true)
	SIMDATA_DEF("cn_p_a", m_Cn_p_a, true)
	SIMDATA_DEF("cn_p_lef_a", m_Cn_p_lef_a, true)
	SIMDATA_DEF("cn_dr_a_b", m_Cn_dr_a_b, true)
	SIMDATA_DEF("cn_da_a_b", m_Cn_da_a_b, true)
	SIMDATA_DEF("cn_de_a_b", m_Cn_de_a_b, true)
	SIMDATA_DEF("cn_da_lef_a_b", m_Cn_da_lef_a_b, true)
	SIMDATA_DEF("cn_lef_a_b", m_Cn_lef_a_b, true)
SIMDATA_XML_END


F16FlightModel::F16FlightModel(): m_ScaleLEF(0.0) {
}

double F16FlightModel::calculateForceCoefficientX() {
	const double q = m_AngularVelocityBody.x();
	m_CX = m_CX_de_a_b[m_Elevator][m_Alpha][m_Beta] + m_CX_lef_a_b[m_Alpha][m_Beta] * m_ScaleLEF;
	m_CX += (m_CX_q_a[m_Alpha] + m_CX_q_lef_a[m_Alpha] * m_ScaleLEF) * m_WingChord * m_Inv2V * q;
	m_CX += m_CX_sb_a[m_Alpha] * m_Airbrake;
	m_CX += m_CX_tef_a[m_Alpha] * m_TrailingEdgeFlap;
	return m_CX;
}

double F16FlightModel::calculateForceCoefficientY() {
	// negative since yaw axis (and hence yaw rate) is opposite nasa 1979.
	const double r = -m_AngularVelocityBody.z();
	const double p = m_AngularVelocityBody.y();
	const double cy_r = m_CY_r_a[m_Alpha] + m_CY_r_lef_a[m_Alpha] * m_ScaleLEF;
	const double cy_p = m_CY_p_a[m_Alpha] + m_CY_p_lef_a[m_Alpha] * m_ScaleLEF;
	const double cy_da = m_CY_da_a_b[m_Alpha][m_Beta] + m_CY_da_lef_a_b[m_Alpha][m_Beta] * m_ScaleLEF;
	m_CY = m_CY_a_b[m_Alpha][m_Beta] + m_CY_lef_a_b[m_Alpha][m_Beta] * m_ScaleLEF;
	m_CY += m_CY_dr_a_b[m_Alpha][m_Beta] * m_Rudder;
	m_CY += cy_da * m_Aileron;
	m_CY += (cy_r * r + cy_p * p) * m_WingSpan * m_Inv2V;
	m_CY *= m_CY_m[m_Mach];
	return m_CY;
}

double F16FlightModel::calculateForceCoefficientZ() {
	const double q = m_AngularVelocityBody.x();
	m_CZ = m_CZ_de_a_b[m_Elevator][m_Alpha][m_Beta] + m_CZ_lef_a_b[m_Alpha][m_Beta] * m_ScaleLEF;
	m_CZ += (m_CZ_q_a[m_Alpha] + m_CZ_q_lef_a[m_Alpha] * m_ScaleLEF) * m_WingChord * m_Inv2V * q;
	m_CZ += m_CZ_sb_a[m_Alpha] * m_Airbrake;
	m_CZ += m_CZ_tef_a[m_Alpha] * m_TrailingEdgeFlap;
	return m_CZ;
}

double F16FlightModel::calculatePitchMoment() const {
	double moment = (
		m_CM_de_a[m_Elevator][m_Alpha] +
		m_CM_sb_a[m_Alpha] * m_Airbrake +
		m_CM_tef_a[m_Alpha] * m_TrailingEdgeFlap +
		m_CM_q_a[m_Alpha] * m_AngularVelocityBody.x() * m_WingChord * m_Inv2V) * m_WingChord;

	double offset_y = m_CenterOfMassBody.y() - m_CL_x_m[m_Mach];
	moment += offset_y * m_CZ / m_WingChord;

	// TODO reduce elevator authority in ground effect
	return moment * m_qBarS * m_WingChord;
}

double F16FlightModel::calculateRollMoment() const {
	// negative since yaw axis (and hence yaw rate) is opposite nasa 1979.
	const double r = -m_AngularVelocityBody.z();
	const double p = m_AngularVelocityBody.y();

	// note that these offsets are in internal body coordinates, not fm coordinates.
	const double offset_x = m_CenterOfMassBody.x();
	const double offset_z = m_CenterOfMassBody.z();  // need additional offset?

	double moment =
		m_CI_de_a_b[m_Elevator][m_Alpha][m_Beta]
		+ m_CI_da_a_b[m_Alpha][m_Aileron > 0 ? m_Beta : -m_Beta] * m_Aileron
		+ m_CI_dr_a_b[m_Alpha][m_Rudder > 0 ? m_Beta : -m_Beta] * m_Rudder
		+ (m_CI_p_a[m_Alpha] * p + m_CI_r_a[m_Alpha] * r) * m_WingSpan * m_Inv2V
		+ (offset_z * m_CY - offset_x * m_CZ) / m_WingSpan;

	return moment * m_qBarS * m_WingSpan;
}

double F16FlightModel::calculateYawMoment() const {
	const double p = -m_AngularVelocityBody.y();
	// negative since yaw axis (and hence yaw rate) is opposite nasa 1979.
	const double r = -m_AngularVelocityBody.z();

	// cn_da = dcn,da + dcn,da,lef * (1 - dlef/25)
	double cn_da = m_Cn_da_a_b[m_Alpha][(m_Aileron > 0) ? m_Beta : -m_Beta];
	cn_da += m_ScaleLEF * (m_Cn_da_lef_a_b[m_Alpha][(m_Aileron > 0) ? m_Beta : -m_Beta] - cn_da);

	double cn_p = (m_Cn_p_a[m_Alpha] + m_Cn_p_lef_a[m_Alpha] * m_ScaleLEF) * p;
	double cn_r = (m_Cn_r_a[m_Alpha] + m_Cn_r_lef_a[m_Alpha] * m_ScaleLEF) * r;
	double cn_dr = m_Cn_dr_a_b[m_Alpha][(m_Rudder > 0) ? m_Beta : -m_Beta];

	// XXX hack (testing spin control in deep stall trim)  without this "boost" the yaw rate in
	// a deep stall is about 45 deg/sec, compared with about 20 deg/sec cited in the nasa paper.
	// this could be due to differences between the nasa data and the data used in the fm.  For
	// convenience, the fm data was taken from the "non-linear f-16 simulation using simulink
	// and matlab" package in predigitized form.  This data should be identical to the nasa data,
	// but in practice there are small variations.
	cn_dr *= simdata::clampTo((simdata::toDegrees(m_Alpha) - 40) * 0.8, 1.0, 20.0);
	cn_dr = -fabs(cn_dr);

	// note that these offsets are in internal body coordinates, not fm coordinates.
	double offset_y = m_CenterOfMassBody.y() - m_CL_x_m[0.0];  // should this vary with mach?
	double offset_x = m_CenterOfMassBody.x();

	double moment =
		m_Cn_de_a_b[m_Elevator][m_Alpha][m_Beta] +
		m_Cn_lef_a_b[m_Alpha][m_Beta] * m_ScaleLEF +
		(cn_p + cn_r) * m_WingSpan * m_Inv2V +
		cn_da * m_Aileron +
		cn_dr * m_Rudder +
		(offset_x * m_CX - offset_y * m_CY) / m_WingSpan;

	// negative since yaw axis (and hence yaw moment) is opposite nasa 1979.
	return -moment * m_qBarS * m_WingSpan;
}

void F16FlightModel::calculateForceAndMoment(simdata::Vector3 &force, simdata::Vector3 &moment) {
	m_ScaleLEF = 1.0 - m_LeadingEdgeFlap * simdata::toRadians(1.0/25.0);

	simdata::Vector3 drag_direction(sin(m_Beta), -cos(m_Beta)*cos(m_Alpha), cos(m_Beta)*sin(m_Alpha));
	simdata::Vector3 lift_direction = (drag_direction ^ simdata::Vector3::ZAXIS) ^ drag_direction;

	// rotate from nasa to internal coordinate system
	simdata::Vector3 force_c(calculateForceCoefficientY(), calculateForceCoefficientX(), -calculateForceCoefficientZ());

	// scale nasa low-speed drag by hffm drag normalized to 1.0 at low speed.
	double CD = force_c * drag_direction;
	double mach_drag_factor = m_CD_m_a[m_Mach][m_Alpha] - 1.0;
	force_c += drag_direction * (CD * mach_drag_factor);

	// blend from nasa lift to hffm lift at high speed (M > 0.5), neglecting beta and de
	// at high speed.  use a cubic function to create a smooth 0-1 transition from M 0.2
	// to M 0.5.
	double CL = force_c * lift_direction;
	double mach_lift_value = m_CL_m_a[m_Mach][m_Alpha];
	double blend = 0.0;
	if (m_Mach > 0.5) {
		blend = 1.0;
	} else if (m_Mach > 0.2) {
		double x = m_Mach - 0.2;
		blend = (33.333 - 74.074 * x) * x *x;
	}
	force_c += lift_direction * ((mach_lift_value - CL) * blend);

	// add in ground effect if necessary.  we operate only on the low speed lift, but
	// the difference should not matter under normal flight conditions!
	if (m_GE > 1.0) {
		force_c += lift_direction * (CL * (m_GE - 1.0));
	}

	force = force_c * m_qBarS;
	moment.set(calculatePitchMoment(), calculateRollMoment(), calculateYawMoment());
}

