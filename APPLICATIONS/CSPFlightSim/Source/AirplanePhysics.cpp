#include "stdinc.h"


#include "AirplanePhysics.h"
#include "SimTime.h"
#include "SymbolTable.h"
#include "BaseSymbol.h"
#include "AeroParamSymbol.h"
#include "TypesMath.h"


extern SimTime * g_pSimTime;
extern SymbolTable GlobalSymbolTable;

AirplanePhysics::AirplanePhysics()
{	
    m_Elevator = 0.0;
    m_Aileron = 0.0;
    m_Rudder = 0.0;
    m_Thrust = 0.0;

	m_depsilon = 0.0;
	
	ThrottleFactor = 256;
}

void AirplanePhysics::Initialize()
{
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: Initialize()...");

	qOrientation = StandardQuaternion(1,0,0,0);

	m_AngularAccelLocal = StandardVector3::ZERO; 
	m_AngularVelocityLocal = StandardVector3::ZERO;
	m_LinearAccelLocal = StandardVector3::ZERO;

    m_VelocityAngular = StandardVector3::ZERO; // angular velocity in body coordinates
	m_angularAccel = StandardVector3::ZERO;
	m_LinearAccel = StandardVector3::ZERO;

	m_falpha = 0.0;
	m_falphaDot = 0.0;
   
    m_ForcesBody = StandardVector3::ZERO;
	m_fgForce = 1.0;
	m_Moments = StandardVector3::ZERO;

	m_usMaxi = 150;
	m_dprevdt = 0.016;
}

void AirplanePhysics::setAeroParams(AeroParam * pAeroParam )
{
	if (pAeroParam)
	{
		m_fWingSpan = pAeroParam->m_fWingSpan;
		m_fWingChord = pAeroParam->m_fWingChord;
		m_fWingArea = pAeroParam->m_fWingArea;

		m_fDeMax = pAeroParam->m_fDeMax * pi / 180.0; // angle data are given in degree
		m_fDeMin = pAeroParam->m_fDeMin * pi / 180.0;
		m_fDaMax = pAeroParam->m_fDaMax * pi / 180.0;
		m_fDaMin = pAeroParam->m_fDaMin * pi / 180.0;
		m_fDrMax = pAeroParam->m_fDrMax * pi / 180.0;
		m_fDrMin = pAeroParam->m_fDrMin * pi / 180.0;

		m_fGMin = pAeroParam->m_fGMin;
        m_fGMax = pAeroParam->m_fGMax;

		m_fMass = pAeroParam->m_fMass;
		m_fMassInverse = 1.0 / m_fMass;
		m_fI_XX = pAeroParam->m_fI_XX;
		m_fI_YY = pAeroParam->m_fI_YY;
		m_fI_ZZ = pAeroParam->m_fI_ZZ;
		m_fI_XZ = pAeroParam->m_fI_XZ;

		// Here: x points right wing, y points noze, z points up.
		// At contrary to litterature where:
		// x points noze, y points right, z points down
		m_Inertia = StandardMatrix3(m_fI_YY,     0.0,     0.0,  
			                            0.0, m_fI_XX, m_fI_XZ, 
								        0.0, m_fI_XZ, m_fI_ZZ);
		m_InertiaInverse = m_Inertia.Inverse();	
       
		m_fThrustMax = pAeroParam->m_fThrustMax;

		m_fCD0 = pAeroParam->m_fCD0;
		m_fCD_a = pAeroParam->m_fCD_a;
		m_fCD_de = pAeroParam->m_fCD_de;

		m_fCL0 = pAeroParam->m_fCL0;
		m_fCL_a = pAeroParam->m_fCL_a;
		m_fCL_adot = pAeroParam->m_fCL_adot;
		m_fCL_q = pAeroParam->m_fCL_q;
		m_fCL_de = pAeroParam->m_fCL_de;
		m_fstallAOA = DegreesToRadians(pAeroParam->m_fstallAOA);

		m_fCM0 = pAeroParam->m_fCM0;         
		m_fCM_a = pAeroParam->m_fCM_a;        
		m_fCM_adot = pAeroParam->m_fCM_adot;
		m_fCM_q = pAeroParam->m_fCM_q;         
		m_fCM_de = pAeroParam->m_fCM_de;        

		m_fCY_beta = pAeroParam->m_fCY_beta;      
		m_fCY_p = pAeroParam->m_fCY_p;         
		m_fCY_r = pAeroParam->m_fCY_r;         
		m_fCY_da = pAeroParam->m_fCY_da;        
		m_fCY_dr = pAeroParam->m_fCY_dr;        

		m_fCI_beta = pAeroParam->m_fCI_beta;       
		m_fCI_p = pAeroParam->m_fCI_p;          
		m_fCI_r = pAeroParam->m_fCI_r;          
		m_fCI_da = pAeroParam->m_fCI_da;         
		m_fCI_dr = pAeroParam->m_fCI_dr;         

		m_fCn_beta = pAeroParam->m_fCn_beta;       
		m_fCn_p = pAeroParam->m_fCn_p;          
		m_fCn_r = pAeroParam->m_fCn_r;          
		m_fCn_da = pAeroParam->m_fCn_da;         
		m_fCn_dr = pAeroParam->m_fCn_dr;	
		m_depsilon = std::min(m_fGMax,fabs(m_fGMin)) / 2.0;
	}
}



