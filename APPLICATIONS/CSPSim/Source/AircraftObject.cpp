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
 * @file AircraftObject.cpp
 *
 **/

#include <SimData/Math.h>

#include "AircraftObject.h"
#include "AircraftPhysicsModel.h"
#include "Collision.h"
#include "Profile.h"
#include "Log.h"

using simdata::DegreesToRadians;
using simdata::RadiansToDegrees;


SIMDATA_REGISTER_INTERFACE(AircraftObject)

AircraftObject::AircraftObject(): DynamicObject()
{
	//m_iObjectType = AIRPLANE_OBJECT_TYPE;
	//m_iObjectID = g_pBattlefield->getNewObjectID();

	CSP_LOG(OBJECT, DEBUG, "AircraftObject::AircraftObject() ...");
	m_ObjectName = "AIRCRAFT";

	m_Heading = 0.0;
	m_Roll = 0.0;
	m_Pitch = 0.0;
	
	m_Aileron = 0.0;
	m_Elevator = 0.0;
	m_Rudder = 0.0;
	m_Throttle = 0.0;

	m_dThrottleInput = 0.0;
	m_dAileronInput = 0.0;
	m_dElevatorInput = 0.0;
	m_dRudderInput = 0.0;
	m_BrakeInput = 0.0;
	m_BrakePulse = 0.0;
	
	m_decayAileron = 0;
	m_decayElevator = 0;
	m_decayRudder = 0;
	
	m_AileronMin = -1.0;
	m_AileronMax = 1.0;
	m_ElevatorMin = -1.0;
	m_ElevatorMax = 1.0;
	m_RudderMin = -1.0;
	m_RudderMax = -1.0;
	
	m_AileronInput = 0.0;
	m_ElevatorInput = 0.0;
	m_RudderInput = 0.0;
	m_ThrottleInput = 0.0;

	m_decayAileron = 0;
	m_decayElevator = 0;

	m_ComplexPhysics = false;
	m_PhysicsInitialized = false;

	setAir(true);

	BIND_AXIS("THROTTLE", setThrottle);
	BIND_AXIS("AILERON", setAileron);
	BIND_AXIS("ELEVATOR", setElevator);
	BIND_AXIS("RUDDER", setRudder);
	BIND_ACTION("INC_THROTTLE", IncThrottle);
	BIND_ACTION("STOP_INC_THROTTLE", noIncThrottle);
	BIND_ACTION("DEC_THROTTLE", DecThrottle);
	BIND_ACTION("STOP_DEC_THROTTLE", noDecThrottle);
	BIND_ACTION("INC_AILERON", IncAileron);
	BIND_ACTION("STOP_INC_AILERON", noIncAileron);
	BIND_ACTION("DEC_AILERON", DecAileron);
	BIND_ACTION("STOP_DEC_AILERON", noDecAileron);
	BIND_ACTION("INC_ELEVATOR", IncElevator);
	BIND_ACTION("STOP_INC_ELEVATOR", noIncElevator);
	BIND_ACTION("DEC_ELEVATOR", DecElevator);
	BIND_ACTION("STOP_DEC_ELEVATOR", noDecElevator);
	BIND_ACTION("SMOKE_ON", SmokeOn);
	BIND_ACTION("SMOKE_OFF", SmokeOff);
	BIND_ACTION("SMOKE_TOGGLE", SmokeToggle);
	BIND_ACTION("WHEEL_BRAKE_PULSE", WheelBrakePulse);
	BIND_ACTION("WHEEL_BRAKE_ON", WheelBrakeOn);
	BIND_ACTION("WHEEL_BRAKE_OFF", WheelBrakeOff);
	BIND_ACTION("WHEEL_BRAKE_TOGGLE", WheelBrakeToggle);
	BIND_AXIS("WHEEL_BRAKE", setWheelBrake);
	BIND_ACTION("GEAR_UP", GearUp);
	BIND_ACTION("GEAR_DOWN", GearDown);
	BIND_ACTION("GEAR_TOGGLE", GearToggle);
	BIND_ACTION("MARKERS_TOGGLE", MarkersToggle);

	CSP_LOG(OBJECT, DEBUG, "... AircraftObject::AircraftObject()");
}


