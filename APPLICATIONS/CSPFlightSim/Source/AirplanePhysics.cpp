#include "stdinc.h"


#include "AirplanePhysics.h"
#include "SimTime.h"
#include "SymbolTable.h"
#include "BaseSymbol.h"
#include "AeroParamSymbol.h"


extern SimTime * g_pSimTime;
extern SymbolTable GlobalSymbolTable;

AirplanePhysics::AirplanePhysics()
{	
    m_Elevator = 0.0;
    m_Aileron = 0.0;
    m_Rudder = 0.0;
    m_Thrust = 0.0;
	
	ThrottleFactor = 256;// this converts throttle setting 0-100 to thrust in kN.
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
	m_gForce = 1.0;
	m_Moments = StandardVector3::ZERO;

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

		m_fMass = pAeroParam->m_fMass;
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
	}
}

void AirplanePhysics::DoSimStep(double dt)
{
	double whatIsReasonable = 2.0*0.22;
	
	double tEnd = dt;
	double stepInverse = std::min<double>(dt,whatIsReasonable / ( m_fSpeedLocal + 1.0 ));

	//dt = stepInverse;

	int nSteps = tEnd/stepInverse + 0.99999999;

	dt = dt/nSteps;


    CSP_LOG(CSP_PHYSICS, CSP_INFO, "AirplanePhysics: DoSimStep; Time: " << g_pSimTime->getSimTime() <<
		", OverallTimeStep: " << tEnd << ", TimeStep: " << dt <<
		", Steps: " << tEnd/dt );
    
	for (double t = 0.0; t < tEnd; t += dt)
	{	 
		m_angularAccel = LocalToBody(m_AngularAccelLocal);
		m_VelocityAngular = LocalToBody(m_AngularVelocityLocal);
		m_LinearAccel = LocalToBody(m_LinearAccelLocal);
		m_VelocityBody = LocalToBody(m_VelocityLocal);

		// Calculate the forces at the next step.
		CalculateForceAndMoment(dt);
		
		// Fist, calculate angular acceleration
		StandardVector3 currentAngularAccel = 
			                          m_InertiaInverse * (m_Moments - (m_VelocityAngular^(m_Inertia * m_VelocityAngular)));
		
		// Integrate angular velocity
		m_VelocityAngular += 0.5 * dt * (currentAngularAccel + m_angularAccel);
		
		// update angular accel
		m_angularAccel = currentAngularAccel;
		
		// Deduce linear acceleration
		StandardVector3 currentLinearAccel =  (1.0 / m_fMass) * m_ForcesBody - (m_VelocityAngular^m_VelocityBody);
		
		// Integrate linear velocity
		m_VelocityBody += 0.5 * dt * (currentLinearAccel + m_LinearAccel);
		//m_directionBody = Normalized(m_VelocityBody);
		m_fSpeedLocal = m_VelocityBody.Length();
		m_LinearAccel = currentLinearAccel;
		
		m_AngularAccelLocal = BodyToLocal(m_angularAccel);
		m_AngularVelocityLocal = BodyToLocal(m_VelocityAngular);
		m_LinearAccelLocal = BodyToLocal(m_LinearAccel);
		StandardVector3 currentLinVelocity = BodyToLocal(m_VelocityBody);
		
		// Integrate the rotation quaternion
		qOrientation +=	0.5f * dt * ( qOrientation * m_VelocityAngular);
		// Now normalize the orientation quaternion:
		double mag = qOrientation.Magnitude();
		if (mag != 0.0)
			qOrientation /= mag;
		
		// Go back to the local frame & update position
		m_PositionLocal += 0.5 * dt * (m_VelocityLocal + currentLinVelocity);
		m_VelocityLocal = currentLinVelocity;
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

 // must calculate the resultant forces and moments on the aircraft
void AirplanePhysics::CalculateForceAndMoment( double dt )
{
    CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: BodyVelocity: " << m_VelocityBody);
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: local velocity: " << m_VelocityLocal);
    CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: BodySpeed: " << m_fSpeedLocal);
	
    // Calculate angle of attack, ...
    double m_falphaNew = - atan(m_VelocityBody.z/m_VelocityBody.y); // alpha is 0 when w velocity (i.e. vertical velocity) is equal to 0
	m_falphaDot = ( m_falphaNew - m_falpha ) / dt;
    m_falpha = m_falphaNew;

    m_fbeta  = atan(m_VelocityBody.x/m_VelocityBody.y); // beta is 0 when u velocity (i.e. side velocity) is equal to 0

    double lift_coe = CalculateLiftCoefficient();
    double drag_coe = CalculateDragCoefficient();
	double side_coe = CalculateSideCoefficient();
    double qbarS = 0.5 * CalculateDynamicPressure(m_PositionLocal.z) * m_fWingArea;
    
    double lift = lift_coe * qbarS;
    double drag = drag_coe * qbarS;
	double side = side_coe * qbarS;
   
	// update G force
	m_gForce = lift / ( 20.0 * m_fMass );

    // Calculate Aero Forces
	m_ForcesBody.x = side * cos(m_fbeta); 
    m_ForcesBody.y = lift * sin(m_falpha) - drag * cos(m_falpha) - sin(m_fbeta) * side + m_Thrust;
    m_ForcesBody.z = lift * cos(m_falpha) + drag * sin(m_falpha);
    
    StandardVector3 gravityWorld = m_fMass * CalculateGravity(m_PositionLocal.z) * StandardVector3::ZAXIS;

    m_GravityBody = LocalToBody( gravityWorld );
	m_ForcesBody -= m_GravityBody;

   
    // Calculate Moments
	m_Moments.x = CalculatePitchMoment(qbarS); // Pitch
    m_Moments.y = CalculateRollMoment(qbarS);  // Roll
    m_Moments.z = CalculateYawMoment(qbarS);   // Yaw
}

double AirplanePhysics::CalculateLiftCoefficient()
{
	double lift_coe;
	//if (m_falpha > 0.0)
		lift_coe = (m_fCL0 + m_fCL_a * m_falpha 
			       + (m_fCL_q * m_VelocityAngular.x + m_fCL_adot * m_falphaDot ) * m_fWingChord / (2.0 * m_fSpeedLocal)
			       + m_fCL_de * m_Elevator);
	//else 
	//	lift_coe = .01;

	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics::CalculateLiftCoefficient() " << 
		lift_coe << " at alpha = " << RadiansToDegrees(m_falpha) << 
		", Elevator: " << m_Elevator );
    return lift_coe;
}

double AirplanePhysics::CalculateDragCoefficient()
{
	double drag_coe;
	
    drag_coe =   m_fCD0 + m_fCD_a * m_falpha + m_fCD_de * m_Elevator;

		CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics::CalculateDragCoefficient() " << 
		drag_coe << " at alpha = " << RadiansToDegrees(m_falpha) << 
		", Elevator: " << m_Elevator );

    return drag_coe;
}

