// Combat Simulator Project
// Copyright (C) 2006 The Combat Simulator Project
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

#include <csp/cspsim/ai/AircraftControl.h>
#include <csp/cspsim/ConditionsChannels.h>
#include <csp/cspsim/ControlInputsChannels.h>
#include <csp/cspsim/FlightDynamicsChannels.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/LandingGearChannels.h>
#include <iostream> // XXX

CSP_NAMESPACE

AircraftControl::AircraftControl():
		m_PitchPID(-5.0, -0.5, -1.0),
		m_RollPID(0.5, 0.05),
		m_ThrottlePID(-6.0, -0.4, -1.0),
		m_RudderPID(-1.0, -0.1),
		m_Invert(false),
		m_FinalApproach(false),
		m_HeadingError(0.0),
		m_DecelerationLimit(4.0),
		m_DynamicRollLimit(1.0),
		m_EngageDynamicRollLimit(false),
		m_TargetG(1.0),
		m_Capture(false),
		m_Done(false) {
	m_PitchPID.clamp(-1.0, 1.0);
	m_RollPID.clamp(-1.0, 1.0);
	m_ThrottlePID.clamp(0.0, 0.9);
	m_RudderPID.clamp(-1.0, 1.0);
}

void AircraftControl::importChannels(Bus *bus) {
	b_PitchInput = bus->getSharedChannel(bus::ControlInputs::PitchInput, true, true);
	b_RollInput = bus->getSharedChannel(bus::ControlInputs::RollInput, true, true);
	b_RudderInput = bus->getSharedChannel(bus::ControlInputs::RudderInput, true, true);
	b_LeftBrakeInput = bus->getSharedChannel(bus::ControlInputs::LeftBrakeInput, true, true);
	b_RightBrakeInput = bus->getSharedChannel(bus::ControlInputs::RightBrakeInput, true, true);
	b_ThrottleInput = bus->getSharedChannel(bus::ControlInputs::ThrottleInput, true, true);
	b_AirbrakeInput = bus->getSharedChannel(bus::ControlInputs::AirbrakeInput, true, true);
	b_GearCommand = bus->getSharedChannel(bus::LandingGear::GearCommand);

	b_CAS = bus->getChannel(bus::Conditions::CAS);
	b_PressureAltitude = bus->getChannel(bus::FlightDynamics::PressureAltitude);
	b_VerticalVelocity = bus->getChannel(bus::FlightDynamics::VerticalVelocity);
	b_Alpha = bus->getChannel(bus::FlightDynamics::Alpha);
	b_Airspeed = bus->getChannel(bus::FlightDynamics::Airspeed);
	b_G = bus->getChannel(bus::FlightDynamics::G);
	b_GroundZ = bus->getChannel(bus::Kinetics::GroundZ);
	b_ModelPosition = bus->getChannel(bus::Kinetics::ModelPosition);
	b_Attitude = bus->getChannel(bus::Kinetics::Attitude);
	b_Pitch = bus->getChannel(bus::Kinetics::Pitch);
	b_Roll = bus->getChannel(bus::Kinetics::Roll);
	b_Heading = bus->getChannel(bus::Kinetics::Heading);
	b_LinearVelocity = bus->getChannel(bus::Kinetics::Velocity);
	b_AccelerationBody = bus->getChannel(bus::Kinetics::AccelerationBody);
	b_WOW = bus->getChannel(bus::LandingGear::WOW);
	b_GearFullyExtended = bus->getChannel(bus::LandingGear::FullyExtended);
	b_GearFullyRetracted = bus->getChannel(bus::LandingGear::FullyRetracted);

	// TODO create a generic channel for this signal; other aircraft will
	// implement it differently.  for now we fallback on a very simple and
	// conservative ground avoidance calculation if this channel isn't
	// available (see pullupAdvisory).
	b_AltitudeAdvisory = bus->getChannel("F16.GroundAvoidance.AltitudeAdvisory", false);
	// set the warning altitude for the agent aircraft.  this is also f16
	// specific at the moment, but should be generalized.
	DataChannel<double>::RefT cara_alow = bus->getSharedChannel("F16.CaraAlow", false);
	if (cara_alow.valid()) cara_alow->value() = 300.0;
}

bool AircraftControl::pullupAdvisory() const {
	if (b_AltitudeAdvisory.valid()) return b_AltitudeAdvisory->value();
	static int xxx = 0; ++xxx; bool debug = (xxx % 100) == 0;
	if (debug) std::cout << "NO ADVISORY CHANNEL\n";
	return (!gearFullyExtended() && -4.0 * vv() > ralt() + 300.0);
}

