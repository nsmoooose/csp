// Combat Simulator Project
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
 *  Rough terrain damage
 *  Generic damage modelling, gear collapse,  etc.
 *  wheel inertia
 *    - move spindown to common method
 *    - base spindown on friction coefficient (Real)
 *    - real wheel inertia?
 *	Touchdown check via push channel
 *
 */

#include <csp/cspsim/LandingGear.h>
#include <csp/cspsim/ConditionsChannels.h>
#include <csp/cspsim/ControlInputsChannels.h>
#include <csp/cspsim/GearAnimation.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/LandingGearChannels.h>
#include <csp/cspsim/SystemsModel.h>
#include <csp/cspsim/ResourceBundle.h>
#include <csp/cspsim/sound/SoundEngine.h>
#include <csp/cspsim/sound/Sample.h>
#include <csp/cspsim/sound/SoundModel.h>

#include <csp/csplib/util/Math.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/util/osg.h>

#include <osgAL/SoundState>

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace csp {

CSP_XML_BEGIN(LandingGear)
	CSP_DEF("gear_name", m_Name, true)
	CSP_DEF("max_position", m_MaxPosition, true)
	CSP_DEF("motion", m_Motion, true)
	CSP_DEF("damage_limit", m_DamageLimit, true)
	CSP_DEF("K", m_K, true)
	CSP_DEF("beta", m_Beta, true)
	CSP_DEF("chain", m_Chained, false)
	CSP_DEF("brake_limit", m_BrakeLimit, false)
	CSP_DEF("brake_slip", m_BrakeSlip, false)
	CSP_DEF("tire_static_friction", m_TireStaticFriction, false)
	CSP_DEF("tire_skid_friction", m_TireSkidFriction, false)
	CSP_DEF("tire_radius", m_TireRadius, false)
	CSP_DEF("compression_limit", m_CompressionLimit, true)
	CSP_DEF("steering_limit", m_SteeringLimit, false)
	CSP_DEF("tire_K", m_TireK, false)
	CSP_DEF("tire_beta", m_TireBeta, false)
	CSP_DEF("abs", m_ABS, false)
	CSP_DEF("rolling_friction", m_RollingFriction, false)
	CSP_DEF("drag_factor", m_DragFactor, false)
	CSP_DEF("gear_animation", m_GearAnimation, false)
CSP_XML_END

CSP_XML_BEGIN(GearDynamics)
	CSP_DEF("gear_set", m_Gear, true)
CSP_XML_END


LandingGear::LandingGear() {
	m_Name = "Unnamed gear";
	m_Chained = false;
	m_K = 0.0;
	m_Beta = 0.0;
	m_SteeringAngle = 0.0;
	m_SteeringLimit = 0.0;
	m_TargetSteerAngle = 0.0;
	m_TireK = 200000.0;
	m_TireBeta = 500.0;
	m_Damage = 0.0;
	m_Extend = false;
	m_Compression = 0.0;
	m_CompressionLimit = 0.0;
	m_Touchdown = false;
	m_Skidding = 0.0;
	m_Brake = 0.0;
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
	m_SteerTransform = Quat::IDENTITY;
	m_Skidding = false;
	m_SkidFlag = false;
	m_Touchdown = false;
	m_TireRotation = 0.0;
	m_TireRotationRate = 0.0;
	m_TireRadius = 0.25;
	m_DragFactor = 0.8;
	m_Initialize = true;
}


LandingGear::~LandingGear() {
}


void LandingGear::postCreate() {
	Object::postCreate();
	m_Motion.normalize();
	m_Position = m_MaxPosition;
}


Vector3 LandingGear::simulateSubStep(Vector3 const &origin,
                                     Vector3 const &vBody,
                                     Quat const &q,
                                     double height,
                                     Vector3 const &normalGroundBody)
{
	if (b_FullyRetracted->value()) return Vector3::ZERO;
	resetForces();

	// do suspension physics to get normal force
	updateSuspension(0.0, origin, vBody, q, height, normalGroundBody);
	
	// if not in contact with the ground, return early
	if (m_Compression <= 0.0) return Vector3::ZERO;
	
	// do wheel physics to get tangent force, but don't update the contact point
	updateWheel(0.0, origin, vBody, q, normalGroundBody, false);

	// return the total force (body coordinates)
	return m_NormalForce + m_TangentForce;
}

void LandingGear::extend() {
	if (!m_Extend && m_GearAnimation.valid()) {
		m_Extend = true;
		b_FullyExtended->value() = false;
		b_FullyRetracted->value() = false;
		m_GearAnimation->extend();
	}
}

void LandingGear::retract() {
	if (m_Extend && m_GearAnimation.valid()) {
		m_Extend = false;
		b_FullyExtended->value() = false;
		b_FullyRetracted->value() = false;
		m_GearAnimation->retract();
	}
}


double LandingGear::getDragFactor() const {
	if (b_FullyRetracted->value()) return 0.0;
	// XXX very temporary hack (need xml, partial extension, etc)
	return m_DragFactor;
}


void LandingGear::preSimulationStep(double /*dt*/) {
	if (b_FullyRetracted->value()) return;
}

/**
 * Update the brake caliper in response to brake input.
 */
void LandingGear::updateBraking(double dt) {
	// Update the actual brake power from the input brake command.  The low pass filter
	// is part of the antilock braking implementation.  In the simulation step, if brake
	// lock is detected m_Brake will be set to zero (brakes released); the brake force
	// then builds up gradually over the next couple hundred milliseconds, repeating ad
	// nauseum if the wheel continues to skid.
	if (b_BrakeCommand.valid()) {
		double f = std::min(dt * 10.0, 1.0);  // ad-hoc time constant
		double brake_command = clampTo(b_BrakeCommand->value(), 0.0, 1.0);
		m_Brake = m_Brake * (1.0 - f) + brake_command * f;
	}
}

/**
 * Update Weight-On-Wheels flag and record touchdown point
 */
void LandingGear::updateWOW(Vector3 const &origin, Quat const &q, Vector3 const &vBody, Vector3 const &normalGroundBody) {
	if (m_Compression > 0.02) {
		// first contact?
		if (!b_WOW->value() && !m_Touchdown) {
			// yes, flag the touchdown
			m_Touchdown = true;
			m_TouchdownVerticalVelocity = std::max(0.0, -dot(vBody, normalGroundBody));
			Vector3 vGroundBody = vBody - m_TouchdownVerticalVelocity * normalGroundBody;
			m_TouchdownSkid = fabs(vGroundBody.length() - getWheelSpeed()) > 10.0 * (1.0 - m_TouchdownVerticalVelocity);
			m_TouchdownPoint = origin + q.rotate(m_Position);
			b_WOW->value() = true;
			// play touchdown sound
			if (m_TouchdownSkid) {
				if (getTouchdownSound().valid()) getTouchdownSound()->play();
			}
		}
	} else {
		b_WOW->value() = false;
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
		double fudge = 50.0 + 0.03 * m_Brake * m_BrakeLimit;
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

/**
 * Simulate the main shock adsorber to determine the normal force and
 * gear compression.
 *
 * @param origin model origin in local coordinates
 * @param vBody strut velocity in body coordinates
 * @param q body orientation
 * @param height height of body origin above ground
 * @param normalGroundBody ground normal vector in body coordinates
 */
void LandingGear::updateSuspension(const double dt,
                                   Vector3 const &/*origin*/,
                                   Vector3 const &vBody,
                                   Quat const &/*q*/,
                                   double const height,
                                   Vector3 const &normalGroundBody)
{
	double compression = 0.0;
	double motionNormal = dot(m_Motion, normalGroundBody);
	Vector3 max_position = getMaxPosition();
	if (motionNormal > 0.0) {
		compression = - (dot(max_position, normalGroundBody) + height) / motionNormal;
		compression = std::max(compression, 0.0);
	}

	// at max extension and not in contact?
	if (compression == 0.0 && m_Compression == 0.0) {
		b_WOW->value() = false;
		m_Position = max_position;
	} else {
		// FIXME in computing vCompression, only the normal force is taken into
		// account, but other components can matter if m_Motion isn't vertical.
		// (e.g. when brakes are applied)
	
		// calculate strut compression speed
		double vCompression = - dot(vBody, normalGroundBody) * motionNormal;
		// restrict compression speed to reasonable limits (faster than this means
		// the gear will probably break in a moment anyway)
		vCompression = clampTo(vCompression, -10.0, 10.0);

		const double old_compression = m_Compression;

		// are we overcompressed?
		if (compression >= m_CompressionLimit) {
			m_Compression = m_CompressionLimit;
			// stiffen the response, but only if we are not already decompressing
			// too quickly (prevents launching the aircraft up when the gear gets
			// overcompressed --- although eventually the gear should just break
			// in this case).
			double extra = clampTo(20.0 * (vCompression + 0.5), 0.0, 10.0);
			compression += std::min(0.1, compression - m_CompressionLimit) * extra;
			// TODO: break the gear if overcompression is too high (should
			// actually be keyed by normalForce below).
		} else {
			m_Compression = compression;
		}

		// ground support (in response to strut compression + damping)
		double normalForce = (m_K * compression + m_Beta * vCompression) * motionNormal;

		if (normalForce > 0.0) {
			m_NormalForce += normalForce * normalGroundBody;
		} else {
			double v_extend = std::max(0.5, (m_K * m_Compression) / m_Beta);
			m_Compression = std::max(old_compression - v_extend * dt, 0.0);
		}

		// update wheel position
		m_Position = max_position + m_Motion * m_Compression;
	}
}

void LandingGear::resetForces() {
	m_NormalForce = Vector3::ZERO;
	m_TangentForce = Vector3::ZERO;
}

void LandingGear::updateSteeringAngle(double /*dt*/) {
	if (m_SteeringLimit > 0 && b_SteeringCommand.valid()) {
		double setting = clampTo(b_SteeringCommand->value(), -1.0, 1.0);

		// add a deadzone and more gentle response for small commands.
		setting = setting * std::max(setting * setting - 0.01, 0.0);

		// ad-hoc velocity limiter
		double v = m_TireRotationRate * m_TireRadius;
		setting *= clampTo(1.1 - 0.1 * v, 0.1, 1.0);

		m_SteeringAngle = setting * m_SteeringLimit;
		m_SteerTransform.makeRotate(toRadians(m_SteeringAngle), Vector3::ZAXIS);
	}
}

void LandingGear::postSimulationStep(double dt,
                                     Vector3 const &origin,
                                     Vector3 const &vBody,
                                     Quat const &q,
                                     double const height,
                                     Vector3 const &normalGroundBody) {
	if (b_FullyRetracted->value()) return;
	resetForces();
	// update order matters
	updateBraking(dt);
	updateSuspension(dt, origin, vBody, q, height, normalGroundBody);
	updateWOW(origin, q, vBody, normalGroundBody);
	updateWheel(dt, origin, vBody, q, normalGroundBody, true);
	updateTireRotation(dt);
	updateSteeringAngle(dt);
}

void LandingGear::residualUpdate(double dt, double airspeed) {
	// ensure that WOW is cleared; heavy shock damping and a fast takeoff
	// could conceivably switch to residual updates before the compression
	// was fully relaxed.
	if (m_Compression > 0.0) {
		m_Compression = 0.0;
		b_WOW->value() = false;
	}
	updateBrakeTemperature(dt, 0.0, airspeed);
	updateTireRotation(dt);
	updateSteeringAngle(dt);
}


void LandingGear::updateAnimation(double dt) {
	if (m_GearAnimation.valid()) {
		m_GearAnimation->update(dt);
		if (m_Compression > 0.0) m_GearAnimation->setCompression(m_Compression / m_CompressionLimit);
		m_GearAnimation->setTireRotation(fmod(m_TireRotation, 2.0*PI) - PI);
		if (m_SteeringLimit > 0.0) {
			m_GearAnimation->setSteeringAngle(toRadians(m_SteeringAngle));
		}
		if (m_Extend && !b_FullyExtended->value()) {
			b_FullyExtended->value() = m_GearAnimation->isFullyExtended();
		} else if (!m_Extend && !b_FullyRetracted->value()) {
			b_FullyRetracted->value() = m_GearAnimation->isFullyRetracted();
		}
		if (m_Initialize) {
			m_Initialize = false;
			m_GearAnimation->force(m_Extend);
			b_FullyExtended->value() = m_Extend;
			b_FullyRetracted->value() = !m_Extend;
		}
	}
}

double LandingGear::getExtension() const {
	return !m_GearAnimation ? 1.0 : m_GearAnimation->getExtension();
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
}

/**
 * Update the ground-tire contact point, detect skidding, and set friction coefficients.
 */
void LandingGear::updateWheel(double dt,
                              Vector3 const &origin,
                              Vector3 const &vBody,
                              Quat const &q,
                              Vector3 const &normalGroundBody,
                              bool updateContact)
{
	static double XXX_t = 0.0;
	if (updateContact) XXX_t += dt;

	Vector3 tirePositionLocal = origin + q.rotate(m_Position);

	if (updateContact && m_ABSActiveTimer > 0.0) {
		m_ABSActiveTimer -= dt;
	}

	b_AntilockBrakingActive->value() = m_ABSActiveTimer > 0.0;

	// FIXME tire contact point not initialized until first call to
	// postSimulationStep.
	if (!updateContact && m_TireContactPoint.isZero()) return;

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

	// TODO rethink how tire contact updates interact with rk substeps.
	double deformation = tireDeformation.normalize();
	tireDeformation *= std::min(deformation, 0.5);

	Vector3 tireForce = - tireDeformation * m_TireK;

	// switch to body coordinates
	Vector3 tireForceBody = q.invrotate(tireForce);
	// project onto the ground
	tireForceBody -= dot(tireForceBody, normalGroundBody) * normalGroundBody;
	// transform to wheel coordinates
	Vector3 tireForceWheel = m_SteerTransform.rotate(tireForceBody);

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
		//tireForceWheel.x() *= (0.1 + 0.9 * fabs(m_SteeringAngle/m_SteeringLimit));
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
				// turn on abs indicator flag for 0.5 sec
				m_ABSActiveTimer = 0.5;
			}
		}
		// reduce total force (reevaluate skidLimit with skidding friction)
		tireForceWheel *= m_TireFriction * normalForce / totalTireForce;
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
		if (updateContact) {
			m_Skidding = false;
			// tire isn't skidding, so use static friction from now on
			m_TireFriction = m_TireStaticFriction;
		}
	}

	// convert back to body coordinates
	tireForceBody = m_SteerTransform.invrotate(tireForceWheel);

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

	m_TangentForce += tireForceBody;
}

