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
 * @file LandingGear.cpp
 *
 **/

/*
 * TODO
 *
 *  Aerodynamic drag
 *  Wind damage
 *  Extension/retract modelling + 3D hooks
 *  Rough terrain damage
 *  Generic damage modelling, gear collapse,  etc.
 *
 */

#include <LandingGear.h>
#include <KineticsChannels.h>

#include <SimData/Math.h>

#include <cstdio>
#include <iostream>

using bus::Kinetics;

using simdata::toRadians;
using simdata::dot;
using simdata::Vector3;


SIMDATA_REGISTER_INTERFACE(LandingGear)
SIMDATA_REGISTER_INTERFACE(GearDynamics)


LandingGear::LandingGear() {
	m_Chained = false;
	m_K = 0.0;
	m_Beta = 0.0;
	m_BrakeSteeringLinkage = 0.0;
	m_BrakeSteer = 0.0;
	m_SteerAngle = 0.0;
	m_SteeringLimit = 0.0;
	m_TireK = 200000.0;
	m_TireBeta = 500.0;
	m_Damage = 0.0;
	m_Extended = true;
	m_Compression = 0.0;
	m_CompressionLimit = 0.0;
	m_WOW = false;
	m_Touchdown = false;
	m_Skidding = 0.0;
	m_Brake = 0.0;
	m_BrakeSetting = 0.0;
	m_BrakeLimit = 50000.0;
	m_BrakeFriction = 1.0;
	m_BrakeSlip = 0.8;
	m_BrakeTemperature = 300.0;
	m_RollingFriction = 1000.0;
	m_TireFriction = 0.8;
	m_TireSkidFriction = 0.6;
	m_TireStaticFriction = 1.0;
	m_ABS = true;
	m_NormalForce = Vector3::ZERO;
	m_TangentForce = Vector3::ZERO;
	m_TouchdownPoint = Vector3::ZERO;
	m_TireContactPoint = Vector3::ZERO;
	m_SteerTransform = simdata::Quat::IDENTITY;
	m_Skidding = false;
	m_SkidFlag = false;
	m_Touchdown = false;
	m_TireRotation = 0.0;
	m_TireRotationRate = 0.0;
	m_TireRadius = 0.25;
}

#if 0
LandingGear::LandingGear(LandingGear const &g) {
	*this = g;
}

LandingGear const &LandingGear::operator=(LandingGear const &g) {
	if (&g != this) {
		m_MaxPosition = g.m_MaxPosition;
		m_Position = g.m_Position;
		m_Motion = g.m_Motion;
		m_Chained = g.m_Chained;
		m_K = g.m_K;
		m_Beta = g.m_Beta;
		m_BrakeLimit = g.m_BrakeLimit;
		m_StaticFriction = g.m_StaticFriction;
		m_DynamicFriction = g.m_DynamicFriction;
		m_SteeringLimit = g.m_SteeringLimit;
		m_TireK = g.m_TireK;
		m_TireBeta = g.m_TireBeta;
		m_Damage = g.m_Damage;
		m_Extended = g.m_Extended;
		m_TireShift = g.m_TireShift;
		m_WOW = g.m_WOW;
		m_Compression = g.m_Compression;
		m_CompressionLimit = g.m_CompressionLimit;
		m_Touchdown = g.m_Touchdown;
		m_Skidding = g.m_Skidding;
		m_Steer = g.m_Steer;
		m_Brake = g.m_Brake;
		m_BrakeSetting = g.m_BrakeSetting;
	}
	return g;
}
#endif

void LandingGear::postCreate() {
	Object::postCreate();
	m_Motion.normalize();
	m_Position = m_MaxPosition;
}

Vector3 LandingGear::simulateSubStep(Vector3 const &origin,
                                              Vector3 const &vBody,
                                              simdata::Quat const &q, 
                                              double height, 
                                              Vector3 const &normalGroundBody) 
{
	if (!m_Extended) return Vector3::ZERO;
	
	resetForces();

	// do suspension physics to get normal force
	updateSuspension(origin, vBody, q, height, normalGroundBody);
	
	// if not in contact with the ground, return early
	if (m_Compression <= 0.0) return Vector3::ZERO;
	
	// do wheel physics to get tangent force, but don't update the contact point
	updateWheel(0.0, origin, vBody, q, normalGroundBody, false);

	// return the total force (body coordinates)
	return m_NormalForce + m_TangentForce;
}