void AircraftControl::flyPitchHeading(double pitch, double heading, double dt) {
	double pitch_error = b_Pitch->value() - pitch;
	b_PitchInput->value() = m_PitchPID.update(pitch_error, dt);
	holdHeading(heading, dt, 0.0);
}

// This seemingly simple task proved surprisingly difficult to implement
// with even moderately reasonable performance.  There remains considerable
// room for improvement, both in terms of aircraft control and code
// complexity.
bool AircraftControl::flyTowardPosition(
		Vector3 const &target,
		double max_g,
		bool can_invert,
		double dt) {
	static int xxx = 0; bool debug = ((++xxx % 10) == 0);

	if ((target - m_LastTarget).length2() > 10000.0) {
		m_LastTarget = target;
		m_Capture = false;
		m_Done = false;
	}

	Vector3 vdir = velocity();
	Vector3 tdir = target - position();

	double dz = tdir.z();
	double range = tdir.normalize();

	double max_pitch = toRadians(15.0);
	double tpitch = asin(dz / range);

	// if the slant to the target is too steep, and we are reasonably far
	// away, we'll prefer to turn away to allow the altitude to be gained
	// or lost over a greater distance.
	bool too_steep = (fabs(tpitch) > max_pitch) && (fabs(dz) > 500.0);
	if (too_steep) {
		tdir.x() = -tdir.x();
		tdir.y() = -tdir.y();
	}

	double speed = vdir.normalize();
	double offset = tdir * vdir;

	double actual_heading = atan2(vdir.x(), vdir.y());
	double target_heading = atan2(tdir.x(), tdir.y());
	double heading_error = actual_heading - target_heading;
	while (heading_error > toRadians(180.0)) heading_error -= toRadians(360.0);
	while (heading_error < -toRadians(180.0)) heading_error += toRadians(360.0);

	if (!too_steep) {
		if (offset > 0.8) m_Capture = true;
		if ((m_Capture && offset < 0.4) || range < speed * 2.0) m_Done = true;
	}

	// restrict pitch angle.  this is primarily intended for leading a flight
	// under non-combat situations, and avoids lots of complications that
	// occur at high pitch angles (e.g., managing energy state).
	tpitch = clampTo(tpitch, -max_pitch, max_pitch);
	double pitch_error = pitch() - tpitch;
	double vv_error = vv() - speed * sin(tpitch);

	// todo: pass roll_limit in instead of can_invert
	double roll_limit = can_invert ? toRadians(180.0) : toRadians(70.0);
	// don't make extreme turns at low altitude
	roll_limit = std::min(roll_limit, toRadians(ralt() * (70.0 / 200.0)));

	// if diving too steeply force wings toward level
	if (pitch_error < toRadians(-15.0)) {
		roll_limit = toRadians(60.0) * clampTo(1.33 + (pitch_error / toRadians(45.0)), 0.0, 1.0);
	}

	if (vv_error < 0.0 && roll_limit > toRadians(89.0)) {
		roll_limit = toRadians(89.0);
	}

	// TODO try to avoid the dynamic roll limiter (which means we're dropping
	// too low in a turn) by estimating a tighter bound on the maximum roll
	// angle that can sustain the target vv.  for example, the angle at which
	// if we pull an immediate g_limit turn, including gravity, vv_error would
	// not grow more negative.  estimating a more realistic bound on the maximum
	// attainable g at low speeds would also help, but this is aircraft dependent.

	// if losing altitude too quickly despite maximum g command, force wings
	// toward level.
	if (vv_error < 0.0 && m_EngageDynamicRollLimit) {
		m_DynamicRollLimit = std::max(0.1, m_DynamicRollLimit + 0.01 * vv_error * dt);
	} else {
		m_DynamicRollLimit = std::min(1.0, m_DynamicRollLimit + 0.25 * dt);
	}
	m_EngageDynamicRollLimit = false;
	if (debug && m_DynamicRollLimit < 1.0) {
		std::cout << "dynamic roll limit = " << m_DynamicRollLimit << ", vv_error = " << vv_error << "\n";
	}

	// restrict the maximum roll angle when pointing toward the target
	double heading_roll_limit = 2.0 * std::min(fabs(heading_error), toRadians(5.0)) + 2.0 * fabs(heading_error);
	roll_limit = std::min(roll_limit * m_DynamicRollLimit, heading_roll_limit);

	// nominal roll to put lift vector on target; this is generally too much
	// roll since it does not include gravity.  the roll_limit combined with
	// vv feedback corrects for this.
	Vector3 tdir_body = toBody(tdir);
	double roll_target = roll() + atan2(tdir_body.x(), tdir_body.z());

	// experimental: permit rolls past the target when climbing too fast.
	// this is intended to reduce the time required to apply load when
	// starting a turn.
	if (vv_error > 0.0) roll_target += sign(roll_target) * toRadians(vv_error);
	
	roll_target = clampTo(roll_target, -roll_limit, roll_limit);

	// g command is used primarily to control vv.  it is limited to the
	// maximum g specified by the caller, and also to prevent excessive
	// deceleration when below corner velocity.
	double g_limit = std::min(m_DecelerationLimit, max_g);

	// below corner velocity, limit g to prevent deceleration but reserve at
	// least 2 g for maneuvering.
	if (speed < 200.0 && m_ThrottlePID.isClampedHi()) {
		Vector3 vdir_body = toBody(vdir);
		double vdir_accel = accelerationBody() * vdir_body / 9.8;
		m_DecelerationLimit = clampTo(m_DecelerationLimit + 3.0 * vdir_accel * dt, 2.0, max_g);
		if (debug) std::cout << "preventing deceleration, accel = " << vdir_accel << ", limit = " << m_DecelerationLimit << "\n";
	} else {
		m_DecelerationLimit = std::min(m_DecelerationLimit + 2.0 * dt, max_g);
	}

	// dp > 0 => push nose down
	double roll_factor = 1.0 - fabs(roll()) / toRadians(90.0);
	bool inverted = roll_factor < 0.0;
	roll_factor = 1.0 / (0.05 + 0.95 * fabs(roll_factor));
	double dp = 0.0;

	if (vv_error < 0.0) {  // too low
		dp = 0.001 * vv_error * (inverted ? 0.0 : roll_factor);
		if (inverted) m_EngageDynamicRollLimit = true;
	} else {  // too high
		dp = 0.003 * vv_error * (inverted ? -roll_factor : 1.0);
	}

	// prevent alpha from going significantly below zero
	if (alpha() < 0.0) {
		double f = std::min(- alpha() / toRadians(5.0), 1.0);
		dp = dp * (1.0 - f) + alpha();
	}

	// pitch input is primarily controlled by vv, except at high g
	double over_g = b_G->value() - g_limit;
	if (over_g > 0.0) {
		double f = std::min(over_g, 1.0);
		dp = dp * (1.0 - f) + over_g * 0.1;
		if (debug) std::cout << "limiting g " << over_g << "\n";
	}
	if (over_g > -0.2) {
		m_EngageDynamicRollLimit = true;
	}

	b_PitchInput->value() = m_PitchPID.update(dp, dt);
	if (b_PitchInput->value() > 0.98) m_EngageDynamicRollLimit = true;

	if (debug) std::cout << "dp = " << dp << ", pi = " << b_PitchInput->value() << ", vve = " << vv_error << ", edrl = " << m_EngageDynamicRollLimit << ", re = " << toDegrees(roll_target - roll()) << "\n";

	holdRoll(roll_target, dt);
	return m_Done;
}