void AirplanePhysics::DoSimStep(double dt)
{
    CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: DoSimStep; Time: " << g_pSimTime->getSimTime() );
	
	unsigned short const maxIteration = 150;
	unsigned short const minIteration = 100;

	if ( m_dprevdt - dt > 0.0 )
	{
		if ( m_usMaxi < maxIteration )
		++m_usMaxi;
	}
	else 
		if ( m_usMaxi > minIteration )
			--m_usMaxi;
    m_dprevdt = dt;
	dt /= m_usMaxi; 
		
		for (unsigned short modelStepNumber = 0; modelStepNumber < m_usMaxi; ++modelStepNumber)
		{	 
			m_angularAccel = LocalToBody(m_AngularAccelLocal);
			m_VelocityAngular = LocalToBody(m_AngularVelocityLocal);
			m_LinearAccel = LocalToBody(m_LinearAccelLocal);
			m_VelocityBody = LocalToBody(m_VelocityLocal);
			
			// Calculate angle of attack, rate of AOA
			// alpha is 0 when w velocity (i.e. vertical velocity) is 0
			double m_falphaNew = - Atan( m_VelocityBody.y, m_VelocityBody.z ); 
			m_falphaDot = ( m_falphaNew - m_falpha ) / dt;
			m_falpha = m_falphaNew;
			
			// Calculate side angle
			// beta is 0 when v velocity (i.e. side velocity) is 0; note here v is m_VelocityBody.x
			m_fbeta  = Atan(m_VelocityBody.y, m_VelocityBody.x); 
			
			StandardVector3 currentAngularAccel;
			StandardVector3 currentVelocityAngular;
			StandardVector3 currentLinearAccel;
			unsigned short GControlNumber = 0;
			
			//do // Testing some feedback control
			{
				m_Elevator *= ControlInputValue( m_fgForce );
				
				// Dynamic pressure
				double qBarS = 0.5 * CalculateDynamicPressure(m_PositionLocal.z) * m_fWingArea;
				
				// Calculate moment
				m_Moments = CalculateMoments(qBarS);
				
				// First, calculate angular acceleration
				currentAngularAccel = m_InertiaInverse * (m_Moments - (m_VelocityAngular^(m_Inertia * m_VelocityAngular)))
					// add a damping term
					;//- 0.5 * m_VelocityAngular.Length() * m_VelocityAngular; //.Unitize();
				
				// Angular acceleration mean
				currentAngularAccel += m_angularAccel;
				currentAngularAccel *= 0.5;
				
				// Integrate angular acceleration gives angular velocity
				currentVelocityAngular = dt * currentAngularAccel + m_VelocityAngular;
				// Games gems advice
				currentVelocityAngular *= 0.999; 
				
				// Calculate force
				m_ForcesBody = CalculateForces(qBarS);  
				
				// Deduce linear acceleration
				currentLinearAccel =  m_fMassInverse * m_ForcesBody - (currentVelocityAngular^m_VelocityBody) 
					;  //- 0.05 * m_VelocityBody;
				// Linear acceleration mean
				currentLinearAccel += m_LinearAccel;
				currentLinearAccel *= 0.5;
				
				++GControlNumber;
			}
			// Exit when g force is convenable or too many loops
			//while ( ( m_fgForce < m_fGMin || m_fgForce > m_fGMax || m_falpha > m_fstallAOA  ) 
			//	  && GControlNumber < 1 ); 
			
			
			// Update angular accel
			m_angularAccel = currentAngularAccel;
			
			// Update angular velocity
			m_VelocityAngular = currentVelocityAngular;
			
			// Update linear acceleration
			m_LinearAccel = currentLinearAccel;
			
			// Integrate linear velocity
			m_VelocityBody += dt * m_LinearAccel;
			
			
			m_fSpeedLocal = m_VelocityBody.Length();
			
			
			m_AngularAccelLocal = BodyToLocal(m_angularAccel);
			m_AngularVelocityLocal = BodyToLocal(m_VelocityAngular);
			m_LinearAccelLocal = BodyToLocal(m_LinearAccel);
			// Calculate local velocity before changing coordinates
			StandardVector3 currentLinVelocity = BodyToLocal(m_VelocityBody);
			
			// Integrate the rotation quaternion
			qOrientation +=	0.5f * dt * ( qOrientation * m_VelocityAngular);
			// Now normalize the orientation quaternion:
			double mag = qOrientation.Magnitude();
			if (mag != 0.0)
				qOrientation /= mag;
			
			
			// Update local linear velocity
			m_VelocityLocal += currentLinVelocity;
			m_VelocityLocal *= 0.5;
			
			// Update position in local frame
			m_PositionLocal += dt * m_VelocityLocal;
			
		}
		
		CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics:   Forces: " << m_ForcesBody );
		CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics:  Moments: " << m_Moments );
		CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics:ang accel: " << m_angularAccel);
		CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: Ang Velo: " << m_VelocityAngular );
		CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics:   LinAcc: " << m_LinearAccel );
		CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: Velocity: " << m_VelocityLocal);
		CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics:    Speed: " << m_fSpeedLocal);
		
		CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: AngOfAtt: " << RadiansToDegrees(m_falpha) );
		
		//CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics:   VelDir: " << VelocityDirection );
		
		CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: Position: " << m_PositionLocal );
		CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhsyics:   Orient: " << qOrientation );
		//CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhyiscs:  Euler A: " << m_EulerAngles );
}

 // must calculate the resultant forces ...