void LandingGear::registerChannels(Bus* bus) {
	CSPLOG(Prio_INFO, Cat_OBJECT) << "Registering " << getName() << " channels";
	b_WOW = bus->registerLocalDataChannel<bool>(bus::LandingGear::selectWOW(getName()), false);
	b_FullyRetracted = bus->registerLocalDataChannel<bool>(bus::LandingGear::selectFullyRetracted(getName()), false);
	b_FullyExtended = bus->registerLocalDataChannel<bool>(bus::LandingGear::selectFullyExtended(getName()), false);
	b_AntilockBrakingActive = bus->registerLocalDataChannel<bool>(bus::LandingGear::selectAntilockBrakingActive(getName()), false);
	if (m_GearAnimation.valid()) {
		m_GearAnimation->setGearName(getName());
		m_GearAnimation->setCompressionMotion(m_Motion, m_CompressionLimit);
		m_GearAnimation->registerChannels(bus);
	} else {
		CSPLOG(Prio_DEBUG, Cat_OBJECT) << "GearAnimation for " << getName() << " not valid";
	}
}

void LandingGear::bindChannels(Bus* bus) {
	if (m_GearAnimation.valid()) {
		m_GearAnimation->bindChannels(bus);
	}
	b_BrakeCommand = bus->getChannel(bus::LandingGear::selectBrakeCommand(getName()), false);
	b_SteeringCommand = bus->getChannel(bus::LandingGear::selectSteeringCommand(getName()), false);
}