double AircraftControl::adjustPitch(double error, double dt) {
	// good in theory, but if the speed drops due to a high alpha
	// climb, there isn't any authority to bring the nose back
	// down.  need to work on this.
	error *= std::min(1.0, (velocity().length() - 50.0) / 100.0);
	return b_PitchInput->value() = m_PitchPID.update(error, dt);
}

bool AircraftControl::flyToPositionSpeed(
		Vector3 const &target_position,
		double target_speed,
		double max_g,
		bool can_invert,
		double dt) {
	Vector3 dir = velocity();
	double speed = dir.normalize();

	// FIXME need mil thrust(M, alt) to compute max climb angle, although the
	// current function should behave reasonably well in practice if airspeed
	// drops in the climb.
	double above_stall = cas() - 75.0;
	//double climb_factor = clampTo(above_stall / 75.0, 0.05, 1.0);

	double airbrakes = speed > target_speed + 10.0 ? 1.0 : 0.0;
	setAirbrakeInput(airbrakes);
	b_ThrottleInput->value() = m_ThrottlePID.update(speed - target_speed, dt);

	// FIXME need maxg(M, alt) of the actual jet.  could also just measure
	// this as we go, but it might mean that the jet starts a maneuver in
	// one direction and then reverses when it sees can't make the corner.
	// would be better to do the approximately right thing from the start.
	max_g = std::min(max_g, std::min(9.0, 1.0 + 8.0 * above_stall / 125.0));

	return flyTowardPosition(target_position, max_g, can_invert, dt);
}