StandardVector3 AirplanePhysics::CalculateForces( double const p_qBarS )
{
    CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: BodyVelocity: " << m_VelocityBody);
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: local velocity: " << m_VelocityLocal);
    CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: BodySpeed: " << m_fSpeedLocal);
	
	// Calculate Aero Forces

	// Lift
	StandardVector3 forceBody = p_qBarS * LiftVector();

	// Evaluate g's
	m_fgForce = m_fMassInverse * forceBody.z / 9.81;

    // Drag and sideforce
	forceBody +=  p_qBarS * ( DragVector() + SideVector() );

	// Add thrust concentrated in y direction
	forceBody.y += m_Thrust;

   // Add gravity
    StandardVector3 gravityWorld = - m_fMass * CalculateGravity(m_PositionLocal.z) * StandardVector3::ZAXIS;
    m_GravityBody = LocalToBody( gravityWorld );
	forceBody += m_GravityBody;

	return forceBody;

}

// ... and moments on the aircraft
StandardVector3 AirplanePhysics::CalculateMoments( double const p_qBarS ) const
{
	StandardVector3 moments;

	moments.x = CalculatePitchMoment(p_qBarS); // Pitch
    moments.y = CalculateRollMoment(p_qBarS);  // Roll
    moments.z = CalculateYawMoment(p_qBarS);   // Yaw

	return moments;
}


double AirplanePhysics::CalculateLiftCoefficient() const
{
	double lift_coe;
		lift_coe = (m_fCL0 + m_fCL_a * m_falpha 
			       + (m_fCL_q * m_VelocityAngular.x + m_fCL_adot * m_falphaDot ) * m_fWingChord / ( 2.0 * m_fSpeedLocal)
			       + m_fCL_de * m_Elevator);

	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics::CalculateLiftCoefficient() " << 
		lift_coe << " at alpha = " << RadiansToDegrees(m_falpha) << 
		", Elevator: " << m_Elevator );
    return lift_coe;
}

