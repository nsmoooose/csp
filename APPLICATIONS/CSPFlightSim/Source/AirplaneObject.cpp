#include "BaseInput.h"
#include "AirplaneObject.h"
#include "LogStream.h"
#include "MessageQueue.h"
#include "SimTime.h"
#include "VirtualBattlefield.h"


extern VirtualBattlefield * g_pBattlefield;
extern MessageQueue * g_pMessageQueue;
extern SimTime * g_pSimTime;
extern BaseInput * g_pPlayerInput;

using namespace std;

double	const	g	= 9.806; // acceleration due to gravity, m/s^2
//double	const	rho = 0.0023769f * 14.5938 / 0.02831685 ; // air density at sea level, slugs/ft^3 -> convert to kg/m^3
double   const   rho = 1.225; //kg/m^3
#define	_DTHRUST	10.0f
#define	_MAXTHRUST	300000.0f
double   const   SlugToKg = 14.5938f;
double   const   FtToMeters = 0.3048f;
double   const   Ft2ToM2 = 0.09290394f;
double   const   FtIbToKgM = 0.1383f;

AirplaneObject::AirplaneObject()
{
  m_iObjectType = AIRPLANE_OBJECT_TYPE;
  m_iObjectID = g_pBattlefield->getNewObjectID();
  m_iControllerID = 0;
  m_sObjectName = "UNNAMEDPLANE";
  m_bDeleteFlag = false;
  m_pController = NULL;

  m_Orientation = StandardMatrix3::IDENTITY;
  m_InitialDirection = StandardVector3(0.0, 1.0, 0.0);
  m_InitialNormDir = StandardVector3(0.0, 0.0, 1.0);

  m_Direction = m_InitialDirection;
  m_CurrentNormDir = m_InitialNormDir;
  setGlobalPosition( StandardVector3(0.0,0.0,0.0) );
  m_Speed = 500.0;      // meters per sec
  m_LinearVelocity = m_Speed*m_Direction;
  m_LinearVelocityDirection = m_LinearVelocity.Normalize();
  m_Thrust = 0.7;
  m_ThrustMin = 0.0;
  m_ThrustMax = 1.0;
  m_Aileron = 0.0;
  m_Elevator = 0.0;
  m_AileronMin = -1.0;
  m_AileronMax = 1.0;
  m_ElevatorMin = -1.0;
  m_ElevatorMax = 1.0;
  m_Rudder = 0.0;
  m_Throttle = 0.2;

  m_AileronKey = 0.0;
  m_ElevatorKey = 0.0;

  m_GravityForce =  - g * StandardVector3::ZAXIS;    // gravity force is always towards earth.
  m_ThrustForce = StandardVector3(0,0,0);
  m_LiftForce = StandardVector3(0,0,0);
  m_DragForce = StandardVector3(0,0,0);
  m_CurrentForceTotal = StandardVector3(0,0,0);
  m_gForce = 1.0;

  m_AngularVelocity = StandardVector3::ZERO;

  m_ForceBody = StandardVector3(0,0,0);;

  sensativity = 1.0;
  planesensx = 0.3;
  planesensy = 0.5;
  plusplus = 1.0;
  m_ThrottleFactor = 300;


  // Data from NPSNET paper.
  m_Mass = 546;

  m_Ibody[0][0] = 8090;
  m_Ibody[1][1] = 25900;
  m_Ibody[2][2] = 29200;
  m_Ibody[0][2] = m_Ibody[2][0] = 1300.0;
  m_IbodyInv = m_Ibody.Inverse();

  m_fBoundingSphereRadius = 20;
//  m_color = new StandardColor(0, 0, 255, 255);

  m_bComplex = false;
  m_bPhysicsInitialized = false;
  m_pAirplanePhysics = new AirplanePhysics;
}

AirplaneObject::~AirplaneObject()
{
 delete m_pAirplanePhysics;
}

void AirplaneObject::initialize()
{
  if (m_sObjectName == "PLAYER" )
    initializeHud();
}