void LandingGear::bindSounds(SoundModel *model, ResourceBundle *bundle) {
	if ( !SoundEngine::getInstance().getSoundEnabled() ) return;
	assert(model);
	CSPLOG(Prio_DEBUG, Cat_AUDIO) << "GearDynamics::bindSounds";
	if (bundle) {
		CSPLOG(Prio_DEBUG, Cat_AUDIO) << "GearDynamics::bindSounds have bundle";
		Ref<const SoundSample> sample(bundle->getSoundSample("wheel_touchdown"));
		m_TouchdownSound = SoundEffect::ExternalSound(sample, model);
		if (m_TouchdownSound.valid()) {
			CSPLOG(Prio_DEBUG, Cat_AUDIO) << "GearDynamics::bindSounds have sound";
			m_TouchdownSound->state()->setPosition(toOSG(getPosition()));
			m_TouchdownSound->state()->setDirection(toOSG(getPosition()));
			CSPLOG(Prio_DEBUG, Cat_AUDIO) << "gear touchdown sound position " << getPosition();
			CSPLOG(Prio_DEBUG, Cat_AUDIO) << "gear touchdown sound direction " << getPosition();
			m_TouchdownSound->state()->apply();
		}
	}
	CSPLOG(Prio_DEBUG, Cat_AUDIO) << "GearDynamics::bindSounds exit";
}