AircraftObject::~AircraftObject() {
}

void AircraftObject::pack(simdata::Packer& p) const {
	DynamicObject::pack(p);
	p.pack(m_ComplexPhysics);
	p.pack(m_AileronMin);
	p.pack(m_AileronMax);
	p.pack(m_ElevatorMin);
	p.pack(m_ElevatorMax);
	p.pack(m_RudderMin);
	p.pack(m_RudderMax);
	p.pack(m_AircraftDynamics);
}

void AircraftObject::unpack(simdata::UnPacker& p) {
	DynamicObject::unpack(p);
	p.unpack(m_ComplexPhysics);
	p.unpack(m_AileronMin);
	p.unpack(m_AileronMax);
	p.unpack(m_ElevatorMin);
	p.unpack(m_ElevatorMax);
	p.unpack(m_RudderMin);
	p.unpack(m_RudderMax);
	p.unpack(m_AircraftDynamics);
}

void AircraftObject::postCreate() {
	DynamicObject::postCreate();

	m_AircraftPhysicsModel = new AircraftPhysicsModel;
	m_AircraftPhysicsModel->bindObject(m_GlobalPosition, m_LinearVelocity, m_AngularVelocity, m_Attitude);
	m_AircraftPhysicsModel->setBoundingRadius(m_Model->getBoundingSphereRadius());
	m_AircraftPhysicsModel->setInertia(m_Mass, m_Inertia);

	m_PrimaryAeroDynamics = m_AircraftDynamics->getAeroDynamics();
	m_PrimaryAeroDynamics->setMassInverse(1.0 / m_Mass);
	m_AircraftPhysicsModel->addDynamics(m_PrimaryAeroDynamics.get());

	m_EngineDynamics = m_AircraftDynamics->getEngineDynamics();
	m_AircraftPhysicsModel->addDynamics(m_EngineDynamics.get());

	m_GearDynamics = m_AircraftDynamics->getGearDynamics();
	m_AircraftPhysicsModel->addDynamics(m_GearDynamics.get());

	std::vector<simdata::Vector3> const &contacts = m_Model->getContacts();
	if (!contacts.empty()) { 
		m_GroundCollisionDynamics = new GroundCollisionDynamics(m_Mass, contacts);
		m_AircraftPhysicsModel->addDynamics(m_GroundCollisionDynamics.get());
	}
}



void AircraftObject::dump() {
}

void AircraftObject::initialize() {
}

double AircraftObject::onUpdate(double dt)
{
	CSP_LOG(OBJECT, DEBUG, "AircraftObject::onUpdate ...");
	// TODO: update AI controller
	updateControls(dt);
	doFCS(dt);
	DynamicObject::onUpdate(dt);
	// TODO: update HUD?
	CSP_LOG(OBJECT, DEBUG, "... AircraftObject::onUpdate");
	return 0.0;
}


unsigned int AircraftObject::onRender() {
	// TODO: animation code here
	DynamicObject::onRender();
	if (m_SceneModel.valid())
		m_SceneModel->setSmokeEmitterLocation(m_EngineDynamics->getSmokeEmitterLocation());
	// FIXME only update if gear is in motion
	m_Model->updateGearSprites(m_GearDynamics->getGearPosition());
	return 0;
}

#define CLIP(a, b, c) (a) = ((a)<(b) ? (b) : ((a)>(c) ? (c) : (a)))

