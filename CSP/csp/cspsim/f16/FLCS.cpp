// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file F16/FLCS.cpp
 *
 **/

#include <csp/cspsim/f16/F16Channels.h>
#include <csp/cspsim/ControlNode.h>
#include <csp/cspsim/Bus.h>
#include <csp/cspsim/ConditionsChannels.h>
#include <csp/cspsim/ControlInputsChannels.h>
#include <csp/cspsim/ControlSurfacesChannels.h>
#include <csp/cspsim/Filters.h>
#include <csp/cspsim/FlightDynamicsChannels.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/LandingGearChannels.h>

#include <csp/csplib/util/Conversions.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/data/LUT.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/data/Vector3.h>

CSP_NAMESPACE

class LeadingEdgeFlapControl: public ControlNode {
public:
	CSP_DECLARE_OBJECT(LeadingEdgeFlapControl)
	LeadingEdgeFlapControl(): m_AlphaFilter(3.75, 7.25, 2.0) { }
	void importChannels(Bus* bus);
private:
	virtual void evaluate(Timer const &timer);
	DataChannel<double>::CRefT b_Alpha;
	DataChannel<double>::CRefT b_Pressure;
	DataChannel<double>::CRefT b_QBar;
	DataChannel<double>::CRefT b_ThrottleInput;
	DataChannel<bool>::CRefT b_LeftMainLandingGearWOW;
	DataChannel<bool>::CRefT b_RightMainLandingGearWOW;
	DataChannel<bool>::CRefT b_WheelSpin;
	double m_AlphaGain;
	double m_QBarGain;
	double m_Offset;
	double m_GroundDeflection;
	LeadLagFilter m_AlphaFilter;
};

CSP_XML_BEGIN(LeadingEdgeFlapControl)
	CSP_DEF("dlef_alpha", m_AlphaGain, true)
	CSP_DEF("dlef_qbar_ps", m_QBarGain, true)
	CSP_DEF("dlef_offset", m_Offset, true)
	CSP_DEF("dlef_ground_deflection", m_GroundDeflection, true)
CSP_XML_END


void LeadingEdgeFlapControl::evaluate(Timer const &timer) {
	double dlef = 0.0;
	m_AlphaFilter.update(b_Alpha->value(), timer.dt());
	if (0 /*m_Gain == StandbyGain*/) {
		// TODO implement me
	} else {
		const bool on_ground = b_LeftMainLandingGearWOW->value() && b_RightMainLandingGearWOW->value();
		if (on_ground || (b_WheelSpin->value() && b_ThrottleInput->value() < 0.01)) {
			dlef = m_GroundDeflection;
		} else {
			dlef = m_AlphaFilter.value() * m_AlphaGain + b_QBar->value()/b_Pressure->value() * m_QBarGain + m_Offset;
		}
	}
	setOutput(dlef, timer);
}

void LeadingEdgeFlapControl::importChannels(Bus* bus) {
	assert(b_Alpha.isNull());
	b_Alpha = bus->getChannel(bus::FlightDynamics::Alpha);
	b_Pressure = bus->getChannel(bus::Conditions::Pressure);
	b_QBar = bus->getChannel(bus::FlightDynamics::QBar);
	b_LeftMainLandingGearWOW = bus->getChannel(bus::LandingGear::selectWOW("LeftGear"));
	b_RightMainLandingGearWOW = bus->getChannel(bus::LandingGear::selectWOW("RightGear"));
	b_WheelSpin = bus->getChannel(bus::F16::WheelSpin);
	b_ThrottleInput = bus->getChannel(bus::ControlInputs::ThrottleInput);
}


class TrailingEdgeFlapControl: public ControlNode {
public:
	CSP_DECLARE_OBJECT(TrailingEdgeFlapControl)
	TrailingEdgeFlapControl(): m_AirspeedFilter(2.0) { }
	void importChannels(Bus* bus);
private:
	virtual void evaluate(Timer const &timer);
	virtual void postCreate();
	DataChannel<double>::CRefT b_Airspeed;
	DataChannel<bool>::CRefT b_AltFlaps;
	DataChannel<bool>::CRefT b_GearHandleUp;
	LagFilter m_AirspeedFilter;
	double m_DeflectionLimit;
	double m_Airspeed1;
	double m_Airspeed2;
	double m_AirspeedDelta;
};