DEFINE_INPUT_INTERFACE(GearDynamics)

void GearDynamics::doComplexPhysics(double) {
	if (b_FullyRetracted->value() && !isGearExtendSelected()) return;

	// testing updating these values during the step to see if holding
	// them constant was responsible for discontinuities in the compression
	// state between time steps.
	const Vector3 model_origin_local = getModelPosition();
	m_WindVelocityBody = toBody(b_WindVelocity->value());
	m_GroundNormalBody = toBody(b_GroundN->value());
	m_Height = model_origin_local.z() - b_GroundZ->value();

	m_Force = m_Moment = Vector3::ZERO;
	Vector3 airflow_body = m_WindVelocityBody - *m_VelocityBody;
	const double airspeed = airflow_body.length();
	Vector3 dynamic_pressure = 0.5 * (b_Density->value()) * airflow_body * airspeed;
	b_FullyRetracted->value() = true;
	const size_t n = m_Gear.size();

	for (size_t i = 0; i < n; ++i) {
		LandingGear &gear = *(m_Gear[i]);
		if (gear.isFullyRetracted()) continue;
		b_FullyRetracted->value() = false;
		const double extension = gear.getExtension();
		assert(extension >= -0.001 && extension <= 1.001);
		// Approx: gear move to center with retraction.  currently LandingGear does not take
		// extension into account when computing forces, so really only effects wind drag.
		// at least it _shouldn't_ be common to retract gear while touching the ground...
		Vector3 R = extension * (gear.getPosition() - b_CenterOfMassOffset->value());  // body (cm) coordinates
		assert(R.valid());
		Vector3 F = Vector3::ZERO;
		if (b_NearGround->value()) {
			Vector3 vBody = *m_VelocityBody + (*m_AngularVelocityBody ^ R);
			F += gear.simulateSubStep(model_origin_local, vBody, *m_Attitude, m_Height, m_GroundNormalBody);
		}
		F += extension * gear.getDragFactor() * dynamic_pressure;
		m_Force += F;
		m_Moment += (R ^ F);
	}
}

