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
 * @file AirplanePhysics.h
 *
 **/


#ifndef __AIRPLANEPHYSICS_H__
#define __AIRPLANEPHYSICS_H__

#include "TypesMath.h"
#include "AeroParam.h"
#include "BasePhysics.h"



/**
 * class AirplanePhysics - Describe me!
 *
 * @author unknown
 */
class AirplanePhysics : public BasePhysics
{
public:
    AirplanePhysics();
	
    virtual void Initialize();
    virtual void DoSimStep(double dt);

	void setAeroParams(AeroParam * pAeroParam );
	
	
public:
	
    void setThrottle(double setting);
	void setSpeed(double p_speed) {m_fSpeedLocal = p_speed;};
	double getAngleOfAttack() const { return m_falpha; }
    double getThrust() const;
    void setThrust(double thrust) { m_Thrust = thrust; }
    void setAileron(double aileron);
    void setElevator(double elevator);
    void setRudder( double rudder );
	
    void setPosition(const StandardVector3 & pos) { m_PositionLocal = pos; }
    void setVelocity(const StandardVector3 & velo);
	
	StandardVector3 CalculateForces( double const p_qBarS ); // update gforce too
	StandardVector3 CalculateMoments( double const p_qBarS ) const;
    double CalculateLiftCoefficient() const; 
    double CalculateDragCoefficient() const;
	double CalculateSideCoefficient() const;
    double CalculateDynamicPressure(double alt) const;
    double CalculateGravity(double const p_altitude) const;
    double CalculateAirDensity(double const alt) const;
    double CalculateRollMoment(double const qbarS) const;
    double CalculatePitchMoment(double const qbarS) const;
    double CalculateYawMoment(double const qbarS) const;
	
    StandardVector3 LocalToBody(const StandardVector3 & vec );
    StandardVector3 BodyToLocal(const StandardVector3 & vec );
	
    StandardMatrix3 MakeAngularVelocityMatrix(StandardVector3 u);
	
	
	double ThrottleFactor;
	
	double m_falpha;    // angle of attack
	double m_falphaDot; // AOA rate
	double m_fbeta;     // side slip angle
	double m_fstallAOA; // stall AOA 
	
	double m_Thrust;
	double m_Aileron;
	double m_Elevator;
	double m_Rudder;
	double m_Throttle;
	
	StandardVector3 m_PositionLocal;       // position in earth coordinates
	StandardVector3 m_VelocityLocal;       // velocity in earth coordinates
	StandardVector3 m_AngularAccelLocal;
	StandardVector3 m_AngularVelocityLocal;
	StandardVector3 m_LinearAccelLocal;
	double m_fSpeedLocal;               // speed (magnitude of the velocity)
	StandardVector3 m_VelocityBody;    // (U,V,W) velocity in body coordinates
	StandardVector3 m_ForcesBody;      // total force on body
	StandardVector3 m_ForcesLocal;
	double m_fgForce;                   // current g acceleration
	StandardVector3 m_LinearAccel;     // (Udot, Vdot, Wdot)
	StandardVector3 m_GravityBody;
	StandardVector3 m_VelocityAngular; // (P,Q,R) angular velocity in body coordinates
	StandardVector3 m_angularAccel;    // (Pdot, Qdot, Rdot)
	StandardVector3 m_Moments;         // (L,M,N) total moment (torque) on body
	
	StandardVector3 m_EulerAngles;    
	StandardQuaternion	qOrientation; // orientation in earth coordinates

	double m_fWingSpan;
	double m_fWingChord; // chord length        
	double m_fWingArea;  // surface area of wings
	
	double m_fDeMax;
	double m_fDeMin;
	double m_fDaMax;
	double m_fDaMin;
	double m_fDrMax;
	double m_fDrMin;

	double m_fGMin;
	double m_fGMax;
	
	double m_fMass;                    // total mass (constant)
	double m_fMassInverse;
	double m_fI_XX;
	double m_fI_YY;
	double m_fI_ZZ;
	double m_fI_XZ;
	StandardMatrix3 m_Inertia;		  // mass moment of inertia in body coordinates (constant)
	StandardMatrix3 m_InertiaInverse; // inverse of mass moment of inertia matrix	(constant)
	
	double m_fThrustMax;
	
	double m_fCD0;     // CDo is the reference drag at zero angle of attack
	double m_fCD_a;    // CDa is the drag curve slope
	double m_fCD_de;   // CDde is the drag due to elevator
	
	double m_fCL0;     // CLo is the reference lift at zero angle of attack
	double m_fCL_a;    // CLa is the lift curve slope
	double m_fCL_adot;
	double m_fCL_q;
	double m_fCL_de;   // CLde is the lift due to elevator
	
	double m_fCM0;     // CMo is the pitch moment coefficient
	double m_fCM_a;    // CMa is the pitch moment coefficient due to angle of attack
	double m_fCM_adot;     
	double m_fCM_q;    // CMq is the pitch moment coefficient due to pitch rate
	double m_fCM_de;   // CMde is the pitch coefficient due to elevator 
	
	double m_fCY_beta; // CLb - the dihedral effect
	double m_fCY_p;    // Clp - roll damping
	double m_fCY_r;    // CLdr - roll due to rudder
	double m_fCY_da;   // CLr - roll due to yaw rate// Clda - roll due to aileron
	double m_fCY_dr;        
	
	double m_fCI_beta;       
	double m_fCI_p;          
	double m_fCI_r;          
	double m_fCI_da;         
	double m_fCI_dr;         

	double m_fCn_beta;   // CNb is the weather cocking stability
	double m_fCn_p;      // CNp is the rudder adverse yaw
	double m_fCn_r;      // CNr is the yaw damping
	double m_fCn_da;     // CNda is the yaw due to aileron
	double m_fCn_dr;     // CNdr is the yaw due to rudder
 protected:
	 StandardVector3 LiftVector() const; 
	 StandardVector3 DragVector() const;
	 StandardVector3 SideVector() const;

	 double ControlSensibility(double p_x) const;       // control joystick sensibility
	 double ControlInputValue(double p_gForce) const;  // decrease deflection value to lower G
	 double CIVbasis(double p_t) const;
	 double SetControl(double const p_setting, double const & p_mMax, double const & p_mMin) const;

	 double m_depsilon;       // Gforce control feedback stall
	 unsigned short m_usMaxi; // number of FM iteration
	 double m_dprevdt;        // previous value of dt
};

#endif // __AIRPLANEPHYSICS_H__