CSP_XML_BEGIN(TrailingEdgeFlapControl)
	CSP_DEF("deflection_limit", m_DeflectionLimit, true)
	CSP_DEF("airspeed_break1", m_Airspeed1, true)
	CSP_DEF("airspeed_break2", m_Airspeed2, true)
CSP_XML_END


void TrailingEdgeFlapControl::postCreate() {
	m_AirspeedDelta = m_Airspeed2 - m_Airspeed1;
	assert(m_AirspeedDelta > 0);
}

void TrailingEdgeFlapControl::evaluate(Timer const &timer) {
	m_AirspeedFilter.update(b_Airspeed->value(), timer.dt());
	double deflection = 0.0;
	if (!b_GearHandleUp->value() || b_AltFlaps->value()) {
		double kts = convert::mps_kts(m_AirspeedFilter.value());
		deflection = m_DeflectionLimit * (1.0 - clampTo((kts - m_Airspeed1) / m_AirspeedDelta, 0.0, 1.0));
	}
	setOutput(deflection, timer);
}

void TrailingEdgeFlapControl::importChannels(Bus* bus) {
	assert(b_Airspeed.isNull());
	b_Airspeed = bus->getChannel(bus::FlightDynamics::Airspeed);
	b_AltFlaps = bus->getChannel(bus::F16::AltFlaps);
	b_GearHandleUp = bus->getChannel(bus::F16::GearHandleUp);
}


// Blackbox pitch limiter.  Has three inputs: alpha_f, pitch_rate, and g_force.
// The output is the "effective g force" which the pitch control circuit attempts
// to balance against the g force commanded by the pilot.  This component
// implements the circuit in the lower left portion of fig. 62, page 210 of
// the Nasa 1979 paper.  The output is the sum of the two vertical arrows on
// either side of either side of the "0.5" gain box near the center of the
// figure.  The alpha_break1 and alpha_break2 values are responsible for the
// standard F-16 AoA/G limiter behavior which restricts G force to +9G below
// 15 deg AoA, and to progressively smaller G forces at higher AoA.  The
// conventional values for these two constants are 15 deg and 20.4 degrees,
// respectively.

class PitchLimiterControl: public ControlNode {
public:
	CSP_DECLARE_OBJECT(PitchLimiterControl)
	PitchLimiterControl(): m_PitchRateDeltaFilter(0.0, 1.0, 1.0), m_GLimitFilter(4.0, 12.0, 3.0), m_RollRateFilter(0.67) { }
	virtual void importChannels(Bus* bus);
	virtual void link(MapID &map);
private:
	virtual void evaluate(Timer const &timer);
	DataChannel<double>::CRefT b_G;
	DataChannel<Vector3>::CRefT b_AngularVelocityBody;
	DataChannel<Vector3>::CRefT b_AccelerationBody;
	DataChannel<double>::CRefT b_QBar;
	DataChannel<bool>::CRefT b_TakeoffLandingGains;
	DataChannel<bool>::CRefT b_CatIII;
	DataChannel<bool>::CRefT b_ManualPitchOverride;
	DataChannel<bool>::RefT b_ManualPitchOverrideActive;
	TypeID m_FilteredGCommandID;
	TypeID m_FilteredAlphaID;
	ControlNode::RefT m_FilteredAlpha;
	ControlNode::RefT m_FilteredGCommand;
	double m_AlphaBreak1;
	double m_AlphaBreak2;
	Table1 m_PitchRateSchedule;
	LeadLagFilter m_PitchRateDeltaFilter;
	LeadLagFilter m_GLimitFilter;
	LagFilter m_RollRateFilter;
};

