// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002, 2004 The Combat Simulator Project
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
 *  wheel inertia
 *    - move spindown to common method
 *    - base spindown on friction coefficient (simdata::Real)
 *    - figure out why wheels slip without braking
 *    - real wheel inertia?
 *
 */

#include <LandingGear.h>
#include <KineticsChannels.h>

#include <SimData/Math.h>

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

using bus::Kinetics;

using simdata::toRadians;
using simdata::dot;
using simdata::Vector3;


SIMDATA_REGISTER_INTERFACE(LandingGear)
SIMDATA_REGISTER_INTERFACE(GearDynamics)
SIMDATA_REGISTER_INTERFACE(GearAnimation)
SIMDATA_REGISTER_INTERFACE(GearStructureAnimation)
SIMDATA_REGISTER_INTERFACE(M2kGearStructureAnimation)


LandingGear::LandingGear() {
	m_Name = "Nonamed gear";
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
	m_RollingFriction.set(0.02f, 0.002f);
	m_TireFriction = 0.8;
	m_TireSkidFriction = 0.6;
	m_TireStaticFriction = 1.0;
	m_ABS = true;
	m_ABSActiveTimer = 0.0;
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
	m_DragFactor = 0.8;
}


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
	setting = simdata::clampTo(setting,-1.0,1.0);
	m_BrakeSetting = setting;
}


double LandingGear::setSteering(double setting, double link_brakes) {
	setting = simdata::clampTo(setting,-1.0,1.0);
	m_BrakeSteer = setting * link_brakes * m_BrakeSteeringLinkage;
	m_SteerAngle = setting * m_SteeringLimit;
	double rad = toRadians(m_SteerAngle);
	m_SteerTransform.makeRotate(rad, Vector3::ZAXIS);
	return m_SteerAngle;
}


double LandingGear::getDragFactor() const {
	// XXX very temporary hack (need xml, partial extension, etc)
	if (m_Extended) return m_DragFactor;
	return 0.0;
}


