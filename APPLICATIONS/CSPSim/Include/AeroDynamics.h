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


#ifndef __AERODYNAMICS_H__
#define __AERODYNAMICS_H__


#include <SimData/Object.h>
#include <SimData/Types.h>
#include <SimData/InterfaceRegistry.h>

class AircraftObject;

	


/**
 * class Physics - Base class for physics computations.
 */
class Physics 
{
public:
	Physics() { }
	virtual ~Physics() {}
	
	virtual void initialize() = 0;
	virtual void doSimStep(double dt) = 0;
};

/**
 * class AeroDynamics - aircraft flight model implementation.
 *
 */
class AeroDynamics: public simdata::Object, public Physics
{
public:
	SIMDATA_OBJECT(AeroDynamics, 0, 0)

	BEGIN_SIMDATA_XML_INTERFACE(AeroDynamics)
		SIMDATA_XML("wing_span", AeroDynamics::m_WingSpan, true)
		SIMDATA_XML("wing_chord", AeroDynamics::m_WingChord, true)
		SIMDATA_XML("wing_area", AeroDynamics::m_WingArea, true)
		SIMDATA_XML("stall_aoa", AeroDynamics::m_stallAOA, true)
		SIMDATA_XML("de_max", AeroDynamics::m_DeMax, true)
		SIMDATA_XML("de_min", AeroDynamics::m_DeMin, true)
		SIMDATA_XML("da_max", AeroDynamics::m_DaMax, true)
		SIMDATA_XML("da_min", AeroDynamics::m_DaMin, true)
		SIMDATA_XML("dr_max", AeroDynamics::m_DrMax, true)
		SIMDATA_XML("dr_min", AeroDynamics::m_DrMin, true)
		SIMDATA_XML("g_min", AeroDynamics::m_GMin, true)
		SIMDATA_XML("g_max", AeroDynamics::m_GMax, true)
		SIMDATA_XML("cd0", AeroDynamics::m_CD0, true)
		SIMDATA_XML("cd_a", AeroDynamics::m_CD_a, true)
		SIMDATA_XML("cd_de", AeroDynamics::m_CD_de, true)
		SIMDATA_XML("cl0", AeroDynamics::m_CL0, true)
		SIMDATA_XML("cl_a", AeroDynamics::m_CL_a, true)
		SIMDATA_XML("cl_adot", AeroDynamics::m_CL_adot, true)
		SIMDATA_XML("cl_q", AeroDynamics::m_CL_q, true)
		SIMDATA_XML("cl_de", AeroDynamics::m_CL_de, true)
		SIMDATA_XML("cm0", AeroDynamics::m_CM0, true)
		SIMDATA_XML("cm_a", AeroDynamics::m_CM_a, true)
		SIMDATA_XML("cm_adot", AeroDynamics::m_CM_adot, true)
		SIMDATA_XML("cm_q", AeroDynamics::m_CM_q, true)
		SIMDATA_XML("cm_de", AeroDynamics::m_CM_de, true)
		SIMDATA_XML("cy_beta", AeroDynamics::m_CY_beta, true)
		SIMDATA_XML("cy_p", AeroDynamics::m_CY_p, true)
		SIMDATA_XML("cy_r", AeroDynamics::m_CY_r, true)
		SIMDATA_XML("cy_da", AeroDynamics::m_CY_da, true)
		SIMDATA_XML("cy_dr", AeroDynamics::m_CY_dr, true)
		SIMDATA_XML("ci_beta", AeroDynamics::m_CI_beta, true)
		SIMDATA_XML("ci_p", AeroDynamics::m_CI_p, true)
		SIMDATA_XML("ci_r", AeroDynamics::m_CI_r, true)
		SIMDATA_XML("ci_da", AeroDynamics::m_CI_da, true)
		SIMDATA_XML("ci_dr", AeroDynamics::m_CI_dr, true)
		SIMDATA_XML("cn_beta", AeroDynamics::m_Cn_beta, true)
		SIMDATA_XML("cn_p", AeroDynamics::m_Cn_p, true)
		SIMDATA_XML("cn_r", AeroDynamics::m_Cn_r, true)
		SIMDATA_XML("cn_da", AeroDynamics::m_Cn_da, true)
		SIMDATA_XML("cn_dr", AeroDynamics::m_Cn_dr, true)
	END_SIMDATA_XML_INTERFACE
		
	AeroDynamics();
	virtual ~AeroDynamics();
	
	virtual void initialize();
	virtual void doSimStep(double dt);

protected:

	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);
	virtual void convertXML();
	void postProcess();


protected:

	float m_WingSpan;
	double m_WingChord; // chord length        
	double m_WingArea;  // surface area of wings
	double m_stallAOA; // stall AOA 
	
	float m_DeMax;
	float m_DeMin;
	float m_DaMax;
	float m_DaMin;
	float m_DrMax;
	float m_DrMin;

	float m_GMin;
	float m_GMax;

	/**
	 * internally: X = right, Y = nose, Z = up
	 * externally: X = nose, Y = right, Z = down (for XML input)
	 */

	double m_CD0;     // CDo is the reference drag at zero angle of attack
	double m_CD_a;    // CDa is the drag curve slope
	double m_CD_de;   // CDde is the drag due to elevator
	
	double m_CL0;     // CLo is the reference lift at zero angle of attack
	double m_CL_a;    // CLa is the lift curve slope
	double m_CL_adot;
	double m_CL_q;
	double m_CL_de;   // CLde is the lift due to elevator
	
	double m_CM0;     // CMo is the pitch moment coefficient
	double m_CM_a;    // CMa is the pitch moment coefficient due to angle of attack
	double m_CM_adot;     
	double m_CM_q;    // CMq is the pitch moment coefficient due to pitch rate
	double m_CM_de;   // CMde is the pitch coefficient due to elevator 
	
	double m_CY_beta; // CLb - the dihedral effect
	double m_CY_p;    // Clp - roll damping
	double m_CY_r;    // CLdr - roll due to rudder
	double m_CY_da;   // CLr - roll due to yaw rate// Clda - roll due to aileron
	double m_CY_dr;        
	
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