void LandingGear::setBraking(double setting) { 
	if (setting < 0.0) setting = 0.0;
	if (setting > 1.0) setting = 1.0;
	m_BrakeSetting = setting; 
}

double LandingGear::setSteering(double setting, double link_brakes) {
	if (setting > 1.0) setting = 1.0;
	if (setting < -1.0) setting = -1.0;
	m_BrakeSteer = setting * link_brakes * m_BrakeSteeringLinkage;
	m_SteerAngle = setting * m_SteeringLimit;
	double rad = toRadians(m_SteerAngle);
	m_SteerTransform.makeRotate(rad, Vector3::ZAXIS);
	return m_SteerAngle;
}

double LandingGear::getDragFactor() const {
	// XXX very temporary hack (need xml, partial extension, etc)
	if (m_Extended) return 1.0; // just a random value
	return 0.0;
}


void LandingGear::preSimulationStep(double dt) {
	if (!m_Extended) return;
	// XXX anything to do here?
}


/**
 * Update the brake caliper in response to brake input.
 */
void LandingGear::updateBraking(double dt) {
	// update actual brake power from the input brake setting and
	// steering linkage (low pass filtered, XXX ad-hoc time constant)
	double f = dt*5.0;
	if (f > 1.0) f = 1.0;
	m_Brake = m_Brake * (1.0-f) + (m_BrakeSetting + m_BrakeSteer) * f;
	if (m_Brake < 0.0) m_Brake = 0.0;
	if (m_Brake > 1.0) m_Brake = 1.0;
}


/**
 * Update Weight-On-Wheels flag and record touchdown point
 */
void LandingGear::updateWOW(Vector3 const &origin, simdata::Quat const &q) {
	if (m_Compression > 0.0) {
		// first contact? 
		if (!m_WOW) {
			// yes, flag the touchdown
			m_Touchdown = true;
			m_TouchdownPoint = origin + q.rotate(m_Position);
			m_WOW = true;
		}
	} else {
		m_WOW = false;
	}
}

/**
 * Simulate the main shock adsorber to determine the normal force and
 * gear compression.
 *
 * @param origin body coordinate origin in local coordinates
 * @param vBody strut velocity in body coordinates
 * @param q body orientation
 * @param height height of body origin above ground
 * @param normalGroundBody ground normal vector in body coordinates
 */
void LandingGear::updateSuspension(Vector3 const &origin, 
                                   Vector3 const &vBody, 
                                   simdata::Quat const &q, 
                                   double const height, 
                                   Vector3 const &normalGroundBody) 
{
	double compression = 0.0;
	double motionNormal = dot(m_Motion, normalGroundBody);
	if (motionNormal > 0.0) { 
		compression = - (dot(m_MaxPosition, normalGroundBody) + height) / motionNormal;
	}

	// at (or past) max extension?
	if (compression <= 0.0) {
		// limit travel and clear weight-on-wheels (WOW)
		m_Compression = 0.0;
		m_Position = m_MaxPosition;
		m_WOW = false;
		// no ground reaction force
	} else {
		// are we overcompressed?
		if (compression >= m_CompressionLimit) {
			m_Compression = m_CompressionLimit;
			// TODO: break the gear!
		} else {
			m_Compression = compression;
		}

		// update wheel position
		m_Position = m_MaxPosition + m_Motion * m_Compression;

		// determine reaction force
		//
		// FIXME in computing vCompression, only the normal force is taken into 
		// account, but other components can matter if m_Motion isn't vertical. 
		// (e.g. when brakes are applied)
	
		// calculate strut compression speed
		double vCompression = - dot(vBody, normalGroundBody) * motionNormal;
		// restrict to reasonable limits (faster than this means the gear will 
		// probably break in a moment anyway)
		vCompression = std::min(std::max(vCompression, -10.0), 10.0);
		// ground support (in response to strut compression + damping)
		double normalForce = (m_K * m_Compression + m_Beta * vCompression) * motionNormal;
		if (normalForce < 0.0) normalForce = 0.0; // wheel hop
		m_NormalForce += normalForce * normalGroundBody;
	}
}