void AircraftObject::updateControls(double dt)
{
	CSP_LOG(OBJECT, DEBUG, "AircraftObject::updateControls ...");
	CSP_LOG(OBJECT, DEBUG, "AircraftObject::m_dThrottleInput = " << m_dThrottleInput);
	CSP_LOG(OBJECT, DEBUG, "AircraftObject::m_dAileronInput = " << m_dAileronInput);
	CSP_LOG(OBJECT, DEBUG, "AircraftObject::m_dElevatorInput = " << m_dElevatorInput);
	m_ThrottleInput += m_dThrottleInput * dt * 0.2;
	m_AileronInput += m_dAileronInput * dt * 0.4;
	m_ElevatorInput += m_dElevatorInput * dt * 0.4;

	if (m_BrakePulse > 0.0) {
		m_BrakePulse -= dt;
		if (m_BrakePulse < 0.0) m_BrakePulse = 0.0;
	} else {
		m_BrakePulse = 0.0;
	}

	// FIXME only need to do this if gear is extended and/or WOW
	if (m_GearDynamics->getExtended() && m_GearDynamics->getWOW()) {
		double braking = m_BrakeInput + m_BrakePulse;
		if (braking > 1.0) braking = 1.0;
		m_GearDynamics->setBraking(braking);
	}
	
	if (m_decayAileron > 0) {
		m_decayAileron--;
		if (m_dAileronInput == 0.0) m_AileronInput *= 0.90;
	}
	if (m_decayElevator > 0) {
		m_decayElevator--;
		if (m_dElevatorInput == 0.0) m_ElevatorInput *= 0.90;
	}
	CLIP(m_ThrottleInput, -1.0, 1.0);
	CLIP(m_AileronInput, -1.0, 1.0);
	CLIP(m_ElevatorInput, -1.0, 1.0);
	CSP_LOG(OBJECT, DEBUG, "AircraftObject::m_decayElevator = " << m_decayElevator);
	CSP_LOG(OBJECT, DEBUG, "AircraftObject::m_ElevatorInput = " << m_ElevatorInput);
	CSP_LOG(OBJECT, DEBUG, "... AircraftObject::updateControls");
}

void AircraftObject::doFCS(double dt)
{
	CSP_LOG(OBJECT, DEBUG, "AircraftObject::doFCS ...");
	// FIXME: very temporary fcs mappings
	m_Rudder = m_RudderInput * m_RudderMax * 0.017;
	m_Aileron = m_AileronInput * m_AileronMax * 0.017;
	m_Elevator = m_ElevatorInput * m_ElevatorMax * 0.017;
	m_Throttle = m_ThrottleInput;
	CSP_LOG(OBJECT, DEBUG, " ... AircraftObject::doFCS");
}

void AircraftObject::setThrottle(double x) { 
	m_ThrottleInput = 0.5 *(1.0 - x); 
}

void AircraftObject::setRudder(double x)
{ 
	m_RudderInput = x; 
	m_GearDynamics->setSteering(x);
}

void AircraftObject::setAileron(double x)
{ 
	m_AileronInput = x; 
}

void AircraftObject::setElevator(double x)
{ 
	CSP_LOG(OBJECT, DEBUG, "AircraftObject::setElevator ...");
	m_ElevatorInput = x; 
	CSP_LOG(OBJECT, DEBUG, "m_ElevatorInput = " << m_ElevatorInput << " ...AircraftObject::setElevator");
}

void AircraftObject::IncElevator() { 
	CSP_LOG(OBJECT, DEBUG, "AircraftObject::IncElevator ...");
	m_dElevatorInput = 1.0; 
	CSP_LOG(OBJECT, DEBUG, "... AircraftObject::IncElevator");
}	

void AircraftObject::noIncElevator() { 
	CSP_LOG(OBJECT, DEBUG, "AircraftObject::noIncElevator ...");
	if (m_dElevatorInput > 0.0) m_dElevatorInput = 0.0; 
	m_decayElevator = 30;
	CSP_LOG(OBJECT, DEBUG, "... AircraftObject::noIncElevator");
}

void AircraftObject::DecElevator() { 
	CSP_LOG(OBJECT, DEBUG, "AircraftObject::DecElevator ...");
	m_dElevatorInput = -1.0; 
	CSP_LOG(OBJECT, DEBUG, "... AircraftObject::DecElevator");
}