CSP_XML_BEGIN(PitchLimiterControl)
	CSP_DEF("filtered_g_command", m_FilteredGCommandID, true)
	CSP_DEF("filtered_alpha", m_FilteredAlphaID, true)
	CSP_DEF("alpha_break1", m_AlphaBreak1, true)
	CSP_DEF("alpha_break2", m_AlphaBreak2, true)
	CSP_DEF("pitch_rate_schedule", m_PitchRateSchedule, true)
CSP_XML_END


void PitchLimiterControl::evaluate(Timer const &timer) {
	CSPLOG(DEBUG, APP) << "PitchLimiterControl.evaluate";

	double alpha_f = m_FilteredAlpha->step(timer);

	// mpo bypasses the g-limiter.  according to code-one, the mpo only gives
	// direct control for negative pitch commands, or positive commands when
	// alpha > 29 deg.  (Jan 1986, "F-16 Flight Controls" by Joe Bill Dryden).
	// note that the article also claims direct control of the control when
	// WOW, but that is contradicted by other articles that mention horizontal
	// stabilizer deflections when taxiing as the flcs tries to "smooth out"
	// the runway.
	const bool mpo = b_ManualPitchOverride->value() && (alpha_f >= 29.0 || m_FilteredGCommand->step(timer) < 0);
	b_ManualPitchOverrideActive->value() = mpo;
	if (mpo) {
		setOutput(0.0, timer);
		return;
	}

	// pitch axis modification for nasa control system b (see fig. 28, page 131)
	double roll_rate = toDegrees(b_AngularVelocityBody->value().y());
	m_RollRateFilter.update(0.15 * std::abs(roll_rate), timer.dt());
	double delta_alpha = clampTo(m_RollRateFilter.value() - 3.0, 0.0, 5.4);
	double comp_alpha = alpha_f + delta_alpha;

	double pitch_rate = toDegrees(b_AngularVelocityBody->value().x());
	m_PitchRateDeltaFilter.update(pitch_rate, timer.dt());

	double alpha_break1 = m_AlphaBreak1;
	double alpha_break2 = m_AlphaBreak2;
	double g_minus_1 = b_G->value() - 1.0;  // omits 50/(s+50) lag filter
	double pitch_rate_control = 0.0;
	double gain = 1.0;
	double alpha1_gain = 0.322;
	if (b_TakeoffLandingGains->value()) {  // pitch rate command system for TO/L gains
		pitch_rate_control = 2.0 * pitch_rate; // ad-hoc multiplier, no solid data
		g_minus_1 = 0.0;
		alpha_break1 = 10.0;
		alpha1_gain = 1.0; // 8 G command negated at 21 deg AOA.

		// what is alpha_break2, and are any of the slopes different?
		alpha_break2 = 90.0; // disable

		// reducing the loop gain keeps the feedback stable at high speed, and helps
		// to reduce pitch oscillations during transients moments such as gear extension.
		gain = 0.4;

		// at very high speed, pitch rate control allows 9G to be exceeded.  presumably
		// TLG should be restricted at high speed.  (it can be engaged without lowering
		// the gear by toggling either the alt flaps or air refuel switches.)
	}
	// todo: supersonic limiter.  based on raptor/mav-jp's hffm data, the limiter kicks
	// in at mach 1, restricting aoa to 20 deg.  aoa drops by 2 deg per 0.1 M until 16
	// degrees and holds roughly constant.
	double reduced_pitch_rate = m_PitchRateSchedule[static_cast<float>(b_QBar->value())] * 0.7 * m_PitchRateDeltaFilter.value();
	double alpha_break1_compensation = reduced_pitch_rate + comp_alpha - alpha_break1;
	double g_limit = std::max(0.0, alpha1_gain * alpha_break1_compensation) + 0.334 * m_PitchRateDeltaFilter.value() + g_minus_1;
	m_GLimitFilter.update(g_limit, timer.dt());
	double alpha_break2_compensation = std::max(0.0, comp_alpha + reduced_pitch_rate - alpha_break2);
	setOutput(gain * (alpha_break2_compensation + m_GLimitFilter.value() + pitch_rate_control), timer);
}