void LandingGear::resetForces() {
	m_NormalForce = Vector3::ZERO;
	m_TangentForce = Vector3::ZERO;
}

void LandingGear::postSimulationStep(double dt,
                                     Vector3 const &origin, 
                                     Vector3 const &vBody,
                                     simdata::Quat const &q, 
				     double const height,
                                     Vector3 const &normalGroundBody) {
	if (!m_Extended) return;
	resetForces();
	// update order matters
	updateBraking(dt);
	updateSuspension(origin, vBody, q, height, normalGroundBody);
	updateWOW(origin, q);
	updateWheel(dt, origin, vBody, q, normalGroundBody, true);
}


/**
 * Update the ground-tire contact point, detect skidding, and set friction coefficients.
 */
void LandingGear::updateWheel(double dt,
                              Vector3 const &origin, 
                              Vector3 const &vBody,
                              simdata::Quat const &q, 
                              Vector3 const &normalGroundBody,
                              bool updateContact) 
{
	static double XXX_t = 0.0;
	if (updateContact) XXX_t += dt;

	Vector3 tirePositionLocal = origin + q.rotate(m_Position);

	// not in contact
	if (m_Compression <= 0.0) {
		if (updateContact) {
			m_Skidding = false;
			m_TireContactPoint = tirePositionLocal;
		}
		return;
	}

	// compute tire deformation and reaction
	Vector3 tireDeformation = tirePositionLocal - m_TireContactPoint;
	Vector3 tireForce = - tireDeformation * m_TireK;

	// switch to body coordinates
	Vector3 tireForceBody = q.invrotate(tireForce);
	// project onto the ground 
	tireForceBody -= dot(tireForceBody, normalGroundBody) * normalGroundBody;
	// transform to wheel coordinates
	Vector3 tireForceWheel = m_SteerTransform.rotate(tireForceBody);

	Vector3 XXX_tfb = tireForceBody;

	// note we are assuming here that the steering axis is very close to the
	// normal axis.  under normal circumstances this should be approximately 
	// true for any wheel that can be steered.

	// normal force on tire/ground interface
	double normalForce = m_NormalForce.length();
	// maximum braking force (neglects axle friction)
	double brakeLimit = m_BrakeFriction * m_Brake * m_BrakeLimit;

	// check if brakes are slipping
	double alignedForce = sqrt(tireForceWheel.y() * tireForceWheel.y() + tireForceWheel.z() * tireForceWheel.z());
	bool brakeSlip = alignedForce > brakeLimit;
	if (brakeSlip) {
		if (updateContact) {
			// slipping so use the slipping friction until slipping stops
			// if the coefficient has changed, don't worry about changing brakeLimit
			// now, it'll be corrected on the next update
			m_BrakeFriction = m_BrakeSlip;
		}
		// reduce the aligned force to account for brake slippage
		double scale = brakeLimit / alignedForce;
		alignedForce *= scale;
		tireForceWheel.y() *= scale;
		tireForceWheel.z() *= scale;
	} else {
		if (updateContact) {
			// not slipping so use the static friction from now on
			m_BrakeFriction = 1.0;
		}
	}

	// ground velocity in body coordinates
	Vector3 vGroundBody = vBody - dot(vBody, normalGroundBody) * normalGroundBody;
	// transform to wheel coordinates
	Vector3 vGroundWheel = m_SteerTransform.rotate(vGroundBody);
	// normalize to get rolling direction
	Vector3 rollingDirectionWheel = Vector3(0.0, vGroundWheel.y(), vGroundWheel.z()).normalized();
	// compute rolling friction
	Vector3 rollingFrictionWheel = - m_RollingFriction * normalForce * rollingDirectionWheel;
	// add rolling friction to tire force XXX (trying this below!)
	// XXX tireForceWheel += rollingFrictionWheel;

	// is this wheel steerable?
	if (m_SteeringLimit > 0.0) {
		// partial (approx) castering of steerable wheels to reduce instability
		tireForceWheel.x() *= (0.1 + 0.9 * fabs(m_SteerAngle/m_SteeringLimit));
	}

	// maximum traction
	double skidLimit = m_TireFriction * normalForce;
	// total (virtual) tire force due to braking and sideslip
	double totalTireForce = tireForceWheel.length();

	// FIXME wheels currently have zero inertia.  the current implementation
	// isn't well suited to modelling wheel spin, and only very crude tracking
	// of wheel rotation is done.  the code should be redesigned at some point
	// to model braking and ground contact forces as torques acting on wheel 
	// rotation, and thereby accounting for wheel inertia, etc.

	// check if tire is skidding
	bool skidding = totalTireForce > skidLimit;
	if (skidding) {
		if (updateContact) {
			m_SkidFlag = true;
			m_Skidding = true;
			// use skidding friction until the skidding stops
			m_TireFriction = m_TireSkidFriction;
			// momentarily release brakes to stop the skid
			if (m_ABS) {
				m_Brake = 0.0;
			}
			// XXX see note above
			m_TireRotationRate = 0.0;
		}
		// reduce total force (reevaluate skidLimit with skidding friction)
		tireForceWheel *= m_TireFriction * normalForce / totalTireForce;
		// convert back to body coordinates
		tireForceBody = m_SteerTransform.invrotate(tireForceWheel);
	} else {
		if (!brakeSlip) {
			// tire is fixed and brakes are locked, so we need to damp the
			// tire vibration.  in principle the damping term should be
			// included in the skid test, but this is probably not a very
			// important detail.
			tireForceWheel += - m_TireBeta * vGroundWheel;
		} else {
			tireForceWheel += rollingFrictionWheel;
		}
		// convert back to body coordinates
		tireForceBody = m_SteerTransform.invrotate(tireForceWheel);
		if (updateContact) {
			m_Skidding = false;
			// tire isn't skidding, so use static friction from now on
			m_TireFriction = m_TireStaticFriction;
			// XXX see note above
			double rollingSpeed = sqrt(vGroundWheel.y()*vGroundWheel.y() + vGroundWheel.z()*vGroundWheel.z());
			if (brakeSlip) {
				m_TireRotationRate = rollingSpeed / m_TireRadius;
				m_TireRotation += m_TireRotationRate * dt;
			} else {
				m_TireRotationRate = 0.0;
			}
		}
	}

	// move the contact point during postSimulationStep() only if
	// the wheel is rolling or skidding
	if (updateContact && (skidding || brakeSlip)) {
		// convert back to local coordinates
		tireForce = q.rotate(tireForceBody);
		// update tire contact point
		m_TireContactPoint = tirePositionLocal + tireForce / m_TireK;
	}

	if (updateContact) { 
		static double t = 0.0;
		static bool reset = false;
		if (m_BrakeLimit == 0.0) reset = true;
		if (m_BrakeLimit > 0.0 && reset) {
			reset = false;
			if (t == 0.0) { std::cerr << "time\tcontact\tvbody_y\tvgrndw_y\tforceb_y\tskid\tslip\ttire_grip\tbrake_grip\n"; }
			t += dt;
			std::cerr << t << "\t" << m_TireContactPoint.y() << "\t" << vBody.y() << "\t" << vGroundWheel.y() << "\t" << tireForceBody.y() << "\t" << (m_Skidding*1000.0) << "\t" << (brakeSlip*1000.0) << "\t" << skidLimit << "\t" << brakeLimit << "\n";
		}
	}

	// model brake temperature (very ad-hoc at the moment)
	if (updateContact && m_BrakeLimit > 0.0) {
		// XXX if skidding, the dissipation will be nearly zero since the wheel
		// is locked.  we should model tire damage under this condition, so that
		// blowouts can result from extended skids.
		double dissipation = fabs(alignedForce * m_TireRotationRate * m_TireRadius);
		double T2 = m_BrakeTemperature * m_BrakeTemperature;
		double R2 = m_TireRadius * m_TireRadius;
		// approximate brake disc surface area (2 sides)
		double surfaceArea = R2 * 4.0;
		// compute black body radiation (approximating emmisivity and surface area)
		double radiation = 5.7e-8 * (T2*T2-8.1e+9) * surfaceArea;
		// total guess
		double aircooling = 30.0 * (m_BrakeTemperature - 300.0) * surfaceArea * (vBody.length() + 1.0);
		// arbitrary universal scaling (approximate volume as R3, specific heat 
		// near that of steel) 
		double heatCapacity = R2 * m_TireRadius * 2e+6; 
		m_BrakeTemperature += (dissipation - radiation - aircooling) / heatCapacity * dt;
		static int yyy = 0; yyy++;
		if (yyy % 240 == 1) {
			if (m_Skidding) {
				std::cout << "SKID\n";
			} else 
			if (brakeSlip) {
				std::cout << "SLIP\n";
			} else {
				std::cout << "NOSLIP: brakelimit " << brakeLimit << " > " << alignedForce << "?\n";
			}
			std::cout << "BRAKE T=" << m_BrakeTemperature << "\n";
		}
	}

	if (0 && XXX_t > 90.0 && (tireForceBody-XXX_tfb).length() > 0.001) {
		std::cout << "DIFFERENCE: " << (tireForceBody-XXX_tfb).asString() << " " << skidding << " " << brakeSlip << ", " << tireForceBody << " " << brakeLimit << " " << m_Brake << " " << m_BrakeFriction << m_BrakeLimit << "\n";
	}
	if (tireForceBody.length() > 50000.0) {
		std::cout << "TFB ---> " << tireForceBody.asString() << "\n";
		std::cout << "DEF ---> " << tireDeformation.asString() << "\n";
		std::cout << "TFB ---> " << XXX_tfb.asString() << "\n";
		std::cout << "VGW ---> " << vGroundWheel.asString() << "\n";
	}
	m_TangentForce += tireForceBody;
	//m_TangentForce += XXX_tfb;
}