public:
	
	double getAngleOfAttack() const { return m_alpha; }
	double getSideSlip() const { return m_beta; }
	double getGForce() const { return m_gForce; }
	double getSpeed() const { return m_Speed; }
	
	void setThrust(double thrust) { m_Thrust = thrust; }

	//void setPosition(const simdata::Vector3 & pos) { m_PositionLocal = pos; }
	//void setVelocity(const simdata::Vector3 & velo);
	
	void bindObject(simdata::Vector3 &position, simdata::Vector3 &velocity, 
	                simdata::Vector3 &angular_velocity, simdata::Quaternion &orientation);
	void setControlSurfaces(double aileron, double elevator, double rudder);
	void setInertia(double mass, simdata::Matrix3 const &I);
	

protected:

	simdata::Vector3 CalculateForces(double const p_qBarS); // update gforce too
	simdata::Vector3 CalculateMoments(double const p_qBarS) const;
	double CalculateLiftCoefficient() const; 
	double CalculateDragCoefficient() const;
	double CalculateSideCoefficient() const;
	double CalculateDynamicPressure(double alt) const;
	double CalculateGravity(double const p_altitude) const;
	double CalculateAirDensity(double const alt) const;
	double CalculateRollMoment(double const qbarS) const;
	double CalculatePitchMoment(double const qbarS) const;
	double CalculateYawMoment(double const qbarS) const;

	double CIVbasis(double p_t) const;
	double ControlInputValue(double p_gForce) const;

	simdata::Vector3 LocalToBody(const simdata::Vector3 & vec);
	simdata::Vector3 BodyToLocal(const simdata::Vector3 & vec);
	simdata::Matrix3 MakeAngularVelocityMatrix(simdata::Vector3 u);
	
	simdata::Vector3 LiftVector() const; 
	simdata::Vector3 DragVector() const;
	simdata::Vector3 SideVector() const;

	/*
	double ControlSensibility(double p_x) const;       // control joystick sensibility
	double ControlInputValue(double p_gForce) const;  // decrease deflection value to lower G
	double CIVbasis(double p_t) const;
	*/

	double m_depsilon;       // Gforce control feedback stall
	unsigned short m_Maxi;   // number of FM iteration
	double m_dprevdt;        // previous value of dt
	double m_ElevatorScale;  // elevator correction
	double m_ElevatorInput;  // desired elevator deflection

	//double SetControl(double const p_setting, double const & p_mMax, double const & p_mMin) const;

	AircraftObject *m_Aircraft;

	// control surfaces
	double m_Aileron;
	double m_Elevator;
	double m_Rudder;

	double m_Thrust;

	// calculated forces
	simdata::Vector3 m_ForceBody;
	simdata::Vector3 m_CurrentForceTotal;
	simdata::Vector3 m_GravityForce;
	simdata::Vector3 m_ThrustForce;
	simdata::Vector3 m_LiftForce;
	simdata::Vector3 m_DragForce;

	// derived quantities
	double m_alpha;    // angle of attack
	double m_alphaDot; // AOA rate
	double m_beta;     // side slip angle
	double m_gForce;   // current g acceleration
	
	double m_Gravity;  // current gravitational acceleration
	simdata::Vector3 m_GravityWorld;        // current gravity vector in earth coordinates

	simdata::Vector3 m_EulerAngles;    
	simdata::Quaternion qOrientation;       // orientation in earth coordinates

	simdata::Vector3 m_ForcesBody;          // total force in body coordinates
	simdata::Vector3 m_ForcesLocal;	        // total force in local coordinates
	simdata::Vector3 m_GravityBody;         // gravitational force in body coordinates

	simdata::Vector3 m_MomentsBody;         // (L,M,N) total moment (torque) in body coordinates

	simdata::Vector3 m_AngularAccelBody;    // (Pdot, Qdot, Rdot)
	simdata::Vector3 m_AngularAccelLocal;   // angular acceleration in local coordinates
	simdata::Vector3 m_LinearAccelBody;     // (Udot, Vdot, Wdot)
	simdata::Vector3 m_LinearAccelLocal;    // linuar acceleration in local coordinates

	simdata::Vector3 m_PositionLocal;       // position in earth coordinates
	simdata::Vector3 m_VelocityLocal;       // velocity in earth coordinates
	simdata::Vector3 m_VelocityBody;        // (U,V,W) velocity in body coordinates
	simdata::Vector3 m_AngularVelocityLocal;// angular velocity in earth coordinates
	simdata::Vector3 m_AngularVelocityBody; // (P,Q,R) angular velocity in body coordinates

	bool m_Bound;
	simdata::Vector3 *m_Position;
	simdata::Vector3 *m_Velocity;
	simdata::Vector3 *m_AngularVelocity;
	simdata::Quaternion *m_Orientation;

	double m_Speed;
	double m_Mass;
	double m_MassInverse;
	simdata::Matrix3 m_Inertia;        // mass moment of inertia in standard coordinates (constant)
	simdata::Matrix3 m_InertiaInverse; // inverse of moment of inertia matrix (constant)
};



#endif  // __AERODYNAMICS_H__