GearDynamics::GearDynamics(): m_Height(0.0) {
}

void GearDynamics::registerChannels(Bus *bus) {
	assert(bus!=0);
	SoundModel *sound_model = getModel()->getSoundModel();
	b_WOW = bus->registerLocalDataChannel<bool>(bus::LandingGear::WOW, false);
	b_FullyRetracted = bus->registerLocalDataChannel<bool>(bus::LandingGear::FullyRetracted, false);
	b_FullyExtended = bus->registerLocalDataChannel<bool>(bus::LandingGear::FullyExtended, true);
	b_GearExtendSelected = bus->registerLocalDataChannel<bool>(bus::LandingGear::GearExtendSelected, true);
	b_GearCommand = bus->registerSharedPushChannel<bool>(bus::LandingGear::GearCommand, true);
	b_GearCommand->connect(this, &GearDynamics::onGearCommand);
	for (unsigned i = 0; i < m_Gear.size(); ++i) {
		m_Gear[i]->registerChannels(bus);
		if (sound_model) {
			m_Gear[i]->bindSounds(sound_model, getResourceBundle());
		}
	}
}

void GearDynamics::importChannels(Bus *bus) {
	assert(bus!=0);
	b_Density = bus->getChannel(bus::Conditions::Density);
	b_WindVelocity = bus->getChannel(bus::Conditions::WindVelocity);
	b_NearGround = bus->getChannel(bus::Kinetics::NearGround);
	b_GroundN = bus->getChannel(bus::Kinetics::GroundN);
	b_GroundZ = bus->getChannel(bus::Kinetics::GroundZ);
	b_CenterOfMassOffset = bus->getChannel(bus::Kinetics::CenterOfMassOffset);
	for (unsigned i = 0; i < m_Gear.size(); ++i) {
		m_Gear[i]->bindChannels(bus);
	}
}