void AircraftObject::noDecElevator() { 
	CSP_LOG(OBJECT, DEBUG, "AircraftObject::noDecElevator ...");
	if (m_dElevatorInput < 0.0) m_dElevatorInput = 0.0; 
	m_decayElevator = 30;
	CSP_LOG(OBJECT, DEBUG, "... AircraftObject::noDecElevator");
}

void AircraftObject::IncAileron() { 
	m_dAileronInput = 1.0; 
}

void AircraftObject::noIncAileron() { 
	if (m_dAileronInput > 0.0) m_dAileronInput = 0.0; 
	m_decayAileron = 30;
}

void AircraftObject::DecAileron() { 
	m_dAileronInput = -1.0; 
}

void AircraftObject::noDecAileron() { 
	if (m_dAileronInput < 0.0) m_dAileronInput = 0.0; 
	m_decayAileron = 30;
}

void AircraftObject::IncThrottle() { 
	m_dThrottleInput = 1.0; 
}

void AircraftObject::noIncThrottle() { 
	if (m_dThrottleInput > 0.0) m_dThrottleInput = 0.0; 
}

void AircraftObject::DecThrottle() { 
	m_dThrottleInput = -1.0; 
}

void AircraftObject::noDecThrottle() { 
	if (m_dThrottleInput < 0.0) m_dThrottleInput = 0.0; 
}

void AircraftObject::SmokeOn() { 
	enableSmoke();
}

void AircraftObject::SmokeOff() { 
	disableSmoke();
}

void AircraftObject::SmokeToggle() { 
	if (isSmoke()) {
		disableSmoke();
	} else {
		enableSmoke();
	}
}

void AircraftObject::WheelBrakePulse() { 
	m_BrakePulse = 2.0;
}

void AircraftObject::WheelBrakeOn() {
	m_BrakeInput = 1.0;
}

void AircraftObject::WheelBrakeOff() {
	m_BrakeInput = 0.0;
}

void AircraftObject::WheelBrakeToggle() {
	if (m_BrakeInput < 0.5) {
		m_BrakeInput = 1.0;
	} else {
		m_BrakeInput = 0.0;
	}
}

void AircraftObject::setWheelBrake(double x) {
	m_BrakeInput = x;
}

void AircraftObject::setGearStatus(bool on) {
	if (on) {
		m_GearDynamics->Extend();
	} else {
		m_GearDynamics->Retract();
	}
	m_Model->showGearSprites(on);
}
	
void AircraftObject::GearUp() {
	setGearStatus(false);
}

void AircraftObject::GearDown() {
	setGearStatus(true);
}

void AircraftObject::GearToggle() {
	setGearStatus(!m_GearDynamics->getExtended());
}

void AircraftObject::MarkersToggle() {
	m_Model->showContactMarkers(!m_Model->getMarkersVisible());
}

void AircraftObject::setAttitude(double pitch, double roll, double heading)
{
	simdata::Quaternion attitude;
	m_Pitch = pitch;
	m_Roll = roll;
	m_Heading = heading;
	
	/*attitude = simdata::Quaternion::MakeQFromEulerAngles( m_Pitch, 
														  m_Roll,   
	                                                     m_Heading);*/

	// use standard Euler convension (X axis is roll, Y is pitch, Z is yaw).
	attitude = simdata::Quaternion::MakeQFromEulerAngles(m_Roll, m_Pitch, -m_Heading);
	DynamicObject::setAttitude(attitude);
}

void AircraftObject::doMovement(double dt)
{
	DynamicObject::doMovement(dt);

	if (isHuman()) {
		if (m_ComplexPhysics) {
			doComplexPhysics(dt);
		} else {
			doSimplePhysics(dt);
		}
	} else {
		//	doSimplePhysics(dt);
		//doComplexPhysics(dt);
	}

	///updateTransform();

	// modified Euler angles are the CSP frame (X is pitch, Y is roll, Z is -yaw).
	simdata::Vector3 angles = simdata::Quaternion::MakeModifiedEulerAnglesFromQ(m_Attitude);
	
	m_Pitch = angles.x;
	m_Roll = angles.y;
	m_Heading = angles.z;
	m_Speed = m_LinearVelocity.Length();
}