void PitchLimiterControl::importChannels(Bus* bus) {
	assert(b_AngularVelocityBody.isNull());
	b_AngularVelocityBody = bus->getChannel(bus::Kinetics::AngularVelocityBody);
	b_AccelerationBody = bus->getChannel(bus::Kinetics::AccelerationBody);
	b_QBar = bus->getChannel(bus::FlightDynamics::QBar);
	b_G = bus->getChannel(bus::FlightDynamics::G);
	b_TakeoffLandingGains = bus->getChannel(bus::F16::TakeoffLandingGains);
	b_CatIII = bus->getChannel(bus::F16::CatIII);
	b_ManualPitchOverride = bus->getChannel(bus::F16::ManualPitchOverride);
	b_ManualPitchOverrideActive = bus->getSharedChannel(bus::F16::ManualPitchOverrideActive);
}

void PitchLimiterControl::link(MapID &map) {
	// Lag(clampTo(alpha, -5deg, 30deg), 10.0) implemented externally
	m_FilteredAlpha = map[m_FilteredAlphaID];
	m_FilteredGCommand = map[m_FilteredGCommandID];
	assert(m_FilteredAlpha.valid());
	assert(m_FilteredGCommand.valid());
}


class RollLimiterControl: public ControlNode {
public:
	CSP_DECLARE_OBJECT(RollLimiterControl)
	RollLimiterControl(): m_RollCommandFilter(10.0), m_YawRateFilter(50.0) { }
	virtual void importChannels(Bus* bus);
private:
	virtual void evaluate(Timer const &timer);
	DataChannel<Vector3>::CRefT b_AngularVelocityBody;
	DataChannel<double>::CRefT b_Alpha;
	DataChannel<double>::CRefT b_QBar;
	DataChannel<double>::CRefT b_ElevatorDeflection;
	DataChannel<double>::CRefT b_RollInput;
	DataChannel<bool>::CRefT b_TakeoffLandingGains;
	DataChannel<bool>::CRefT b_CatIII;
	Table1 m_RollCommandSchedule;
	LagFilter m_RollCommandFilter;
	LagFilter m_YawRateFilter;
};

CSP_XML_BEGIN(RollLimiterControl)
	CSP_DEF("roll_command_schedule", m_RollCommandSchedule, true)
CSP_XML_END


void RollLimiterControl::evaluate(Timer const &timer) {
	double alpha = toDegrees(b_Alpha->value());
	// negative since yaw axis (and hence yaw rate) is opposite nasa 1979.
	m_YawRateFilter.update(toDegrees(-b_AngularVelocityBody->value().z()), timer.dt());
	double roll_signal = 1.0;
	if (alpha >= 29.0) {
		double r_f = m_YawRateFilter.value();
		roll_signal = 8.34 * r_f;
	} else {
		// roll axis modification for nasa control system b (see fig. 35, page 146)
		double qbar_limit = -0.0115 * std::min(0.0, b_QBar->value() - 10500.0);
		double alpha_limit = 4.0 * std::max(0.0, alpha - 15.0);
		double htail_limit = 4.0 * std::max(0.0, toDegrees(b_ElevatorDeflection->value()) - 5.0);
		double limit = qbar_limit + alpha_limit + htail_limit;
		double roll_rate = toDegrees(b_AngularVelocityBody->value().y());
		limit = limit * clampTo((std::abs(roll_rate) - 30.0) / 20.0, 0.0, 1.0);
		limit = clampTo(limit, 0.0, 228.0);
		limit = 308.0 - limit;

		double roll_command = m_RollCommandSchedule[static_cast<float>(b_RollInput->value() * 80.0)];
		if (b_TakeoffLandingGains->value()) {
			roll_command = 0.50 * roll_command;
		} else {
			roll_command = clampTo(roll_command, -limit, limit);
			if (b_CatIII->value()) roll_command *= 0.40;  // not sure about this!
		}
		// XXX omits positive and negative rate feedback loops
		m_RollCommandFilter.update(roll_command, timer.dt());
		// XXX omits roll rate filter
		roll_signal = roll_rate - m_RollCommandFilter.value();
	}
	double da = clampTo(0.12 * roll_signal, -21.5, 21.5);
	setOutput(da, timer);
}