double AirplanePhysics::CalculateSideCoefficient() const
{
	double side_coe;

	side_coe = m_fCY_beta * m_fbeta + m_fCY_dr * m_Rudder - m_fCY_r * m_VelocityAngular.z;

	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics::CalculateSideCoefficient() " << 
		side_coe );

    return side_coe;
}

double AirplanePhysics::CalculateRollMoment(double qbarS)
{
    return    ( m_fCI_beta * m_fbeta + m_fCI_da * m_Aileron
		      + (m_fCI_p * m_VelocityAngular.y - m_fCI_r * m_VelocityAngular.z)* m_fWingSpan / (2.0 * m_fSpeedLocal)
		      + m_fCI_dr * m_Rudder) * qbarS * m_fWingSpan;
}

double AirplanePhysics::CalculatePitchMoment(double qbarS)
{
	return ( m_fCM0 + m_fCM_a * m_falpha 
		   + ( m_fCM_q * m_VelocityAngular.x + m_fCM_adot * m_falphaDot ) * m_fWingChord / (2.0 * m_fSpeedLocal)
		   + m_fCM_de * m_Elevator) * qbarS * m_fWingChord;
}

double AirplanePhysics::CalculateYawMoment(double qbarS)
{   
    return - ( m_fCn_beta * m_fbeta 
		     + (m_fCn_p * m_VelocityAngular.y - m_fCn_r * m_VelocityAngular.z) * m_fWingSpan / (2.0 * m_fSpeedLocal) 
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
double AirplanePhysics::CalculateAirDensity(double alt) const
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
		density = 0.3602 * exp (- 0.00001606304 * CalculateGravity(alt) * (alt - 11000));
    CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics: CalculateAirDensity: " << density << " at altitude: " << alt);
	return density;
}

double AirplanePhysics::CalculateGravity(double p_altitude) const
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
	m_Thrust = ThrottleFactor * CalculateAirDensity(m_PositionLocal.z) * sqrt(m_Throttle)
		     * CalculateGravity(m_PositionLocal.z) * m_fThrustMax / m_fSpeedLocal;
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics::setThrottle(): Throttle: " << 
		m_Throttle << ", Thrust: " << m_Thrust );
}

void AirplanePhysics::setAileron(double aileron) 
{ 
	if ( aileron >=0.0 )
		m_Aileron = aileron * m_fDaMax; 
	else 
		m_Aileron = fabs(aileron) * m_fDaMin;
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics::setAileron(): " << 
		m_Aileron  );
}

void AirplanePhysics::setElevator(double elevator) 
{
	if ( elevator >=0.0 )
		m_Elevator = elevator * m_fDeMin; 
	else 
		m_Elevator = fabs(elevator) * m_fDeMax; 
	CSP_LOG(CSP_PHYSICS, CSP_DEBUG, "AirplanePhysics::setElevator(): " << 
		m_Elevator  );
}

void AirplanePhysics::setRudder( double rudder ) 
{ 

	if ( rudder >=0.0 )
		m_Rudder = rudder * m_fDrMax; 
	else 
		m_Rudder = fabs(rudder) * m_fDrMin; 
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