StandardVector3 AirplanePhysics::LiftVector() const
{
	StandardVector3 lift;
	lift.x = 0.0;

	double Cl = CalculateLiftCoefficient();
	lift.y = Cl * sin(m_falpha);
	lift.z = Cl * cos(m_falpha);

	if ( m_falpha > m_fstallAOA)
	{
		double c = m_falpha - m_fstallAOA;
		c *= c;
        lift.y -= c;
		lift.z /= 1.0 + c;
	}
	
	return lift;
}

double AirplanePhysics::CalculateDragCoefficient() const
{
	double drag_coe;
	
    drag_coe =  m_fCD0 + m_fCD_a * m_falpha + m_fCD_de * m_Elevator;

		CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics::CalculateDragCoefficient() " << 
		drag_coe << " at alpha = " << RadiansToDegrees(m_falpha) << 
		", Elevator: " << m_Elevator );

    return drag_coe;
}

StandardVector3 AirplanePhysics::DragVector() const
{
	StandardVector3 drag;
	double Cd = CalculateDragCoefficient();
	
	drag.x = 0.0;
	drag.y = - Cd * cos(m_falpha);
	drag.z = Cd * sin(m_falpha);

	return drag;
}

double AirplanePhysics::CalculateSideCoefficient() const
{
	double side_coe;

	side_coe = m_fCY_beta * m_fbeta + m_fCY_dr * m_Rudder - m_fCY_r * m_VelocityAngular.z;

	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics::CalculateSideCoefficient() " << 
		side_coe );

    return side_coe;
}

StandardVector3 AirplanePhysics::SideVector() const
{
	StandardVector3 side;
	double Cs = CalculateSideCoefficient();
	
	side.x = Cs * cos(m_fbeta);
	side.y = - Cs * sin(m_fbeta);
	//side.x = Cs * sin(m_fbeta);
	//side.y = - Cs * cos(m_fbeta);
	side.z = 0.0;

	return side;
}

double AirplanePhysics::CalculateRollMoment(double const qbarS) const
{
    return    ( m_fCI_beta * m_fbeta + m_fCI_da * m_Aileron
		      + (m_fCI_p * m_VelocityAngular.y - m_fCI_r * m_VelocityAngular.z)* m_fWingSpan / ( 2.0 * m_fSpeedLocal)
		      + m_fCI_dr * m_Rudder) * qbarS * m_fWingSpan;
}

double AirplanePhysics::CalculatePitchMoment(double const qbarS) const 
{
	return ( m_fCM0 + m_fCM_a * m_falpha 
		   + ( m_fCM_q * m_VelocityAngular.x + m_fCM_adot * m_falphaDot ) * m_fWingChord / ( 2.0 * m_fSpeedLocal)
		   + m_fCM_de * m_Elevator) * qbarS * m_fWingChord;
}

double AirplanePhysics::CalculateYawMoment(double const qbarS) const
{   
    return - ( m_fCn_beta * m_fbeta 
		     + (m_fCn_p * m_VelocityAngular.y - m_fCn_r * m_VelocityAngular.z) * m_fWingSpan / ( 2.0 * m_fSpeedLocal) 
			 + m_fCn_da * m_Aileron + m_fCn_dr * m_Rudder) * qbarS *  m_fWingSpan; // Yaw
} 

double AirplanePhysics::CalculateDynamicPressure(double alt) const
{
	double qBar = CalculateAirDensity(alt) * m_fSpeedLocal * m_fSpeedLocal;
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics::CalculateDynamicPressure: qBar " << 
	qBar);
    return qBar;
}