// this method was intended to be used for waypoint navigation, but never
// worked as intended.  it is currently in disrepair, and instead
// flyTowardPosition is used for waypoint navigation.
void AircraftControl::flyToPositionVelocity(
		Vector3 const &target_position,
		Vector3 const &target_velocity,
		double max_g,
		bool can_invert,
		double dt) {
	Vector3 dir = velocity();
	Vector3 pos = position();
	Vector3 delta = target_position - pos;

	double speed = dir.normalize();
	Vector3 target_dir = target_velocity;
	double target_speed = target_dir.normalize();
	double target_heading = atan2(target_dir.x(), target_dir.y());

	double dz = delta.z();
	double range = delta.normalize();
	//double bearing = atan2(delta.x(), delta.y());
	double climb_angle = atan2(dz, range);

	// FIXME need mil thrust(M, alt) to compute max climb angle, although the
	// current function should behave reasonably well in practice if airspeed
	// drops in the climb.
	double above_stall = cas() - 75.0;
	double climb_factor = clampTo(above_stall / 75.0, 0.05, 1.0);

	double airbrakes = speed > target_speed + 10.0 ? 1.0 : 0.0;
	setAirbrakeInput(airbrakes);
	b_ThrottleInput->value() = m_ThrottlePID.update(speed - target_speed, dt);

	// FIXME need maxg(M, alt) of the actual jet.  could also just measure
	// this as we go, but it might mean that the jet starts a maneuver in
	// one direction and then reverses when it sees can't make the corner.
	// would be better to do the approximately right thing from the start.
	max_g = std::min(max_g, std::min(9.0, 1.0 + 8.0 * above_stall / 125.0));

	// XXX XXX XXX
	flyTowardPosition(target_position, max_g, can_invert, dt);
	return;

	// FIXME need different logic when very close (e.g., don't trigger
	// the climb away branch), and need to detect and return "close enough".

	static int xxx = 0; ++xxx;
	bool debug = ((xxx % 30) == 0);

	if (fabs(dz) > 500.0) {
		if (climb_angle > toRadians(20.0) * climb_factor) {
			// target is too high.  execute a turning climb away from the target
			// direction; once we are high enough the normal logic will turn back
			// toward the target position and continue the climb on roughly the
			// correct heading.
			flyPitchHeading(toRadians(20.0) * climb_factor, target_heading + 180.0, dt);
			if (debug) std::cout << "climb away\n";
			return;
		}
		
		if (climb_angle < -toRadians(15.0)) {
			std::cout << "TOO LOW\n";
			return;
		}
	}

	if (range > 10000.0) {
		// just fly straight toward target, no need for fancy maneuvers
		if (debug) std::cout << "far\n";
		flyTowardPosition(target_position, max_g, can_invert, dt);
		return;
	}

	if (can_invert) {
		double s = std::max(speed, target_speed);
		double r_min = s * s / (max_g * 9.8);

		Vector3 mp_normal = target_dir ^ delta;
		Vector3 mp_center_dir = mp_normal ^ target_dir;

		Vector3 c = target_position + mp_center_dir * r_min;
		double ox = sqrt(range*range - r_min*r_min) * r_min / range;
		double oy = r_min * r_min / range;
		Vector3 cp = (target_position - c).normalized();
		Vector3 cn = mp_normal ^ cp;
		Vector3 intercept = c + ox * cp + oy * cn;
		std::cout << "ZZ " << pos << " " << intercept << " " << target_position << "\n";

		// FIXME once we reach the intercept point this approach breaks down
		// further, it fails if we are inside the r_min circle
		if (debug) std::cout << "intercept " << intercept << "\n";
		flyTowardPosition(intercept, max_g, can_invert, dt);

	} else {
		// TODO
		std::cout << "IMPLEMENT ME\n";
	}

}