void GearDynamics::computeForceAndMoment(double x) {
	doComplexPhysics(x);
}

bool GearDynamics::isGearExtendSelected() const {
	return b_GearExtendSelected->value();
}

bool GearDynamics::getWOW() const {
	return b_WOW->value();
}

void GearDynamics::preSimulationStep(double dt) {
	BaseDynamics::preSimulationStep(dt);

	b_WOW->value() = false;

	if (b_FullyRetracted->value()) return;
	if (!b_NearGround->value()) return;

	m_WindVelocityBody = toBody(b_WindVelocity->value());
	m_GroundNormalBody = toBody(b_GroundN->value());
	const Vector3 model_origin_local = getModelPosition();
	m_Height = model_origin_local.z() - b_GroundZ->value();
	size_t n =  m_Gear.size();
	for (size_t i = 0; i < n; ++i) {
		m_Gear[i]->preSimulationStep(dt);
	}
}

void GearDynamics::postSimulationStep(double dt) {
	BaseDynamics::postSimulationStep(dt);
	size_t n =  m_Gear.size();
	if (b_FullyRetracted->value() || !b_NearGround->value()) {
		for (size_t i = 0; i < n; ++i) {
			const bool fully_retracted = m_Gear[i]->isFullyRetracted();
			double airspeed = fully_retracted ? 0.0 : m_VelocityBody->length();  // approx
			m_Gear[i]->residualUpdate(dt, airspeed);
		}
		return;
	}
	const Vector3 model_origin_local = getModelPosition();
	m_Height = model_origin_local.z() - b_GroundZ->value();
	for (size_t i = 0; i < n; ++i) {
		Vector3 R = m_Gear[i]->getPosition() - b_CenterOfMassOffset->value();  // body (cm) coordinates
		Vector3 vBody = *m_VelocityBody + (*m_AngularVelocityBody ^ R);
		m_Gear[i]->postSimulationStep(dt, model_origin_local, vBody, *m_Attitude, m_Height, m_GroundNormalBody);
		// generic WOW signal (any gear in contact with the ground triggers it)
		if (m_Gear[i]->getWOW()) b_WOW->value() = true;
		// TODO: The touchdown flag should be implemented as a push channel
		if (m_Gear[i]->getTouchdown()) {
			m_Gear[i]->resetTouchdown();
		}
	}
}

double GearDynamics::onUpdate(double dt) {
	size_t n =  m_Gear.size();
	b_FullyExtended->value() = true;
	b_FullyRetracted->value() = true;
	for (size_t i = 0; i < n; ++i) {
		m_Gear[i]->updateAnimation(dt);
		const bool fully_extended = m_Gear[i]->isFullyExtended();
		const bool fully_retracted = m_Gear[i]->isFullyRetracted();
		if (!fully_extended) b_FullyExtended->value() = false;
		if (!fully_retracted) b_FullyRetracted->value() = false;
	}
	return 0.016;
}

void GearDynamics::getInfo(InfoList &info) const {
	std::stringstream line;
	line.setf(std::ios::fixed | std::ios::showpos);
	line.precision(0);
	Ref<LandingGear> main = m_Gear[1];
	line << "Gear: ";
	line << (b_FullyRetracted->value() ? "UP" : (b_FullyExtended->value() ? "DOWN" : "TRANS"));
	line << " T_brakes " << std::setw(3) << main->getBrakeTemperature() << "K";
	if (getWOW()) line << " WOW";
	if (main->getSkidding()) line << " SKID";
	if (main->getABSActive()) line << " ABS";
	info.push_back(line.str());
}


bool GearDynamics::allowGearUp() const {
	return !getWOW();
}

void GearDynamics::GearUp() {
	if (allowGearUp()) {
		for (unsigned i = 0; i < m_Gear.size(); ++i) m_Gear[i]->retract();
		b_GearExtendSelected->value() = false;
	}
}

void GearDynamics::GearDown() {
	for (unsigned i = 0; i < m_Gear.size(); ++i) m_Gear[i]->extend();
	b_GearExtendSelected->value() = true;
}

void GearDynamics::GearToggle() {
	if (isGearExtendSelected()) {
		GearUp();
	} else {
		GearDown();
	}
}

void GearDynamics::onGearCommand() {
	if (b_GearCommand->value()) {
		GearUp();
	} else {
		GearDown();
	}
}

} // namespace csp