// Air density is a function of temperature, and altitude. Maybe also pressure and humidity.
// Return units in kg/m^3
double AirplanePhysics::CalculateAirDensity(double const alt) const
{  
	// Mair = 28.966 10^(-3) kg / mol
	// rho0 = 1.206 : air density at 0 level
	// T0 = 288 K : air temperature at 0 level
	// R = 8.31
	// alpha = 1.4
    // coef = Mair * (1 - alpha) / (R * T0 * alpha) 
    // powerCoef = 1 / (alpha - 1) = 2.5
    // rho = rho1 * (1 + coef * g * z )^(1/(alpha - 1))
    // T1 = 217
    // rho1 = 0.3602
	// z1 = 11000
	// rho = rho1 * exp(-Mair * g / (R * T1) * (z - z1))

	double density;

	if ( alt < 11000.0 )
	    density = 1.206 * pow(1.0 - 0.0000034616 * CalculateGravity(alt) * alt, 2.5);
	else
		density = 0.3602 * exp (- 0.00001606304 * CalculateGravity(alt) * (alt - 11000.0));
    CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: CalculateAirDensity: " << density << " at altitude: " << alt);
	return density;
}

double AirplanePhysics::CalculateGravity(double const p_altitude) const
{
	// g = G * Mearth / (Rearth + z)^2
	// G = 6.672 10^(-11)
	// Mearth = 5.976 10^(24) kg
	// Rearth = 6378 10^3 m

	double h =   1.0 + p_altitude / 6378000.0 ;
	double gravity = 9.8016140266 / ( h * h );
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: CalculateGravity: " << gravity << " at altitude " << p_altitude);
    return gravity;
}

StandardVector3 AirplanePhysics::LocalToBody(const StandardVector3 & vec )
{
    return QVRotate( qOrientation.Bar(), vec );
}

StandardVector3 AirplanePhysics::BodyToLocal(const StandardVector3 & vec )
{
    return QVRotate( qOrientation, vec );
}

StandardMatrix3 AirplanePhysics::MakeAngularVelocityMatrix(StandardVector3 u)
{
	return StandardMatrix3(	0.0f, -u.z, u.y,
					        u.z, 0.0f, -u.x,
						   -u.y, u.x, 0.0f);
}

void AirplanePhysics::setThrottle(double setting)
{ 
	m_Throttle = setting; 
	m_Thrust = ThrottleFactor * CalculateAirDensity(m_PositionLocal.z) * m_Throttle
		     * CalculateGravity(m_PositionLocal.z) * m_fThrustMax / ( 1.0 + m_fSpeedLocal );
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics::setThrottle(): Throttle: " << 
		m_Throttle << ", Thrust: " << m_Thrust );
}

void AirplanePhysics::setAileron(double aileron) 
{ 
    m_Aileron = SetControl(ControlSensibility(aileron), m_fDaMax, m_fDaMin);
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics::setAileron(): " << 
		m_Aileron  );
}

void AirplanePhysics::setElevator(double elevator) 
{
    m_Elevator = SetControl(ControlSensibility(elevator), m_fDeMax, m_fDeMin);
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics::setElevator(): " << 
		m_Elevator  );
}

void AirplanePhysics::setRudder( double rudder ) 
{ 
	m_Rudder = SetControl(ControlSensibility(rudder), m_fDrMax, m_fDrMin);
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics::setRudder(): " << 
		m_Rudder  );
}

double AirplanePhysics::getThrust() const
{
 return m_Thrust;
}

void  AirplanePhysics::setVelocity(const StandardVector3 & velo) 
{ 
	m_VelocityLocal = velo; 
}

double AirplanePhysics::CIVbasis(double p_t) const
{
	double cIV = p_t * p_t  * ( 2 * p_t + 3 );
	return cIV;
}

double AirplanePhysics::ControlInputValue(double p_gForce) const
{ // to reduce G, decrease deflection control surface
	double control = 1.0;

    if ( p_gForce > m_fGMax || p_gForce < m_fGMin || m_falpha > m_fstallAOA )
		return 0.0;
	if ( p_gForce > m_fGMax - m_depsilon )
		control = CIVbasis((p_gForce - m_fGMax) / m_depsilon);
	else
		if ( p_gForce < m_fGMin + m_depsilon )
			control = CIVbasis((m_fGMin - p_gForce) / m_depsilon);
		
	return control;
}

double AirplanePhysics::SetControl(double const p_setting, double const & p_mMax, double const & p_mMin) const
{
    if ( p_setting >=0.0 )
		return p_setting * p_mMax;
	else
		return -p_setting * p_mMin;
}

double AirplanePhysics::ControlSensibility(double p_x) const
{ // smooth the joystick controls

	double z = p_x * p_x;
	double y = p_x * z * ( 2.0 - z );

	return y;
}