void AirplaneObject::initializeHud()
{
  m_phud = osgNew Hud(m_InitialDirection);
  m_phud->BuildHud("../Data/Scripts/HudM2k.csp");
  m_rpSwitch->addChild(m_phud);
}

void AirplaneObject::dump()
{
  cout << "ID: " << m_iObjectID
           << ", TYPE: " << m_iObjectType
           << ", ARMY: " << m_iArmy
           << ", GLOPOSX: " << m_GlobalPosition.x
           << ", GLOPOSY: " << m_GlobalPosition.y
           << ", GLOPOSZ: " << m_GlobalPosition.z
           << ", DIRX: " << m_Direction.x
           << ", DIRY: " << m_Direction.y
           << ", DIRZ: " << m_Direction.z
           << ", VELX: " << m_LinearVelocity.x
	   << ", VELY: " << m_LinearVelocity.y
           << ", VELZ: " << m_LinearVelocity.z
           << endl;      

}

void AirplaneObject::OnUpdate(double dt)
{
   CSP_LOG(CSP_APP, CSP_DEBUG, "AirplaneObject::OnUpdate ...");
    {
        if (m_pController)
          m_pController->OnUpdate(dt);

        if ( m_sObjectName == "PLAYER" )
         g_pPlayerInput->OnUpdate();

        doMovement(dt);

		if ( m_sObjectName == "PLAYER" )
		  m_phud->OnUpdate();
    }
   CSP_LOG(CSP_APP, CSP_DEBUG, "... AirplaneObject::OnUpdate");
	
}

int AirplaneObject::updateScene()
{
	BaseObject::updateScene();
	return 0;
}

unsigned int AirplaneObject::OnRender()
{
//  updateScene();
  return 0;
}


void AirplaneObject::doMovement(double dt)
{
    if (m_bComplex)
        doComplexPhysics(dt);
    else
        doSimplePhysics(dt);

	updateLocalPosition();
	updateGlobalPosition();

    m_Orientation.ToEulerAnglesZXY(m_heading, m_pitch, m_roll);

    m_heading = osg::RadiansToDegrees( m_heading );
    m_pitch = osg::RadiansToDegrees ( m_pitch );
    m_roll = osg::RadiansToDegrees( m_roll );
}

void AirplaneObject::doSimplePhysics(double dt)
{
  // Save the objects old position
  m_PrevPosition = m_LocalPosition;

  // update the orientation matrix based on control surfaces;
  double pitchrate = (m_Elevator) * sensativity * planesensx * plusplus * dt;
  double rollrate = (m_Aileron) * sensativity * planesensy * plusplus * dt;
  // rudder effect is limited by speed
  double yawrate = (m_Rudder) * sensativity * planesensy * plusplus * dt * 10 / sqrt(1+m_Speed);
  
  StandardMatrix3 rotZ;
  rotZ.FromAxisAngle (m_CurrentNormDir, - yawrate - rollrate); 

  StandardMatrix3 rotY;
  rotY.FromAxisAngle (rotZ * m_Direction, 2 * rollrate + 0.25*yawrate ); 

  StandardMatrix3 rotX;
  rotX.FromAxisAngle( rotY * rotZ * (m_Direction^m_CurrentNormDir), pitchrate); 

  // update orientation matrix
  m_Orientation = rotX * rotY * rotZ * m_Orientation;
  
  // update direction
  m_Direction = m_Orientation * m_InitialDirection; //+ 0.5 * dt * m_GravityForce;
  
  m_AngleOfAttack = angleBetweenTwoVectors(m_Direction, StandardVector3(m_Direction.x,m_Direction.y,0.0));

  // update normal vector to wings aircraft plane
  m_CurrentNormDir = m_Orientation * m_InitialNormDir;

  m_Speed = 200.0;
  m_Speed += dt * m_Thrust * m_Speed / (sqrt(m_Speed) + 1.0); 
  
  // update linear velocity and position
  m_Direction.Normalize(); 
  m_LinearVelocity = m_Speed * m_Direction;
//  setVelocity(m_LinearVelocity);
  m_LocalPosition += dt * m_LinearVelocity;

  CSP_LOG(CSP_APP, CSP_TRACE, "AirplaneObject::SimplePhysics: ObjectID: " << m_iObjectID 

      << ", Elevator: " << m_Elevator <<
                  ", Aileron: " << m_Aileron << ", PitchRate: " << pitchrate <<
                  ", RollRate: " << yawrate );
  CSP_LOG(CSP_APP, CSP_TRACE, "AirplaneObject::SimplePhysics: Position: " << m_LocalPosition <<
                  ", Direction: " << m_Direction << ", Up: " << m_CurrentNormDir <<
                  ", Velocity: " << m_LinearVelocity );
  CSP_LOG(CSP_APP, CSP_TRACE, "AirplaneObject::SimplePhysics: Orientation: " << std::endl << m_Orientation);
}