void RollLimiterControl::importChannels(Bus* bus) {
	assert(b_AngularVelocityBody.isNull());
	b_AngularVelocityBody = bus->getChannel(bus::Kinetics::AngularVelocityBody);
	b_Alpha = bus->getChannel(bus::FlightDynamics::Alpha);
	b_QBar = bus->getChannel(bus::FlightDynamics::QBar);
	b_ElevatorDeflection = bus->getChannel(bus::ControlSurfaces::ElevatorDeflection);
	b_RollInput = bus->getChannel(bus::ControlInputs::RollInput);
	b_TakeoffLandingGains = bus->getChannel(bus::F16::TakeoffLandingGains);
	b_CatIII = bus->getChannel(bus::F16::CatIII);
}


class YawLimiterControl: public ControlNode {
public:
	CSP_DECLARE_OBJECT(YawLimiterControl)
	YawLimiterControl(): m_YawRateFilter(50.0), m_CoupledFilter0(5.0, 15.0, 3.0), m_CoupledFilter1(0.0, 1.5, 1.0) { }
	virtual void importChannels(Bus* bus);
private:
	virtual void evaluate(Timer const &timer);
	DataChannel<Vector3>::CRefT b_AngularVelocityBody;
	DataChannel<double>::CRefT b_LateralG;
	DataChannel<double>::CRefT b_Alpha;
	DataChannel<double>::CRefT b_QBar;
	DataChannel<bool>::CRefT b_ManualPitchOverrideActive;
	LagFilter m_YawRateFilter;
	LeadLagFilter m_CoupledFilter0;
	LeadLagFilter m_CoupledFilter1;
};

CSP_XML_BEGIN(YawLimiterControl)
CSP_XML_END

void YawLimiterControl::evaluate(Timer const &timer) {
	// negative since yaw axis (and hence yaw rate) is opposite nasa 1979.
	m_YawRateFilter.update(toDegrees(-b_AngularVelocityBody->value().z()), timer.dt());
	double r_f = m_YawRateFilter.value();
	double alpha = toDegrees(b_Alpha->value());

	// Not sure if this is the correct anti-spin departure control and mpo behavior.  The
	// spin control is based on the nasa fcs yaw limiter circuit, while the mpo behavior
	// is based on an ambiguous paragraph in the dash-one.
	if (alpha >= 29.0 && !b_ManualPitchOverrideActive->value()) {
		setOutput(0.75 * r_f, timer);
		return;
	}

	double p = toDegrees(b_AngularVelocityBody->value().y());
	double r_stab = r_f - p * (1.0 / 57.3) * alpha;
	m_CoupledFilter0.update(r_stab, timer.dt());
	m_CoupledFilter1.update(m_CoupledFilter0.value(), timer.dt());
	double output = 0.5 * (m_CoupledFilter1.value() + b_LateralG->value() * 19.32);

	// XXX omits aileron response

	setOutput(output, timer);
}

void YawLimiterControl::importChannels(Bus* bus) {
	assert(b_AngularVelocityBody.isNull());
	b_AngularVelocityBody = bus->getChannel(bus::Kinetics::AngularVelocityBody);
	b_LateralG = bus->getChannel(bus::FlightDynamics::LateralG);
	b_Alpha = bus->getChannel(bus::FlightDynamics::Alpha);
	b_QBar = bus->getChannel(bus::FlightDynamics::QBar);
	b_ManualPitchOverrideActive = bus->getChannel(bus::F16::ManualPitchOverrideActive);
}

CSP_NAMESPACE_END

