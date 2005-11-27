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
 * @file StabilityFlightModel.h
 *
 **/


#ifndef __CSPSIM_STABILITYFLIGHTMODEL_H__
#define __CSPSIM_STABILITYFLIGHTMODEL_H__


#include <FlightModel.h>
#include <csp/lib/data/LUT.h>

/**
 * StabilityFlightModel implements an  aircraft flight model based on stability
 * derivatives.
 */
class StabilityFlightModel: public FlightModel {
public:
	SIMDATA_DECLARE_OBJECT(StabilityFlightModel)

	StabilityFlightModel();

	virtual void calculateForceAndMoment(simdata::Vector3 &force, simdata::Vector3 &moment);

protected:
	virtual void convertXML();

	simdata::Vector3 calculateLiftVector();
	simdata::Vector3 calculateDragVector();
	simdata::Vector3 calculateSideVector();

	double calculateRollMoment() const;
	double calculatePitchMoment() const;
	double calculateYawMoment() const;

	// xml interface parameters

	double m_stallAOA;  // stall angle of attack

	simdata::Table2 m_CD_m_a;  // CD_m_a is the drag coefficient as a function of mach and alpha
	double m_CD_de;   // CD_de is the drag due to elevator
	double m_CD_db;   // CD_db is the drag due to the airbrake(s)
	double m_CD_i;    // induced drag coefficient 1 / (e * PI * lambda)
	                  // where e = Osswald coeff and lambda = span^2 / area

	simdata::Table2 m_CL_m_a;  // CL_m_a is the lift coefficient as a function of mach and alpha
	double m_CL_adot;
	double m_CL_q;
	double m_CL_de;   // CL_de is the lift due to elevator

	double m_CY_b;    // CY_beta
	double m_CY_p;    // CY_p
	double m_CY_r;    // CY_r
	double m_CY_da;   // CY_da
	double m_CY_dr;   // CY_dr

	double m_Cm_0;    // CM_o is the pitch moment coefficient
	double m_Cm_a;    // CM_a is the pitch moment coefficient due to angle of attack
	double m_Cm_adot;
	double m_Cm_q;    // CMq is the pitch moment coefficient due to pitch rate
	double m_Cm_de;   // CMde is the pitch coefficient due to elevator
	
	double m_Ci_beta; // Ci_beta - the dihedral effect
	double m_Ci_p;    // Ci_p - roll damping
	double m_Ci_r;    // Ci_r - roll due to yaw rate
	double m_Ci_da;   // Ci_da - roll due to aileron
	double m_Ci_dr;   // Ci_dr - roll due to rudder

	double m_Cn_beta;   // CN_beta is the weather cocking stability
	double m_Cn_p;      // CN_p is the rudder adverse yaw
	double m_Cn_r;      // CN_r is the yaw damping
	double m_Cn_da;     // CN_da is the yaw due to aileron
	double m_Cn_dr;     // CN_dr is the yaw due to rudder

	// shared intermediate values
	double m_CL;
	double m_CD;
	double m_CY;
};


#endif  // __STABILITYFLIGHTMODEL_H__