void AirplaneObject::doComplexPhysics(double dt)
{
    CSP_LOG(CSP_APP, CSP_TRACE, "AirplaneObject::doComplexPhysics...");

    if (!m_bPhysicsInitialized)
    {
        m_pAirplanePhysics->Initialize();
	    m_pAirplanePhysics->setVelocity(m_LinearVelocity);
        m_pAirplanePhysics->setSpeed(m_Speed);
        m_pAirplanePhysics->qOrientation.FromRotationMatrix(m_Orientation);
        m_bPhysicsInitialized = true;
    }
    
    m_pAirplanePhysics->setPosition(m_LocalPosition);
    m_pAirplanePhysics->setThrottle(m_Throttle);
    m_pAirplanePhysics->setAileron(m_Aileron);
    m_pAirplanePhysics->setElevator(m_Elevator);
    m_pAirplanePhysics->setRudder(m_Rudder);

    m_pAirplanePhysics->DoSimStep(dt);

    m_pAirplanePhysics->qOrientation.ToRotationMatrix(m_Orientation);

	// testing
	m_qOrientation = m_pAirplanePhysics->qOrientation;

    m_Direction = m_Orientation * m_InitialDirection;
    m_CurrentNormDir = m_Orientation * m_InitialNormDir;

    m_LocalPosition = m_pAirplanePhysics->m_PositionLocal;

    m_LinearVelocity = m_pAirplanePhysics->m_VelocityLocal;
	m_Speed = m_pAirplanePhysics->m_fSpeedLocal;
	m_AngleOfAttack = m_pAirplanePhysics->getAngleOfAttack();
	setGForce(m_pAirplanePhysics->m_fgForce);

    //m_Force = m_pAirplanePhysics->m_ForcesBody;
    //m_Torque = m_pAirplanePhysics->m_Moments;
	CSP_LOG(CSP_APP, CSP_DEBUG, "...AirplaneObject::doComplexPhysics");
	CSP_LOG(CSP_APP, CSP_DEBUG, "");
}

void AirplaneObject::setElevator(double setting)
{
    m_Elevator = setting;
}

double AirplaneObject::getElevator() const
{
    return m_Elevator;
}


void AirplaneObject::setAileron(double setting)
{
    m_Aileron = setting;
}

double AirplaneObject::getAileron() const
{
    return m_Aileron;
}

void AirplaneObject::setRudder(double setting)
{
    m_Rudder = setting;
}

double AirplaneObject::getRudder() const
{
    return m_Rudder;
}

void AirplaneObject::setThrust(double setting)
{
    m_Thrust = setting * m_ThrottleFactor;
}

double AirplaneObject::getThrust() const
{
    return m_Thrust;
}

void AirplaneObject::setThrottle(double setting)
{
	 m_Throttle = setting;
	 setThrust(setting);
}

double AirplaneObject::getThrottle() const
{
	return m_Throttle;
}

void AirplaneObject::setAero( AeroParam * pAeroParam )
{
	m_pAirplanePhysics->setAeroParams( pAeroParam );
}