void GearDynamics::doComplexPhysics(double x) {
	m_Force = m_Moment = Vector3::ZERO;
	if (!m_Extended) return;
	Vector3 airflow_body = m_WindVelocityBody - *m_VelocityBody;
	double airspeed = airflow_body.length();
	Vector3 dynamic_pressure = 0.5 * (b_Density->value()) * airflow_body * airspeed;
	size_t n = m_Gear.size();
	for (size_t i = 0; i < n; ++i) {
		LandingGear &gear = *(m_Gear[i]);
		Vector3 R = gear.getPosition();
		Vector3 F = Vector3::ZERO;
		if (b_NearGround->value()) {
			Vector3 vBody = *m_VelocityBody + (*m_AngularVelocityBody ^ R);
			F += gear.simulateSubStep(*m_PositionLocal, 
			                          vBody,
			                          *m_Attitude,
			                          m_Height,
			                          m_GroundNormalBody);
		}
		// TODO torque position should depend on extension
		F += gear.getDragFactor() * dynamic_pressure;
		m_Force += F;
		m_Moment += (R ^ F);
	}
}

//FIXME: just for some testing purpose
void GearDynamics::setStatus(bool on) {
	size_t n = m_Gear.size();
	for (size_t i = 0; i < n; ++i) {
		m_Gear[i]->setExtended(on);
	}
	m_Extended = on;
	// XXX TEMPORARY HACK
	b_GearExtended->value() = on;
}