void AircraftObject::doSimplePhysics(double dt)
{
/*
	double sensitivity = 1.0;
	double planesensx = 0.3;
	double planesensy = 0.5;
	double plusplus = 1.0;

	// FIXME!
	simdata::Vector3 m_CurrentNormDir;

	// Save the objects old position
	m_PrevPosition = m_LocalPosition;

	// update the orientation matrix based on control surfaces;
	double pitchrate = m_Elevator * sensitivity * planesensx * plusplus * dt;
	double rollrate = m_Aileron * sensitivity * planesensy * plusplus * dt;
	// rudder effect is limited by speed
	double yawrate = m_Rudder * sensitivity * planesensy * plusplus * dt * 10 / sqrt(1+m_Speed);

	simdata::Matrix3 rotZ;
	rotZ.FromAxisAngle (m_CurrentNormDir, - yawrate - rollrate); 

	simdata::Matrix3 rotY;
	rotY.FromAxisAngle (rotZ * m_Direction, 2 * rollrate + 0.25*yawrate ); 

	simdata::Matrix3 rotX;
	rotX.FromAxisAngle( rotY * rotZ * (m_Direction^m_CurrentNormDir), pitchrate); 

	// update orientation matrix
	m_Orientation = rotX * rotY * rotZ * m_Orientation;

	// update direction
	m_Direction = m_Orientation * m_InitialDirection; //+ 0.5 * dt * m_GravityForce;

	m_AngleOfAttack = angleBetweenTwoVectors(m_Direction, simdata::Vector3(m_Direction.x,m_Direction.y,0.0));

	// update normal vector to wings aircraft plane
	m_CurrentNormDir = m_Orientation * m_InitialNormDir;

	m_Speed = 200.0;
	m_Speed += dt * m_Thrust * m_Speed / (sqrt(m_Speed) + 1.0); 

	// update linear velocity and position
	m_Direction.Normalize(); 
	m_LinearVelocity = m_Speed * m_Direction;
	//  setVelocity(m_LinearVelocity);
	m_LocalPosition += dt * m_LinearVelocity;
*/
}

void AircraftObject::doComplexPhysics(double dt) {
	m_PrimaryAeroDynamics->setControlSurfaces(m_Aileron, m_Elevator, m_Rudder);
	// FIXME: calibrate joystick positions for csp
	m_EngineDynamics->setThrottle(2.0 * m_Throttle);

	//preset various aircraft dimensions and landing/takeoff parameters:
	//  pitch and roll limits
	//  gear vectors and state
	//  gear spring constants and damping
	//  gear travel and force limits
	//flightmodel needs to return out of spec parameters for subsequent
	//damage modelling.

	m_PrimaryAeroDynamics->setControlSurfaces(m_Aileron, m_Elevator, m_Rudder);
	m_AircraftPhysicsModel->setGroundZ(m_GroundZ);
	m_AircraftPhysicsModel->setGroundN(m_GroundN);
	m_AircraftPhysicsModel->doSimStep(dt);
}

void AircraftObject::getStats(std::vector<std::string> &stats) const {
	DynamicObject::getStats(stats);
	char buffer[256];
	snprintf(buffer, 255, "Throttle: %.3f", m_Throttle);
	stats.push_back(buffer);
	snprintf(buffer, 255, "Elevator: %.3f, Aileron %.3f, Rudder %.3f",
	         m_Elevator, m_Aileron, m_Rudder);
	stats.push_back(buffer);
	float speed = getSpeed();
	float alpha = getAngleOfAttack();
	float G = getGForce();
	snprintf(buffer, 255, "Alpha: %+.2f, G: %+.2f, Air speed: %.1f", RadiansToDegrees(alpha), G, speed);
	stats.push_back(buffer);
	float heading = RadiansToDegrees(m_Heading);
	if (heading < 0.0) heading += 360.0;
	snprintf(buffer, 255, "Heading: %.3f, Pitch: %.3f, Roll: %.3f",
	         heading,
	         RadiansToDegrees(m_Pitch), 
	         RadiansToDegrees(m_Roll));
	stats.push_back(buffer);
}