// This was the very first waypoint navigation algorithm that was
// implemented.  It worked ok, but could not command high-g turns.
// See flyTowardPosition for a more sophisticated approach.
void AircraftControl::flyLevelHeading(double altitude, double heading, double dt) {
	static int xxx = 0; ++xxx;
	double error = -(alt() - altitude);
	double vvi = clampTo(error * 0.2, -100.0, 100.0);
	error = vv() - vvi;
//		if (b_Pitch->value() > toRadians(5.0) && error < 0.0) error = b_Pitch->value() - toRadians(5.0);
//		if (b_Pitch->value() < -toRadians(5.0) && error > 0.0) error = b_Pitch->value() + toRadians(5.0);
//		if ((xxx % 100) == 0) std::cout << "alt error = " << toDegrees(error) << ", pitch = " << toDegrees(b_Pitch->value()) << "\n";
//		error = clampTo(error, -1.0, 1.0);
//		b_PitchInput->value() = m_PitchPID.update(error, dt);

	if (error > 40.0) {
		m_Invert = true;
	} else if (error < 5.0) {
		m_Invert = false;
	}

	if ((xxx % 100) == 0) {
		std::cout << "alt = " << alt()
				  << ", vv  = " << vv()
				  << ", target_vv  = " << vvi
				  << ", vv error = " << error
				  << ", pi error = " << (error * 0.01)
				  << ", inv = " << m_Invert << "\n";
	}

	static double last_altitude = 0.0;
	static double smooth_t = 0.0;
	if (fabs(altitude - last_altitude) > 10.0) {
		smooth_t = 5.0;
	}
	last_altitude = altitude;
	if (smooth_t > 0.0) {
		smooth_t -= dt;
		error *= 1.0 - (smooth_t / 5.0);
	}
	b_PitchInput->value() = m_PitchPID.update((m_Invert ? -1.0 : 1.0) * error * 0.005, dt);
	holdHeading(heading, error, dt);
}

void AircraftControl::holdAirspeed(double airspeed, double dt) {
	double error = cas() - airspeed;
	b_ThrottleInput->value() = m_ThrottlePID.update(error, dt);
}

// Wrapper around flyLevelHeading for use in waypoint navigation.
// Returns true when the target waypoint is reached.  As a better
// alternative, see flyToPositionSpeed.
bool AircraftControl::flyToPosition(Vector3 const &target, double dt) {
	Vector3 pos = b_ModelPosition->value();
	Vector3 vel = b_LinearVelocity->value();
	Vector3 delta = target - pos;
	double bearing = atan2(delta.x(), delta.y());
	bool near = false;
	static double on_target = false;
	if (delta.length() < 1000.0) {
		bearing = atan2(vel.x(), vel.y());
		near = true;
	}
	flyLevelHeading(target.z(), bearing, dt);
	if (fabs(m_HeadingError) < toRadians(10.0)) {
		on_target = true;
	} else if (on_target && fabs(m_HeadingError) > toRadians(45.0)) {
		near = true;
		on_target = false;
	}
	return near;
}

void AircraftControl::steerToPoint(Vector3 const &point, double dt) {
	if (!wow()) return;
	Vector3 direction = (point - position()).normalized();
	Vector3 motion = velocity().normalized();
	double error = asin((motion ^ direction).z());
	setRudderInput(m_RudderPID.update(error, dt));
}

void AircraftControl::holdHeading(double heading, double vv_error, double dt) {
	Vector3 vel = b_LinearVelocity->value();
	double actual_heading = atan2(vel.x(), vel.y());
	double heading_error = actual_heading - heading;
	controlHeading(heading_error, vv_error, dt);
}

void AircraftControl::holdAlpha(double target, double dt) {
	double error = 3.0 * (alpha() - target);
	controlPitch(error, dt);
}

void AircraftControl::controlPitch(double error, double dt) {
	b_PitchInput->value() = m_PitchPID.update(error, dt);
}

void AircraftControl::controlThrottle(double error, double dt) {
	b_ThrottleInput->value() = m_ThrottlePID.update(error, dt);
}

void AircraftControl::fastRoll(double roll, double dt) {
	double error = roll - b_Roll->value();
	b_RollInput->value() = m_RollPID.update(5.0 * error, dt);
}

void AircraftControl::holdRoll(double roll, double dt) {
	double error = roll - b_Roll->value();
	if (!gearFullyRetracted()) error *= 6.0;
	b_RollInput->value() = m_RollPID.update(error, dt);
}

void AircraftControl::controlHeading(double heading_error, double vv_error, double dt) {
	while (heading_error > toRadians(180.0)) heading_error -= toRadians(360.0);
	while (heading_error < -toRadians(180.0)) heading_error += toRadians(360.0);
	double max_roll = clampTo(0.02 * (cas() - 65.0), 0.0, toRadians(60.0));
	if (vv_error < 0.0) max_roll *= std::max(0.0, 1.0 + vv_error / 100.0);
	if (!gearFullyRetracted()) {
		max_roll = std::min(max_roll, toRadians(30.0));
		m_Invert = false;
	}
	double roll = clampTo(-3.0 * heading_error, -max_roll, max_roll);
	static int xxx = 0; bool debug = ((++xxx % 60) == 0);
	if (m_Invert) roll = toRadians(180.0) - roll;
	if (debug) std::cout << "heading error: " << toDegrees(heading_error) << " " << roll << " " << m_Invert << "\n";
	holdRoll(roll, dt);
	m_HeadingError = heading_error;
}

CSP_NAMESPACE_END