GearDynamics::GearDynamics():
	m_Extended(true),
	m_Height(0.0)
{ 
	BIND_ACTION("GEAR_UP", GearUp);
	BIND_ACTION("GEAR_DOWN", GearDown);
	BIND_ACTION("GEAR_TOGGLE", GearToggle);
}

void GearDynamics::registerChannels(Bus *bus) {
	assert(bus!=0);
	b_WOW = bus->registerLocalDataChannel<bool>("State.WOW", false);
	// XXX TEMPORARY HACKS
	b_GearPosition = bus->registerLocalDataChannel("Animation.GearPosition", std::vector<simdata::Vector3>());
	b_GearExtended = bus->registerLocalDataChannel<bool>("Animation.GearExtended", true);
}

void GearDynamics::importChannels(Bus *bus) {
	assert(bus!=0);
	b_LeftBrakeInput = bus->getChannel("ControlInputs.LeftBrakeInput");
	b_RightBrakeInput = bus->getChannel("ControlInputs.RightBrakeInput");
	b_SteeringInput = bus->getChannel("ControlInputs.RudderInput");
	b_Density = bus->getChannel("Conditions.Density");
	b_WindVelocity = bus->getChannel("Conditions.WindVelocity");
	b_NearGround = bus->getChannel(Kinetics::NearGround);
	b_GroundN = bus->getChannel(Kinetics::GroundN);
	b_GroundZ = bus->getChannel(Kinetics::GroundZ);
}
	
