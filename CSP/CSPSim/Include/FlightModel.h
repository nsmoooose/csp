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
 * @file FlightModel.h
 *
 **/


#ifndef __FLIGHTMODEL_H__
#define __FLIGHTMODEL_H__


#include <SimData/LUT.h>
#include <SimData/Math.h>
#include <SimData/Object.h>
#include <SimData/Vector3.h>

#include "Atmosphere.h"
#include "CSPSim.h"

/**
 * class FlightModel
 *
 * An aircraft flight model based on stability derivatives.  FlightModel
 * instances are shared by all aircraft of a given type.  Each aircraft has its
 * own FlightDynamics instance that stores state information and drives the
 * FlightModel.  To pass parameters more efficiently, the FlightDynamics
 * instance sets a number of temporary state variables within the FlightModel,
 * then performs a number of computations using these variables.  Hence, the
 * FlightModel methods are not suitable for multithreaded environments in which
 * more than one aircraft is simulated at a time.
 *
 */
class FlightModel: public simdata::Object {
public:
	SIMDATA_DECLARE_OBJECT(FlightModel)

	FlightModel();
	virtual ~FlightModel();

	inline void setAirstream(double alpha,
	                         double alphaDot,
	                         double beta,
	                         double airspeed,
	                         double qBar) {
		// prevent driving the model outside its range of validity
		m_Alpha = simdata::clampTo(alpha,-0.8, 0.8);
		m_Alpha_float = static_cast<float>(m_Alpha);
		m_AlphaDot = alphaDot;
		m_Beta = beta;
		m_Beta_float = static_cast<float>(m_Beta);
		m_AirSpeed = airspeed;
		m_qBarS = m_HalfWingArea * 2.0 * qBar;
		m_Inv2V = 0.5 / std::max(0.5, airspeed);
	}

	inline void setControlSurfaces(double aileron,
	                               double elevator,
	                               double rudder,
	                               double airbrake) {
		m_Aileron = aileron;
		m_Elevator = elevator;
		m_Rudder = rudder;
		m_Airbrake = airbrake;
	}

	inline void setKinetics(simdata::Vector3 const &angular_velocity_body, double height) {
		m_AngularVelocityBody = angular_velocity_body;
		const Atmosphere* atmosphere = CSPSim::theSim->getAtmosphere();
		m_AirSpeedM = static_cast<float>(atmosphere->getMach(m_AirSpeed, height));
		updateGroundEffect(height);
	}

	simdata::Vector3 calculateForce();
	simdata::Vector3 calculateMoment();

protected:

	virtual void convertXML();
	virtual void postCreate();

	double m_WingSpan;
	double m_WingChord;		// chord length
	double m_WingArea;		// surface area of wings
	double m_stallAOA;		// stall AOA
	
	/**
	 * internally: X = right, Y = nose, Z = up
	 * externally: X = nose, Y = right, Z = down (for XML input)
	 */

	double m_CD0;     // CDo is the reference drag at zero angle of attack
	double m_CD_a;    // CDa is the drag curve slope
	double m_CD_de;   // CDde is the drag due to elevator
	double m_CD_db;   // CDdb is the drag due to the airbrake(s)
	simdata::Table2 m_CD_m_a;
	
	double m_CL0;     // CLo is the reference lift at zero angle of attack
	double m_CL_a;    // CLa is the lift curve slope
	simdata::Table1 m_CL_adot;
	double m_CL_q;
	double m_CL_de;   // CLde is the lift due to elevator
	simdata::Table2 m_CL_m_a; // lift curve vs (mach,aoa)
	
	double m_CM0;     // CMo is the pitch moment coefficient
	double m_CM_a;    // CMa is the pitch moment coefficient due to angle of attack
	double m_CM_adot;
	double m_CM_q;    // CMq is the pitch moment coefficient due to pitch rate
	double m_CM_de;   // CMde is the pitch coefficient due to elevator
	simdata::Table2 m_CY_m_a;
	simdata::Table1 m_Delta_CM_de;
	
	double m_CY_beta; // CLb - the dihedral effect
	double m_CY_p;    // Clp - roll damping
	double m_CY_r;    // CLr - roll due to yaw rate
	double m_CY_da;   // Clda - roll due to aileron
	double m_CY_dr;   // CLdr - roll due to rudder
	
	double m_CI_beta;
	double m_CI_p;
	double m_CI_r;
	double m_CI_da;
	double m_CI_dr;

	double m_Cn_beta;   // CNb is the weather cocking stability
	double m_Cn_p;      // CNp is the rudder adverse yaw
	double m_Cn_r;      // CNr is the yaw damping
	double m_Cn_da;     // CNda is the yaw due to aileron
	double m_Cn_dr;     // CNdr is the yaw due to rudder

	void updateAngles(double dt);

	simdata::Vector3 calculateLiftVector();
	simdata::Vector3 calculateDragVector();
	simdata::Vector3 calculateSideVector();

	double calculateRollMoment() const;
	double calculatePitchMoment() const;
	double calculateYawMoment() const;

	// compute the ground effect coefficient for use in setAirstream()
	void updateGroundEffect(double height);
	
	// derived quantities
	double m_HalfWingArea;
	double m_AspectRatio;
	double m_CD_i;    // CD_i is induced drag coefficient = 1 / (e * pi * lambda)
	                  // where lamdba = Span^2 / Area and e is Osswald coefficient
	
	// shared intermediate values
	double m_CL;
	double m_CD;
	double m_CY;

	// airstream parameters
	double m_Alpha;
	float m_Alpha_float;
	double m_AlphaDot;
	double m_Beta;
	float m_Beta_float;
	double m_qBarS;
	double m_Inv2V;
	double m_GE;
	double m_AirSpeed;
	float m_AirSpeedM;

	// control surfaces
	double m_Aileron;
	double m_Elevator;
	double m_Rudder;
	double m_Airbrake;

	// kinetics
	simdata::Vector3 m_AngularVelocityBody;
	
};


#endif  // __FLIGHTMODEL_H__

