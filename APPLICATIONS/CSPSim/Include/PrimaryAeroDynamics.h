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
 * @file AeroDynamics.h
 *
 **/


#ifndef __PRIMARYAERODYNAMICS_H__
#define __PRIMARYAERODYNAMICS_H__

#include <SimData/InterfaceRegistry.h>

#include "BaseDynamics.h"

/**
 * class PrimaryAeroDynamics - aircraft primary flight model implementation.
 *
 */
class PrimaryAeroDynamics: public simdata::Object, public BaseDynamics {                
public:
	SIMDATA_OBJECT(PrimaryAeroDynamics, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(PrimaryAeroDynamics)
		SIMDATA_XML("wing_span", PrimaryAeroDynamics::m_WingSpan, true)
		SIMDATA_XML("wing_chord", PrimaryAeroDynamics::m_WingChord, true)
		SIMDATA_XML("wing_area", PrimaryAeroDynamics::m_WingArea, true)
		SIMDATA_XML("stall_aoa", PrimaryAeroDynamics::m_stallAOA, true)
		SIMDATA_XML("de_max", PrimaryAeroDynamics::m_DeMax, true)
		SIMDATA_XML("de_min", PrimaryAeroDynamics::m_DeMin, true)
		SIMDATA_XML("da_max", PrimaryAeroDynamics::m_DaMax, true)
		SIMDATA_XML("da_min", PrimaryAeroDynamics::m_DaMin, true)
		SIMDATA_XML("dr_max", PrimaryAeroDynamics::m_DrMax, true)
		SIMDATA_XML("dr_min", PrimaryAeroDynamics::m_DrMin, true)
		SIMDATA_XML("g_min", PrimaryAeroDynamics::m_GMin, true)
		SIMDATA_XML("g_max", PrimaryAeroDynamics::m_GMax, true)
		SIMDATA_XML("cd0", PrimaryAeroDynamics::m_CD0, true)
		SIMDATA_XML("cd_a", PrimaryAeroDynamics::m_CD_a, true)
		SIMDATA_XML("cd_de", PrimaryAeroDynamics::m_CD_de, true)
		SIMDATA_XML("cl0", PrimaryAeroDynamics::m_CL0, true)
		SIMDATA_XML("cl_a", PrimaryAeroDynamics::m_CL_a, true)
		SIMDATA_XML("cl_adot", PrimaryAeroDynamics::m_CL_adot, true)
		SIMDATA_XML("cl_q", PrimaryAeroDynamics::m_CL_q, true)
		SIMDATA_XML("cl_de", PrimaryAeroDynamics::m_CL_de, true)
		SIMDATA_XML("cm0", PrimaryAeroDynamics::m_CM0, true)
		SIMDATA_XML("cm_a", PrimaryAeroDynamics::m_CM_a, true)
		SIMDATA_XML("cm_adot", PrimaryAeroDynamics::m_CM_adot, true)
		SIMDATA_XML("cm_q", PrimaryAeroDynamics::m_CM_q, true)
		SIMDATA_XML("cm_de", PrimaryAeroDynamics::m_CM_de, true)
		SIMDATA_XML("cy_beta", PrimaryAeroDynamics::m_CY_beta, true)
		SIMDATA_XML("cy_p", PrimaryAeroDynamics::m_CY_p, true)
		SIMDATA_XML("cy_r", PrimaryAeroDynamics::m_CY_r, true)
		SIMDATA_XML("cy_da", PrimaryAeroDynamics::m_CY_da, true)
		SIMDATA_XML("cy_dr", PrimaryAeroDynamics::m_CY_dr, true)
		SIMDATA_XML("ci_beta", PrimaryAeroDynamics::m_CI_beta, true)
		SIMDATA_XML("ci_p", PrimaryAeroDynamics::m_CI_p, true)
		SIMDATA_XML("ci_r", PrimaryAeroDynamics::m_CI_r, true)
		SIMDATA_XML("ci_da", PrimaryAeroDynamics::m_CI_da, true)
		SIMDATA_XML("ci_dr", PrimaryAeroDynamics::m_CI_dr, true)
		SIMDATA_XML("cn_beta", PrimaryAeroDynamics::m_Cn_beta, true)
		SIMDATA_XML("cn_p", PrimaryAeroDynamics::m_Cn_p, true)
		SIMDATA_XML("cn_r", PrimaryAeroDynamics::m_Cn_r, true)
		SIMDATA_XML("cn_da", PrimaryAeroDynamics::m_Cn_da, true)
		SIMDATA_XML("cn_dr", PrimaryAeroDynamics::m_Cn_dr, true)
	END_SIMDATA_XML_INTERFACE
		
	PrimaryAeroDynamics();
	virtual ~PrimaryAeroDynamics();

	void computeForceAndMoment(double x);
	void initializeSimulationStep(double dt);
	void postSimulationStep(double dt);

	double getAngleOfAttack() const { return m_alpha; }
	double getSideSlip() const { return m_beta; }
	double getGForce() const { return m_gForce; }
	double getSpeed() const { return m_AirSpeed; }

	void setControlSurfaces(double aileron, double elevator, double rudder);
	void setMassInverse(double massInverse);

protected:

	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);
	virtual void convertXML();
	virtual void postCreate();