SIMDATA_REGISTER_INTERFACE(AircraftDynamics)

void AircraftDynamics::pack(simdata::Packer& p) const {
	simdata::Object::pack(p); 
    p.pack(m_PrimaryAeroDynamics); 
	p.pack(m_EngineDynamics);
    p.pack(m_GearDynamics);
}

void AircraftDynamics::unpack(simdata::UnPacker& p) {
	simdata::Object::unpack(p); 
    p.unpack(m_PrimaryAeroDynamics); 
	p.unpack(m_EngineDynamics);
    p.unpack(m_GearDynamics);
}


























#if 0
/*
void AircraftObject::initialize()
{
  if (m_sObjectName == "PLAYER" )
    initializeHud();
}

void AircraftObject::initializeHud()
{
  m_phud = new Hud(m_InitialDirection);
  m_phud->BuildHud("../Data/Scripts/HudM2k.csp");
  m_rpSwitch->addChild(m_phud);
}

void AircraftObject::dump()
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

void AircraftObject::OnUpdate(double dt)
{
   CSP_LOG(OBJECT, DEBUG, "AircraftObject::OnUpdate ...");
    {
        if (m_pController)
          m_pController->OnUpdate(dt);

        if ( m_sObjectName == "PLAYER" )
         g_pPlayerInput->OnUpdate();

        doMovement(dt);

		if ( m_sObjectName == "PLAYER" )
		  m_phud->OnUpdate();
    }
   CSP_LOG(OBJECT, DEBUG, "... AircraftObject::OnUpdate");
	
}

int AircraftObject::updateScene()
{
	BaseObject::updateScene();
	return 0;
}

unsigned int AircraftObject::OnRender()
{
//  updateScene();
  return 0;
}


void AircraftObject::doMovement(double dt)
{
    if (m_bComplex)
        doComplexPhysics(dt);
    else
        doSimplePhysics(dt);

	updateLocalPosition();
	updateGlobalPosition();

    m_Orientation.ToEulerAnglesZXY(m_heading, m_pitch, m_roll);

    m_heading = RadiansToDegrees( m_heading );
    m_pitch = RadiansToDegrees ( m_pitch );
    m_roll = RadiansToDegrees( m_roll );
}

void AircraftObject::doSimplePhysics(double dt)
{
  // Save the objects old position
  m_PrevPosition = m_LocalPosition;

  // update the orientation matrix based on control surfaces;
  double pitchrate = (m_Elevator) * sensativity * planesensx * plusplus * dt;
  double rollrate = (m_Aileron) * sensativity * planesensy * plusplus * dt;
  // rudder effect is limited by speed
  double yawrate = (m_Rudder) * sensativity * planesensy * plusplus * dt * 10 / sqrt(1+m_Speed);
  
  simdata::Matrix3 rotZ;
  rotZ.FromAxisAngle (m_CurrentNormDir, - yawrate - rollrate); 

  simdata::Matrix3 rotY;
  rotY.FromAxisAngle (rotZ * m_Direction, 2 * rollrate + 0.25*yawrate ); 

  simdata::Matrix3 rotX;
  rotX.FromAxisAngle( rotY * rotZ * (m_Direction^m_CurrentNormDir), pitchrate); 

  // update orientation matrix
  m_Orientation = rotX * rotY * rotZ * m_Orientation;
  
  // update direction
  m_Direction = m_Orientation * m_InitialDirection; //+ 0.5 * dt * m_GravityForce;
  
  m_AngleOfAttack = angleBetweenTwoVectors(m_Direction, simdata::Vector3(m_Direction.x,m_Direction.y,0.0));

  // update normal vector to wings aircraft plane
  m_CurrentNormDir = m_Orientation * m_InitialNormDir;

  m_Speed = 200.0;
  m_Speed += dt * m_Thrust * m_Speed / (sqrt(m_Speed) + 1.0); 
  
  // update linear velocity and position
  m_Direction.Normalize(); 
  m_LinearVelocity = m_Speed * m_Direction;
//  setVelocity(m_LinearVelocity);
  m_LocalPosition += dt * m_LinearVelocity;

  CSP_LOG(OBJECT, CSP_TRACE, "AircraftObject::SimplePhysics: ObjectID: " << m_iObjectID 

      << ", Elevator: " << m_Elevator <<
                  ", Aileron: " << m_Aileron << ", PitchRate: " << pitchrate <<
                  ", RollRate: " << yawrate );
  CSP_LOG(OBJECT, CSP_TRACE, "AircraftObject::SimplePhysics: Position: " << m_LocalPosition <<
                  ", Direction: " << m_Direction << ", Up: " << m_CurrentNormDir <<
                  ", Velocity: " << m_LinearVelocity );
  CSP_LOG(OBJECT, CSP_TRACE, "AircraftObject::SimplePhysics: Orientation: " << std::endl << m_Orientation);
}

void AircraftObject::doComplexPhysics(double dt)
{
    CSP_LOG(OBJECT, CSP_TRACE, "AircraftObject::doComplexPhysics...");

    if (!m_bPhysicsInitialized)
    {
        m_pAircraftPhysics->Initialize();
	    m_pAircraftPhysics->setVelocity(m_LinearVelocity);
        m_pAircraftPhysics->setSpeed(m_Speed);
        m_pAircraftPhysics->qOrientation.FromRotationMatrix(m_Orientation);
        m_bPhysicsInitialized = true;
    }
    
    m_pAircraftPhysics->setPosition(m_LocalPosition);
    m_pAircraftPhysics->setThrottle(m_Throttle);
    m_pAircraftPhysics->setAileron(m_Aileron);
    m_pAircraftPhysics->setElevator(m_Elevator);
    m_pAircraftPhysics->setRudder(m_Rudder);

    m_pAircraftPhysics->DoSimStep(dt);

    m_pAircraftPhysics->qOrientation.ToRotationMatrix(m_Orientation);

	// testing
	m_qOrientation = m_pAircraftPhysics->qOrientation;

    m_Direction = m_Orientation * m_InitialDirection;
    m_CurrentNormDir = m_Orientation * m_InitialNormDir;

    m_LocalPosition = m_pAircraftPhysics->m_PositionLocal;

    m_LinearVelocity = m_pAircraftPhysics->m_VelocityLocal;
	m_Speed = m_pAircraftPhysics->m_fSpeedLocal;
	m_AngleOfAttack = m_pAircraftPhysics->getAngleOfAttack();
	setGForce(m_pAircraftPhysics->m_fgForce);

    //m_Force = m_pAircraftPhysics->m_ForcesBody;
    //m_Torque = m_pAircraftPhysics->m_Moments;
	CSP_LOG(OBJECT, DEBUG, "...AircraftObject::doComplexPhysics");
	CSP_LOG(OBJECT, DEBUG, "");
}

*/
#endif



/*
double AircraftObject::getElevator() const
{
	return m_Elevator;
}

double AircraftObject::getAileron() const
{
	return m_Aileron;
}

double AircraftObject::getRudder() const
{
	return m_Rudder;
}
*/

/*
void AircraftObject::setThrust(double setting)
{
	m_Thrust = setting * m_ThrottleFactor;
}

double AircraftObject::getThrust() const
{
	return m_Thrust;
}
*/

/*
void AircraftObject::setThrottle(double setting)
{
	m_Throttle = setting;
//	setThrust(setting);
}

double AircraftObject::getThrottle() const
{
	return m_Throttle;
}
*/