void LandingGear::preSimulationStep(double /*dt*/) {
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
	double newBrake = simdata::clampTo(m_BrakeSetting + m_BrakeSteer,0.0,1.0);
	m_Brake = m_Brake * (1.0-f) + newBrake * f;
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


void LandingGear::updateTireRotation(double dt) {
	// spindown due to friction when not in contact
	if (m_TireRotationRate == 0.0) return;
	if (m_Compression <= 0.0) {
		// rolling friction is an effective friction coefficient when in
		// contact with the ground, which include tire deformation, so it
		// isn't really appropriate here.  nevertheless, it serves as a
		// convenient scaling factor.
		// XXX spindown is currently too long without setting an offset,
		// and the spindown rate with braking is completely arbitrary.
		double fudge = 50.0 + 0.03 * m_BrakeSetting * m_BrakeLimit;
		double torque = fudge * m_RollingFriction /* x m_TireRadius x wheel_mass */;
		double inertia = 0.5 * m_TireRadius /* x m_TireRadius x wheel_mass */;
		double dw = dt * torque / inertia;
		if (m_TireRotationRate > 0.0) {
			m_TireRotationRate = std::max(0.0, m_TireRotationRate - dw);
		} else {
			m_TireRotationRate = std::min(0.0, m_TireRotationRate + dw);
		}
	}
	m_TireRotation += m_TireRotationRate * dt;
}

simdata::Vector3 LandingGear::getDisplacement() const {
	Vector3 move = getPosition();
	move -= getMaxPosition();
	return move;
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
void LandingGear::updateSuspension(Vector3 const &/*origin*/,
                                   Vector3 const &vBody,
                                   simdata::Quat const &/*q*/,
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
		vCompression = simdata::clampTo(vCompression, -10.0, 10.0);
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
	updateTireRotation(dt);
}

void LandingGear::residualUpdate(double dt, double airspeed) {
	updateBrakeTemperature(dt, 0.0, airspeed);
	updateTireRotation(dt);
}

void LandingGear::updateBrakeTemperature(double dt, double dissipation, double airspeed) {
	double T2 = m_BrakeTemperature * m_BrakeTemperature;
	double R2 = m_TireRadius * m_TireRadius;
	// approximate brake disc surface area (2 sides)
	double surfaceArea = R2 * 4.0;
	// compute black body radiation (approximating emmisivity and surface area)
	double radiation = 5.7e-8 * (T2*T2-8.1e+9) * surfaceArea;
	// total guess
	double aircooling = 30.0 * (m_BrakeTemperature - 300.0) * surfaceArea * (airspeed + 1.0);
	// arbitrary universal scaling (approximate volume as R3, specific heat
	// near that of steel)
	double heatCapacity = R2 * m_TireRadius * 2e+6;
	m_BrakeTemperature += (dissipation - radiation - aircooling) / heatCapacity * dt;
	/*
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
	*/
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

	if (updateContact && m_ABSActiveTimer > 0.0) {
		m_ABSActiveTimer -= dt;
	}

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
	bool brakeSlip = alignedForce >= brakeLimit;
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

	// maximum traction
	double skidLimit = m_TireFriction * normalForce;

	// ground velocity in body coordinates
	Vector3 vGroundBody = vBody - dot(vBody, normalGroundBody) * normalGroundBody;
	// transform to wheel coordinates
	Vector3 vGroundWheel = m_SteerTransform.rotate(vGroundBody);
	// normalize to get rolling direction
	Vector3 rollingDirectionWheel = Vector3(0.0, vGroundWheel.y(), vGroundWheel.z()).normalized();
	// compute rolling friction
	Vector3 rollingFrictionWheel = - m_RollingFriction * normalForce * rollingDirectionWheel;

	// is this wheel steerable?
	if (m_SteeringLimit > 0.0) {
		// partial (approx) castering of steerable wheels to reduce instability
		// 2004-08-02 DISABLED(OS): this no longer seems necessary; the original instability
		// may have been due to a bug that has since been fixed.  anyhow, it's not good to
		// caster at low speeds (and castering should be reflected in the gear steering angle).
		//tireForceWheel.x() *= (0.1 + 0.9 * fabs(m_SteerAngle/m_SteeringLimit));
	}

	// total (virtual) tire force due to braking and sideslip
	double totalTireForce = tireForceWheel.length();

	// FIXME wheels currently have zero inertia.  the current implementation
	// isn't well suited to modelling wheel spin, and only very crude tracking
	// of wheel rotation is done.  the code should be redesigned at some point
	// to model braking and ground contact forces as torques acting on wheel
	// rotation, and thereby accounting for wheel inertia, etc.  otoh, these
	// affects are generally pretty small and may not be worth modelling
	// accurately.

	// check if tire is skidding
	bool skidding = totalTireForce >= skidLimit;
	if (skidding) {
		if (updateContact) {
			m_SkidFlag = true;
			m_Skidding = true;
			// use skidding friction until the skidding stops
			m_TireFriction = m_TireSkidFriction;
			// momentarily release brakes to stop the skid
			if (m_ABS) {
				m_Brake = 0.0;
				// turn on abs indicator flag for 0.5 sec
				m_ABSActiveTimer = 0.5;
			}
		}
		// reduce total force (reevaluate skidLimit with skidding friction)
		tireForceWheel *= m_TireFriction * normalForce / totalTireForce;
		// convert back to body coordinates
		tireForceBody = m_SteerTransform.invrotate(tireForceWheel);
	} else {
		// damp the oscillation of the tire rubber
		Vector3 tireDampingWheel = - m_TireBeta * vGroundWheel;
		if (brakeSlip) {
			// tire is rolling, so only damp sideways motion and add rolling friction
			tireDampingWheel.y() = tireDampingWheel.z() = 0.0;
			tireForceWheel += rollingFrictionWheel;
		}
		// in principle, the damping term should be included in the skid test.  for now
		// we just limit the damping force to prevent overflows if vGroundWheel is large.
		double tireDampingForce = tireDampingWheel.length();
		if (tireDampingForce > skidLimit) {
			tireDampingWheel *= (skidLimit / tireDampingForce);
		}
		tireForceWheel += tireDampingWheel;
		// convert back to body coordinates
		tireForceBody = m_SteerTransform.invrotate(tireForceWheel);
		if (updateContact) {
			m_Skidding = false;
			// tire isn't skidding, so use static friction from now on
			m_TireFriction = m_TireStaticFriction;
		}
	}

	if (updateContact) {
		double rollingSpeed = sqrt(vGroundWheel.y()*vGroundWheel.y() + vGroundWheel.z()*vGroundWheel.z());
		if (vGroundWheel.y() < 0.0) rollingSpeed = -rollingSpeed;
		if (brakeSlip) {
			m_TireRotationRate = rollingSpeed / m_TireRadius;
		} else {
			m_TireRotationRate = 0.0;
			//std::cout << "WHEEL LOCK\n";
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

	if (0 && updateContact) {
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
	if (updateContact) {
		double dissipation = 0.0;
		if (m_BrakeLimit > 0.0) {
			// XXX if skidding, the dissipation will be nearly zero since the wheel
			// is locked.  we should model tire damage under this condition, so that
			// blowouts can result from extended skids.
			dissipation = fabs(alignedForce * m_TireRotationRate * m_TireRadius);
		}
		double airspeed = vBody.length();
		updateBrakeTemperature(dt, dissipation, airspeed);
	}

	/*
	if (0 && XXX_t > 90.0 && (tireForceBody-XXX_tfb).length() > 0.001) {
		std::cout << "DIFFERENCE: " << (tireForceBody-XXX_tfb).asString() << " " << skidding << " " << brakeSlip << ", " << tireForceBody << " " << brakeLimit << " " << m_Brake << " " << m_BrakeFriction << m_BrakeLimit << "\n";
	}
	if (tireForceBody.length() > 50000.0) {
		std::cout << "TFB ---> " << tireForceBody.asString() << "\n";
		std::cout << "DEF ---> " << tireDeformation.asString() << "\n";
		std::cout << "TFB ---> " << XXX_tfb.asString() << "\n";
		std::cout << "VGW ---> " << vGroundWheel.asString() << "\n";
	}
	*/
	m_TangentForce += tireForceBody;
	//m_TangentForce += XXX_tfb;
}

DEFINE_INPUT_INTERFACE(GearAnimation);

void GearDynamics::doComplexPhysics(double) {
	m_Force = m_Moment = Vector3::ZERO;
	if (!isGearExtended()) return;
	Vector3 airflow_body = m_WindVelocityBody - *m_VelocityBody;
	double airspeed = airflow_body.length();
	Vector3 dynamic_pressure = 0.5 * (b_Density->value()) * airflow_body * airspeed;
	size_t n = m_Gear.size();
	for (size_t i = 0; i < n; ++i) {
		LandingGear &gear = *(m_Gear[i]);
		double extension = 1.0;
		if (b_GearExtension.valid())
			extension = b_GearExtension->value();
		Vector3 R = extension * gear.getPosition();
		Vector3 F = Vector3::ZERO;
		if (b_NearGround->value()) {
			Vector3 vBody = *m_VelocityBody + (*m_AngularVelocityBody ^ R);
			F += gear.simulateSubStep(*m_PositionLocal,
			                          vBody,
			                          *m_Attitude,
			                          m_Height,
			                          m_GroundNormalBody);
		}
		F += extension * gear.getDragFactor() * dynamic_pressure;
		m_Force += F;
		m_Moment += (R ^ F);
	}
}

GearDynamics::GearDynamics():
	m_Height(0.0) {
}

void GearDynamics::registerChannels(Bus *bus) {
	assert(bus!=0);
	b_WOW = bus->registerLocalDataChannel<bool>("State.WOW", false);

	size_t n(m_Gear.size());
	b_GearDisplacement.resize(n);
	b_TireRotation.resize(n);
	for(size_t i = 0; i<n; ++i) {
		std::string gear_name = "LandingGear." + m_Gear[i]->getName();
		CSP_LOG(OBJECT, INFO, "GearDynamics: gear name = " << m_Gear[i]->getName());
		b_GearDisplacement[i] = bus->registerLocalDataChannel<Vector3>(gear_name + "Displacement",Vector3::ZERO);
		b_TireRotation[i] = bus->registerLocalDataChannel<double>(gear_name + "TireRotation",0.0);
		//b_GearDisplacement.push_back(bus->registerLocalDataChannel<Vector3>(gear_name + "Displacement",Vector3::ZERO));
		//b_TireRotation.push_back(bus->registerLocalDataChannel<double>(gear_name+"TireRotation",0.0));
	}
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
	b_GearExtension = bus->getChannel("Aircraft.GearSequence.NormalizedTime"); 
}
	
void GearDynamics::computeForceAndMoment(double x) {
	doComplexPhysics(x);
}

bool GearDynamics::isGearExtended() const {
	// TODO: different extension for each gear
	if (b_GearExtension.valid())
		return b_GearExtension->value() > 0.1;
	else 
		return true;
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

void GearDynamics::preSimulationStep(double dt) {
	BaseDynamics::preSimulationStep(dt);
	b_WOW->value() = false;
	setSteering(b_SteeringInput->value());
	setBraking(b_LeftBrakeInput->value(), b_RightBrakeInput->value());
	if (!isGearExtended()) return;
	if (!b_NearGround->value()) return;
	m_WindVelocityBody = m_Attitude->invrotate(b_WindVelocity->value());
	m_GroundNormalBody = m_Attitude->invrotate(b_GroundN->value());
	m_Height = m_PositionLocal->z() - b_GroundZ->value();
	size_t n =  m_Gear.size();
	for (size_t i = 0; i < n; ++i) {
		m_Gear[i]->preSimulationStep(dt);
	}
}

void GearDynamics::postSimulationStep(double dt) {
	BaseDynamics::postSimulationStep(dt);
	size_t n =  m_Gear.size();
	if (!isGearExtended() || !b_NearGround->value()) {
		double airspeed = b_GearExtension->value() ? m_VelocityBody->length() : 0.0;  // approx	
		for (size_t i = 0; i < n; ++i) { m_Gear[i]->residualUpdate(dt, airspeed); }
		return;
	}
	m_Height = m_PositionLocal->z() - b_GroundZ->value();
	for (size_t i = 0; i < n; ++i) {
		Vector3 R = m_Gear[i]->getPosition();
		Vector3 vBody = *m_VelocityBody + (*m_AngularVelocityBody ^ R);
		m_Gear[i]->postSimulationStep(dt, *m_PositionLocal, vBody, *m_Attitude, m_Height, m_GroundNormalBody);
		if (m_Gear[i]->getWOW()) b_WOW->value() = true;
	}
}

double GearDynamics::onUpdate(double dt) {
	size_t n =  m_Gear.size();
	for (size_t i = 0; i < n; ++i) {
		const LandingGear &gear = *m_Gear[i];
		b_GearDisplacement[i]->value() = gear.getDisplacement();
		b_TireRotation[i]->value() = gear.getTireRotation();
	}
	return 0.016;
}

void GearDynamics::getInfo(InfoList &info) const {
	std::stringstream line;
	line.setf(std::ios::fixed | std::ios::showpos);
	line.precision(0);
	simdata::Ref<LandingGear> main = m_Gear[1];
	line << "Gear: ";
	if (!isGearExtended()) line << "UP"; else line << "DOWN";
	line << " T_brakes " << std::setw(3) << main->getBrakeTemperature() << "K";
	if (getWOW()) line << " WOW";
	if (main->getSkidding()) line << " SKID";
	if (main->getABSActive()) line << " ABS";
	info.push_back(line.str());
}

