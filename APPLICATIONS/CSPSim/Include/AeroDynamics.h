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

#include "LandingGear.h"
#include "DynamicalSystem.h"

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

class AeroVectorField;

/**
 * class AeroDynamics - aircraft flight model implementation.
 *
 */
class AeroDynamics: public simdata::Object, public Physics, protected DynamicalSystem
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
	virtual void doSimStep2(double dt);

protected:

	virtual void pack(simdata::Packer& p) const;
	virtual void unpack(simdata::UnPacker& p);
	virtual void convertXML();
	virtual void postCreate();


protected:

    //vector field which determines trajectory and behaviour of this dynamical system
	std::vector<double> const& _f(double x, std::vector<double>& y);

	float m_WingSpan;
	double m_WingChord; // chord length        
	double m_WingArea;  // surface area of wings
	double m_stallAOA;  // stall AOA 
	
	float m_DeMax;
	float m_DeMin;
	float m_DaMax;
	float m_DaMin;
	float m_DrMax;
	float m_DrMin;

	// the folowing parameters are both structural and controlled in nature
	float m_GMin; // min number of G that this aircraft model can support (in general < 0)
	float m_GMax; // max  ...                                             (in general > 3)

	/**
	 * internally: X = right, Y = nose, Z = up
	 * externally: X = nose, Y = right, Z = down (for XML input)
	 */

	double m_CD0;     // CDo is the reference drag at zero angle of attack
	double m_CD_a;    // CDa is the drag curve slope
	double m_CD_de;   // CDde is the drag due to elevator
	double m_CD_i;    // CD_i is induced drag coefficient = 1 / (e * pi * lambda) where lamdba = Span^2 / Area  
	                  // and e is Osswald coefficient
	
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
	double getSpeed() const { return m_AirSpeed; }
	
	void setThrust(double thrust) { m_Thrust = thrust; }
	void setGroundZ(double groundz) { m_GroundZ = groundz; }
	void setGroundN(simdata::Vector3 const &groundn) { m_GroundN = groundn; }

	//void setPosition(const simdata::Vector3 & pos) { m_PositionLocal = pos; }
	//void setVelocity(const simdata::Vector3 & velo);
	
	void bindObject(simdata::Vector3 &position, simdata::Vector3 &velocity, 
	                simdata::Vector3 &angular_velocity, simdata::Quaternion &orientation);
	void bindGearSet(LandingGearSet &);
	void bindContacts(std::vector<simdata::Vector3> const &);
	void setControlSurfaces(double aileron, double elevator, double rudder);
	void setInertia(double mass, simdata::Matrix3 const &I);
	void setBoundingRadius(double radius);
	

protected:
	// FIXME: needs to be moved in a mother class
	bool isNearGround();


	std::vector<double> const &f(double x, std::vector<double> &y);
	void bindToBody(std::vector<double> const &y);

	simdata::Vector3 const& CalculateForces(double const p_qBarS); // update gforce too
	simdata::Vector3 const& CalculateMoments(double const p_qBarS);
	void updateAngles(double dt);
	double CalculateLiftCoefficient() const; 
	double CalculateDragCoefficient() const;
	double CalculateSideCoefficient() const;
	double CalculateRollMoment(double const qbarS) const;
	double CalculatePitchMoment(double const qbarS) const;
	double CalculateYawMoment(double const qbarS) const;

	double CIVbasis(double p_t) const;
	double ControlInputValue(double p_gForce) const;

	simdata::Vector3 LocalToBody(const simdata::Vector3 & vec);
	simdata::Vector3 BodyToLocal(const simdata::Vector3 & vec);
	void BodyToLocal();
	
	simdata::Vector3 const& LiftVector(); 
	simdata::Vector3 const& DragVector() const;
	simdata::Vector3 const& SideVector() const;

	double m_depsilon;       // Gforce control feedback stall
	unsigned short m_Maxi;   // number of FM iteration
	double m_dprevdt;        // previous value of dt
	double m_ElevatorScale;  // elevator correction
	double m_ElevatorInput;  // desired elevator deflection

	// control surfaces
	double m_Aileron;
	double m_Elevator;
	double m_Rudder;

	double m_Thrust;

	simdata::Vector3 m_CurrentForceTotal;
	simdata::Vector3 m_GravityForce;
	simdata::Vector3 m_ThrustForce;
	simdata::Vector3 m_LiftForce;
	simdata::Vector3 m_DragForce;

	// derived quantities
	double m_alpha;    // current angle of attack
	double m_alpha0;   // discrete AOA
	double m_alphaDot; // AOA rate
	double m_beta;     // side slip angle
	double m_gForce;   // current g acceleration
	
	double m_qBarFactor;                    // 0.5 * WingSpan * density (updated 1 time in a simulation step)
	double m_Gravity;                       // current gravitational acceleration
	simdata::Vector3 m_GravityWorld;        // current gravity vector in earth coordinates

	simdata::Vector3 m_EulerAngles;    
	simdata::Quaternion qOrientation;       // orientation in earth coordinates

	simdata::Vector3 m_ForcesBody;          // total force in body coordinates
	simdata::Vector3 m_ForcesLocal;	        // total force in local coordinates
	simdata::Vector3 m_GravityBody;         // gravitational force in body coordinates

	simdata::Vector3 m_MomentsBody;         // (L,M,N) total moment (torque) in body coordinates

	simdata::Vector3 m_PositionBody;        // position after integration step in body coordinates
	simdata::Vector3 m_AngularAccelBody;    // (Pdot, Qdot, Rdot)
	simdata::Vector3 m_AngularAccelLocal;   // angular acceleration in local coordinates
	simdata::Vector3 m_LinearAccelBody;     // (Udot, Vdot, Wdot)
	simdata::Vector3 m_LinearAccelLocal;    // linuar acceleration in local coordinates

	simdata::Vector3 m_PositionLocal;       // position in earth coordinates
	simdata::Vector3 m_VelocityLocal;       // velocity in earth coordinates
	simdata::Vector3 m_VelocityBody;        // (U,V,W) velocity in body coordinates
	simdata::Vector3 m_AirflowBody;         // air flow velocity in body coordinate
	simdata::Vector3 m_WindLocal;           // wind velocity in local coordinates
	simdata::Vector3 m_AngularVelocityLocal;// angular velocity in earth coordinates
	simdata::Vector3 m_AngularVelocityBody; // (P,Q,R) angular velocity in body coordinates

	simdata::Vector3 m_ExtraForceBody;      // Landing gear, etc 
	simdata::Vector3 m_ExtraMomentBody;     // Landing gear, etc

	bool m_Bound;
	bool m_NearGround;
	simdata::Vector3 *m_Position;
	simdata::Vector3 *m_Velocity;
	simdata::Vector3 *m_AngularVelocity;
	simdata::Quaternion *m_Orientation;

	double m_Distance;
	double m_AirSpeed;
	double m_Mass;
	double m_MassInverse;
	double m_AspectRatio;
	mutable double m_Cl;
	double m_GE;                       // fractional ground effect
	simdata::Matrix3 m_Inertia;        // mass moment of inertia in standard coordinates (constant)
	simdata::Matrix3 m_InertiaInverse; // inverse of moment of inertia matrix (constant)

	LandingGearSet *m_Gear;                                // landing gear
	const std::vector<simdata::Vector3> *m_Contacts;       // other contact points
	double m_Radius;                                       // bounding sphere radius
	double m_GroundZ;                                      // ground position
	simdata::Vector3 m_GroundN;                            // ground normal
};



#endif  // __AERODYNAMICS_H__