	float m_WingSpan;
	double m_WingChord;		// chord length        
	double m_WingArea;		// surface area of wings
	double m_HalfWingArea;
	double m_stallAOA;		// stall AOA 
	double m_AspectRatio;
	
	double m_DeMax;
	double m_DeMin;
	double m_DaMax;
	double m_DaMin;
	double m_DrMax;
	double m_DrMin;

	// the folowing parameters are both structural and controlled in nature
	double m_GMin; // min number of G that this aircraft model can support (in general < 0)
	double m_GMax; // max  ...                                             (in general > 3)

	/**
	 * internally: X = right, Y = nose, Z = up
	 * externally: X = nose, Y = right, Z = down (for XML input)
	 */

	double m_CD0;     // CDo is the reference drag at zero angle of attack
	double m_CD_a;    // CDa is the drag curve slope
	double m_CD_de;   // CDde is the drag due to elevator
	double m_CD_i;    // CD_i is induced drag coefficient = 1 / (e * pi * lambda) where lamdba = Span^2 / Area  
	                  // and e is Osswald coefficient
	double m_CD; 
	
	double m_CL0;     // CLo is the reference lift at zero angle of attack
	double m_CL_a;    // CLa is the lift curve slope
	double m_CL_adot;
	double m_CL_q;
	double m_CL_de;   // CLde is the lift due to elevator
	double m_CL;      // total lift coefficient; used by induced drag  = CD_i * Cl^2
	
	double m_CM0;     // CMo is the pitch moment coefficient
	double m_CM_a;    // CMa is the pitch moment coefficient due to angle of attack
	double m_CM_adot;     
	double m_CM_q;    // CMq is the pitch moment coefficient due to pitch rate
	double m_CM_de;   // CMde is the pitch coefficient due to elevator 
	
	double m_CY_beta; // CLb - the dihedral effect
	double m_CY_p;    // Clp - roll damping
	double m_CY_r;    // CLr - roll due to yaw rate
	double m_CY_da;   // Clda - roll due to aileron
	double m_CY_dr;   // CLdr - roll due to rudder   
	double m_CY;
	
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

	double m_GE;        // drag reduction factor due to ground effect

	void updateAngles(double dt);
	void calculateLiftCoefficient(); 
	void calculateDragCoefficient();
	void calculateSideCoefficient();
	void calculateGroundEffectCoefficient();
	double calculateRollMoment() const;
	double calculatePitchMoment() const;
	double calculateYawMoment() const;

	double controlIVbasis(double p_t) const;
	double controlInputValue(double p_gForce) const;
	
	simdata::Vector3 const& liftVector(); 
	simdata::Vector3 const& dragVector();
	simdata::Vector3 const& sideVector();

	double m_depsilon;       // G-force control feedback stall
	double m_ElevatorScale;  // elevator correction
	double m_ElevatorInput;  // desired elevator deflection

	// control surfaces
	double m_Aileron;
	double m_Elevator;
	double m_Rudder;

	// derived quantities
	double m_alpha;		// current angle of attack
	double m_alpha0;	// discrete AOA
	double m_alphaDot;	// AOA rate
	double m_beta;		// side slip angle
	double m_gForce;	// current g acceleration
	
	double m_qBarS;		// 0.5 * WingSpan * density * V^2 
	
	double m_MassInverse;

	double m_AirSpeed;
	simdata::Vector3 m_AirflowBody;         // air flow velocity in body coordinate
};


#endif  // __PRIMARYAERODYNAMICS_H__