void GearDynamics::computeForceAndMoment(double x) {
	doComplexPhysics(x);
}

void GearDynamics::GearUp() { 
	setStatus(false);
}
	
void GearDynamics::GearDown() {
	setStatus(true);
}

void GearDynamics::GearToggle() {
	setStatus(!m_Extended);
}
	
bool GearDynamics::getExtended() const {
	return m_Extended;
}

bool GearDynamics::getWOW() const {
	return b_WOW->value();
}

void GearDynamics::setBraking(double left, double right) {
	size_t n = m_Gear.size();
	double x = (left + right) * 0.5; // FIXME
	for (size_t i = 0; i < n; ++i) {
		m_Gear[i]->setBraking(x);
	}
}

void GearDynamics::setSteering(double x, double link_brakes) {
	size_t n = m_Gear.size();
	for (size_t i = 0; i < n; ++i) {
		m_Gear[i]->setSteering(x, link_brakes);
	}
}

LandingGear const *GearDynamics::getGear(unsigned i) {
	assert(i <= m_Gear.size());
	return m_Gear[i].get();
}

size_t GearDynamics::getGearNumber() const {
	return m_Gear.size();
}

std::vector<Vector3> GearDynamics::getGearPosition() const {
	std::vector<Vector3> gear_pos;
	size_t n =  m_Gear.size();
	for (size_t i = 0; i < n; ++i) {
		LandingGear const *gear = m_Gear[i].get();
		Vector3 move = gear->getPosition();
		// FIXME motion should be projected along gear axis
		move.z() -= gear->getMaxPosition().z();
		gear_pos.push_back(move);
	}
	return gear_pos;
}

void GearDynamics::preSimulationStep(double dt) {
	BaseDynamics::preSimulationStep(dt);
	b_WOW->value() = false;
	if (!m_Extended) return;
	if (!b_NearGround->value()) return;
	m_WindVelocityBody = m_Attitude->invrotate(b_WindVelocity->value());
	m_GroundNormalBody = m_Attitude->invrotate(b_GroundN->value());
	setSteering(b_SteeringInput->value());
	setBraking(b_LeftBrakeInput->value(), b_RightBrakeInput->value());
	m_Height = m_PositionLocal->z() - b_GroundZ->value();
	size_t n =  m_Gear.size();
	for (size_t i = 0; i < n; ++i) {
		m_Gear[i]->preSimulationStep(dt);
	}
}

void GearDynamics::postSimulationStep(double dt) {
	BaseDynamics::postSimulationStep(dt);
	if (!m_Extended) return;
	if (!b_NearGround->value()) return;
	m_Height = m_PositionLocal->z() - b_GroundZ->value();
	size_t n =  m_Gear.size();
	for (size_t i = 0; i < n; ++i) {
		Vector3 R = m_Gear[i]->getPosition();
		Vector3 vBody = *m_VelocityBody + (*m_AngularVelocityBody ^ R);
		m_Gear[i]->postSimulationStep(dt, *m_PositionLocal, vBody, *m_Attitude, m_Height, m_GroundNormalBody);
		if (m_Gear[i]->getWOW()) b_WOW->value() = true;
	}
	b_GearPosition->value() = getGearPosition();
}



