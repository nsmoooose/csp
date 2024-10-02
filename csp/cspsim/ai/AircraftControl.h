#pragma once
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

#include <csp/csplib/data/Quat.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/Bus.h>
#include <csp/cspsim/ai/PID.h>

namespace csp {
namespace ai {

class AircraftControl: public Referenced {
public:
	AircraftControl();

	virtual void importChannels(Bus *bus);

	void raiseGear() { b_GearCommand->push(true); }
	void lowerGear() { b_GearCommand->push(false); }
	void setLeftBrakeInput(double x) { b_LeftBrakeInput->value() = x; }
	void setRightBrakeInput(double x) { b_RightBrakeInput->value() = x; }
	void setThrottleInput(double x) {
		b_ThrottleInput->value() = x;
		m_ThrottlePID.set(x);
	}
	void setPitchInput(double x) { b_PitchInput->value() = x; }
	void setRollInput(double x) { b_RollInput->value() = x; }
	void setRudderInput(double x) { b_RudderInput->value() = x; }
	void setAirbrakeInput(double x) { b_AirbrakeInput->value() = x; }
	void setBrakes(double x) { setLeftBrakeInput(x); setRightBrakeInput(x); }
	double throttleInput() const { return b_ThrottleInput->value(); }
	bool wow() const { return b_WOW->value(); }
	bool gearFullyExtended() const { return b_GearFullyExtended->value(); }
	bool gearFullyRetracted() const { return b_GearFullyRetracted->value(); }
	double roll() const { return b_Roll->value(); }
	double pitch() const { return b_Pitch->value(); }
	double alpha() const { return b_Alpha->value(); }
	double cas() const { return b_CAS->value(); }
	double ralt() const { return b_ModelPosition->value().z() - b_GroundZ->value(); }
	double alt() const { return b_ModelPosition->value().z(); }
	double vv() const { return b_LinearVelocity->value().z(); }
	Vector3 const &position() const { return b_ModelPosition->value(); }
	Vector3 const &velocity() const { return b_LinearVelocity->value(); }
	Vector3 const &accelerationBody() const { return b_AccelerationBody->value(); }
	Quat const &attitude() { return b_Attitude->value(); }
	Vector3 toBody(Vector3 const &v) { return attitude().invrotate(v); }
	Vector3 toWorld(Vector3 const &v) { return attitude().rotate(v); }

	bool pullupAdvisory() const;
	void flyPitchHeading(double pitch, double heading, double dt);
	bool flyTowardPosition(Vector3 const &target, double max_g, bool can_invert, double dt);
	double adjustPitch(double error, double dt);
	bool flyToPositionSpeed(Vector3 const &target_position, double target_speed, double max_g, bool can_invert, double dt);

	// this method is intended to be used for waypoint navigation.
	// it is currently stateless, so velocity changes can't be
	// spread uniformly over entire maneuver.
	void flyToPositionVelocity(Vector3 const &target_position, Vector3 const &target_velocity, double max_g, bool can_invert, double dt);
	void flyLevelHeading(double altitude, double heading, double dt);
	void holdAirspeed(double airspeed, double dt);
	bool flyToPosition(Vector3 const &target, double dt);
	bool flyToPosition(double x, double y, double z, double dt) {
		return flyToPosition(Vector3(x, y, z), dt);
	}

	// TODO reverse this --- pitch controls alpha, throttle controls glide slope?
	// seems reasonable on approach, but we don't want to throttle up to flare.
	// we also don't want to pitch up when airspeed is too high; start with the
	// current approach then switch once aoa is high enough?
	// also, detect bad approaches (too steep, too far off angle)
	bool flyApproach(double x, double y, double z, double dt);

	void steerToPoint(Vector3 const &point, double dt);
	void holdHeading(double heading, double vv_error, double dt);
	void holdAlpha(double target, double dt);
	void controlPitch(double error, double dt);
	void controlThrottle(double error, double dt);
	void fastRoll(double roll, double dt);
	void holdRoll(double roll, double dt);
	void controlHeading(double heading_error, double vv_error, double dt);

private:
	DataChannel<double>::RefT b_PitchInput;
	DataChannel<double>::RefT b_RollInput;
	DataChannel<double>::RefT b_RudderInput;
	DataChannel<double>::RefT b_LeftBrakeInput;
	DataChannel<double>::RefT b_RightBrakeInput;
	DataChannel<double>::RefT b_ThrottleInput;
	DataChannel<double>::RefT b_AirbrakeInput;

	DataChannel<bool>::RefT b_GearCommand;
	DataChannel<bool>::CRefT b_WOW;
	DataChannel<bool>::CRefT b_GearFullyExtended;
	DataChannel<bool>::CRefT b_GearFullyRetracted;
	DataChannel<bool>::CRefT b_AltitudeAdvisory;
	DataChannel<double>::CRefT b_CAS;
	DataChannel<double>::CRefT b_PressureAltitude;
	DataChannel<double>::CRefT b_VerticalVelocity;
	DataChannel<double>::CRefT b_Alpha;
	DataChannel<double>::CRefT b_Airspeed;
	DataChannel<double>::CRefT b_G;
	DataChannel<double>::CRefT b_GroundZ;
	DataChannel<double>::CRefT b_Pitch;
	DataChannel<double>::CRefT b_Roll;
	DataChannel<double>::CRefT b_Heading;
	DataChannel<Vector3>::CRefT b_ModelPosition;
	DataChannel<Vector3>::CRefT b_LinearVelocity;
	DataChannel<Vector3>::CRefT b_AccelerationBody;
	DataChannel<Quat>::CRefT b_Attitude;

	PID m_PitchPID;
	PID m_RollPID;
	PID m_ThrottlePID;
	PID m_RudderPID;

	bool m_Invert;
	bool m_FinalApproach;
	double m_HeadingError;
	double m_DecelerationLimit;
	double m_DynamicRollLimit;
	bool m_EngageDynamicRollLimit;
	double m_TargetG;
	Vector3 m_LastTarget;
	bool m_Capture;
	bool m_Done;
};

} // end namespace ai
} // end namespace csp
