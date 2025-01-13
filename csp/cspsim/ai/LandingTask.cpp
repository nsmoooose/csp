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


#include <csp/cspsim/ai/LandingTask.h>
#include <csp/cspsim/ai/AircraftControl.h>
#include <csp/cspsim/ai/Runway.h>
#include <iostream>

namespace csp {
namespace ai {

namespace {
inline double slope(Vector3 const &v) {
	return -atan2(v.z(), sqrt(v.x()*v.x() + v.y()*v.y()));
}
}


LandingTask::LandingTask(): AircraftTask("Landing") {
	addHandler(LINEUP, sigc::mem_fun(*this, &LandingTask::onLineup), "LINEUP");
	addHandler(APPROACH, sigc::mem_fun(*this, &LandingTask::onApproach), "APPROACH");
	addHandler(FINAL_APPROACH, sigc::mem_fun(*this, &LandingTask::onFinalApproach), "FINAL_APPROACH");
	addHandler(AEROBRAKE, sigc::mem_fun(*this, &LandingTask::onAeroBrake), "AEROBRAKE");
	addHandler(BRAKE, sigc::mem_fun(*this, &LandingTask::onBrake), "BRAKE");
	addHandler(ABORT, sigc::mem_fun(*this, &LandingTask::onAbort), "ABORT");
	next(LINEUP);
}

void LandingTask::init() {
	assert(runway());
	assert(ai());
}

void LandingTask::preupdate() {
	Vector3 delta = runway()->aimpoint - ai()->position();
	m_Range = (delta * runway()->direction);

	double aimpoint_offset = clampTo(m_Range * 0.5, 400.0, 1600.0);
	m_AimPoint = runway()->aimpoint + runway()->glide * (m_Range - aimpoint_offset) / cos(runway()->glideslope);
	delta = m_AimPoint - ai()->position();

	Vector3 gdelta = delta - runway()->z * (delta * runway()->z);
	m_Offset = (gdelta ^ runway()->direction).length();
	m_SlantRange = delta.length();
	m_AngleOff = atan2(m_Offset, std::max(m_Range, 400.0));
	m_AimBearing = atan2(delta.x(), delta.y());
	m_TargetSlope = slope(delta);
	m_GlideSlope = slope(ai()->velocity());
}

void LandingTask::onLineup() {
	if (initial()) {
		ai()->raiseGear();
	}

	if (m_TargetSlope < runway()->glideslope) {  // low
		// as long as we aren't too near the ground, don't
		// climb up to the glideslope, just wait to intersect
		// it.  the functional form is chosen to provide
		// smooth transitions as a function of ralt and slope.
		static const double low = 300.0;  // meters
		m_TargetSlope = std::max(m_TargetSlope, -ai()->ralt() / low);
		if (debug()) std::cout << "low\n";
		// TODO should check for inadequate power (stalling) here.
	}

	double speed = 150.0;
	if (m_Range > 6500.0) {
		//ai()->setAirbrakeInput(ai()->cas() > 160 ? 1.0 : 0.0);
		//ai()->holdAirspeed(150.0, dt());
	} else if (m_Range > 5000.0) {
		//ai()->setAirbrakeInput(1.0);
		//ai()->holdAirspeed(125.0, dt());
		speed = 125.0;
	} else {
		ai()->lowerGear();
		//ai()->setAirbrakeInput(1.0);
		speed = 125.0;
		next(APPROACH);
	}

	ai()->flyToPositionSpeed(m_AimPoint, speed, 4.0, false, dt());
	checkCaptureAngle();
	if (checkDive()) return next(ABORT);
}

void LandingTask::checkCaptureAngle() {
	const double limit = toRadians(std::max(m_Range / 160.0, 2.0));
	if (debug()) {
		if (fabs(m_AngleOff) > limit) std::cout << "H: " << toDegrees(m_AngleOff) << "\n";
		if (m_TargetSlope < -limit) std::cout << "S: " << toDegrees(m_TargetSlope) << "\n";
	}
	if (fabs(m_AngleOff) > limit) return next(ABORT);
	if (m_TargetSlope < -limit) return next(ABORT);
}

void LandingTask::onApproach() {
	double slope_error = m_GlideSlope - m_TargetSlope;
	double alpha_error = toRadians(11.0) - ai()->alpha();
	ai()->controlPitch(-3.0 * slope_error, dt());
	ai()->controlThrottle(2.0 * alpha_error, dt());
	ai()->holdHeading(m_AimBearing, 0.0, dt());
	ai()->setAirbrakeInput(1.0);
	checkCaptureAngle();

	if (ai()->alpha() > toRadians(15.0 + ai()->ralt() / 50.0)) return next(ABORT);
	if (ai()->ralt() < 20.0) return next(FINAL_APPROACH);
	if (checkDive()) return next(ABORT);
}

void LandingTask::onFinalApproach() {
	// flare_agl is an important parameter for achieving a soft touchdown
	// without bounce.  11.0 works for the m2k and the f16dj, both with
	// and without full external fuel tanks, but this value may need to be
	// externalized so that it can be adjusted for other aircraft types.
	static const double flare_agl = 20.0;

	double agl = ai()->ralt();
	double flare = clampTo((agl - 2.5) / flare_agl, 0.0, 1.0);
	double slope_error = m_GlideSlope - m_TargetSlope * flare;

	double alpha_target = toRadians(13.0 - 2.0 * flare);
	double alpha_error = alpha_target - ai()->alpha();

	ai()->controlPitch(-1.5 * slope_error, dt());  // WAS -4.0, with flare_agl = 11.0
	ai()->controlThrottle(4.0 * alpha_error, dt());
	ai()->holdHeading(m_AimBearing, 0.0, dt());

	// aborting would only work if was executed immediately on the start
	// of the final approach, but since onApproach didn't abort we are
	// pretty much committed to landing.
	//checkCaptureAngle();

	if (ai()->ralt() < 8.0) ai()->setThrottleInput(0.0);
	if (ai()->wow()) {
		ai()->setThrottleInput(0.0);
		next(AEROBRAKE);
	}
}

void LandingTask::onAeroBrake() {
	ai()->setThrottleInput(0.0);
	if (ai()->cas() < 40.0) return next(BRAKE);
	ai()->holdAlpha(toRadians(13.0), dt());
	ai()->steerToPoint(m_AimPoint, dt());
}

void LandingTask::onBrake() {
	ai()->steerToPoint(m_AimPoint, dt());
	ai()->setBrakes(1.0);

	// taxiing could be done in this task, but it may make
	// more sense to call done() earlier (once the speed is
	// low enough to taxi) and have the higher level logic
	// hand off to a separate taxi task.
	// for now, just roll to a stop on the runway.
	if (ai()->velocity().length2() < 0.1) succeed();
}

void LandingTask::onAbort() {
	if (initial()) {
		m_AbortSafe = false;
		// assume we are below corner velocity, so hit the gas.
		ai()->setThrottleInput(1.0);
		ai()->setAirbrakeInput(0.0);
		Vector3 v = ai()->velocity();
		v.z() = 0.0;
		m_AbortPoint = ai()->position() + v.normalized() * 6000.0;
		m_AbortPoint.z() += 300.0;
	}
	if (!m_AbortSafe && ai()->vv() > 0.0) {
		resetElapsedTime();
		m_AbortSafe = true;
		ai()->raiseGear();
	}
	if (m_AbortSafe) {
		ai()->holdAirspeed(150.0, dt());
		// climb out for a few seconds, then hand off to higher level logic
		// which, for example, can contact atc for new landing vectors.
		if (elapsed() > 5.0) fail();
		if (ai()->flyToPositionSpeed(m_AbortPoint, 150.0, 4.0, false, dt())) {
			m_AbortPoint += Vector3(10000.0, 0.0, 300.0);
			fail();
		}
	} else {
		// at full afterburner, no brakes
		ai()->flyTowardPosition(m_AbortPoint, 4.0, false, dt());
	}
}

} // end namespace ai
} // end namespace csp
