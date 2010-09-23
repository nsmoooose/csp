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
 * @file F16FlightModel.h
 *
 **/


#ifndef __CSPSIM_F16_FLIGHTMODEL_H__
#define __CSPSIM_F16_FLIGHTMODEL_H__

#include <csp/cspsim/FlightModel.h>
#include <csp/csplib/data/LUT.h>

namespace csp {

/**
 * class F16FlightModel
 *
 * An aircraft flight model based on NASA low speed wind tunnel data
 * for the F-16, and extended with high speed performance data computed
 * by Raptor One and Mav-JP.
 *
 */
class F16FlightModel: public FlightModel {
public:
	CSP_DECLARE_OBJECT(F16FlightModel)

	F16FlightModel();
	virtual void calculateForceAndMoment(Vector3 &force, Vector3 &moment);

protected:

	double calculateForceCoefficientX();
	double calculateForceCoefficientY();
	double calculateForceCoefficientZ();

	double calculateRollMoment() const;
	double calculatePitchMoment() const;
	double calculateYawMoment() const;

	/**
	 * internally: X = right, Y = nose, Z = up
	 * externally: X = nose, Y = right, Z = down (for XML input)
	 */

	// xml interface parameters
	Table3 m_CX_de_a_b;
	Table2 m_CX_lef_a_b;
	Table1 m_CX_q_a;
	Table1 m_CX_q_lef_a;
	Table1 m_CX_sb_a;
	Table1 m_CX_tef_a;

	Table3 m_CZ_de_a_b;
	Table2 m_CZ_lef_a_b;
	Table1 m_CZ_q_a;
	Table1 m_CZ_q_lef_a;
	Table1 m_CZ_sb_a;
	Table1 m_CZ_tef_a;

	Table2 m_CD_m_a; /** drag scaling at high speed */
	Table2 m_CL_m_a; /** lift scaling at high speed */
	Table1 m_CL_x_m; /** center of lift offset vs mach */

	Table1 m_CM_q_a;
	Table1 m_CM_sb_a;
	Table1 m_CM_tef_a;
	Table2 m_CM_de_a;
	Table1 m_Delta_CM_de;

	Table2 m_CY_a_b;
	Table2 m_CY_lef_a_b;
	Table2 m_CY_dr_a_b;
	Table2 m_CY_da_a_b;
	Table2 m_CY_da_lef_a_b;
	Table1 m_CY_m;
	Table1 m_CY_p_a;
	Table1 m_CY_r_a;
	Table1 m_CY_p_lef_a;
	Table1 m_CY_r_lef_a;

	Table3 m_CI_de_a_b;
	Table2 m_CI_da_a_b;
	Table2 m_CI_dr_a_b;
	Table1 m_CI_p_a;
	Table1 m_CI_r_a;

	Table1 m_Cn_p_a;
	Table1 m_Cn_p_lef_a;
	Table1 m_Cn_r_a;
	Table1 m_Cn_r_lef_a;
	Table2 m_Cn_da_a_b;
	Table2 m_Cn_dr_a_b;
	Table3 m_Cn_de_a_b;
	Table2 m_Cn_lef_a_b;
	Table2 m_Cn_da_lef_a_b;

	// shared intermediate values
	double m_CX;
	double m_CY;
	double m_CZ;
	double m_ScaleLEF;
};

} // namespace csp

#endif  // __F16_FLIGHTMODEL_H__

