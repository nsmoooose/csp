#include <ControlNode.h>
#include <Bus.h>
#include <ConditionsChannels.h>
#include <ControlInputsChannels.h>
#include <ControlSurfacesChannels.h>
#include <Filters.h>
#include <FlightDynamicsChannels.h>
#include <KineticsChannels.h>
#include <LandingGearChannels.h>
#include <SimData/Conversions.h>
#include <SimData/Math.h>
#include <SimData/Vector3.h>
#include <SimData/LUT.h>

#include "F16Channels.h"


class LeadingEdgeFlapControl: public ControlNode {
public:
	SIMDATA_OBJECT(LeadingEdgeFlapControl, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(LeadingEdgeFlapControl, ControlNode)
		SIMDATA_XML("dlef_alpha", LeadingEdgeFlapControl::m_AlphaGain, true)
		SIMDATA_XML("dlef_qbar_ps", LeadingEdgeFlapControl::m_QBarGain, true)
		SIMDATA_XML("dlef_offset", LeadingEdgeFlapControl::m_Offset, true)
		SIMDATA_XML("dlef_ground_deflection", LeadingEdgeFlapControl::m_GroundDeflection, true)
	END_SIMDATA_XML_INTERFACE
	LeadingEdgeFlapControl(): m_AlphaFilter(3.75, 7.25, 2.0) { }
	void importChannels(Bus* bus);
private:
	virtual void evaluate(Timer const &timer);
	DataChannel<double>::CRef b_Alpha;
	DataChannel<double>::CRef b_Pressure;
	DataChannel<double>::CRef b_QBar;
	DataChannel<double>::CRef b_ThrottleInput;
	DataChannel<bool>::CRef b_LeftMainLandingGearWOW;
	DataChannel<bool>::CRef b_RightMainLandingGearWOW;
	DataChannel<bool>::CRef b_WheelSpin;
	double m_AlphaGain;
	double m_QBarGain;
	double m_Offset;
	double m_GroundDeflection;
	LeadLagFilter m_AlphaFilter;
};

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

SIMDATA_REGISTER_INTERFACE(LeadingEdgeFlapControl)



class TrailingEdgeFlapControl: public ControlNode {
public:
	SIMDATA_OBJECT(TrailingEdgeFlapControl, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(TrailingEdgeFlapControl, ControlNode)
		SIMDATA_XML("deflection_limit", TrailingEdgeFlapControl::m_DeflectionLimit, true)
		SIMDATA_XML("airspeed_break1", TrailingEdgeFlapControl::m_Airspeed1, true)
		SIMDATA_XML("airspeed_break2", TrailingEdgeFlapControl::m_Airspeed2, true)
	END_SIMDATA_XML_INTERFACE
	TrailingEdgeFlapControl(): m_AirspeedFilter(2.0) { }
	void importChannels(Bus* bus);
private:
	virtual void evaluate(Timer const &timer);
	virtual void postCreate();
	DataChannel<double>::CRef b_Airspeed;
	DataChannel<bool>::CRef b_AltFlaps;
	DataChannel<bool>::CRef b_GearHandleUp;
	LagFilter m_AirspeedFilter;
	double m_DeflectionLimit;
	double m_Airspeed1;
	double m_Airspeed2;
	double m_AirspeedDelta;
};

void TrailingEdgeFlapControl::postCreate() {
	m_AirspeedDelta = m_Airspeed2 - m_Airspeed1;
	assert(m_AirspeedDelta > 0);
}

void TrailingEdgeFlapControl::evaluate(Timer const &timer) {
	m_AirspeedFilter.update(b_Airspeed->value(), timer.dt());
	double deflection = 0.0;
	if (!b_GearHandleUp->value() || b_AltFlaps->value()) {
		double kts = simdata::convert::mps_kts(m_AirspeedFilter.value());
		deflection = m_DeflectionLimit * (1.0 - simdata::clampTo((kts - m_Airspeed1) / m_AirspeedDelta, 0.0, 1.0));
	}
	setOutput(deflection, timer);
}

void TrailingEdgeFlapControl::importChannels(Bus* bus) {
	assert(b_Airspeed.isNull());
	b_Airspeed = bus->getChannel(bus::FlightDynamics::Airspeed);
	b_AltFlaps = bus->getChannel(bus::F16::AltFlaps);
	b_GearHandleUp = bus->getChannel(bus::F16::GearHandleUp);
}

SIMDATA_REGISTER_INTERFACE(TrailingEdgeFlapControl)



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
	SIMDATA_OBJECT(PitchLimiterControl, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(PitchLimiterControl, ControlNode)
		SIMDATA_XML("filtered_alpha", PitchLimiterControl::m_FilteredAlphaID, true)
		SIMDATA_XML("alpha_break1", PitchLimiterControl::m_AlphaBreak1, true)
		SIMDATA_XML("alpha_break2", PitchLimiterControl::m_AlphaBreak2, true)
		SIMDATA_XML("pitch_rate_schedule", PitchLimiterControl::m_PitchRateSchedule, true)
	END_SIMDATA_XML_INTERFACE
	PitchLimiterControl(): m_PitchRateDeltaFilter(0.0, 1.0, 1.0), m_GLimitFilter(4.0, 12.0, 3.0), m_RollRateFilter(0.67) { }
	virtual void importChannels(Bus* bus);
	virtual void link(MapID &map);
private:
	virtual void evaluate(Timer const &timer);
	DataChannel<double>::CRef b_GForce;
	DataChannel<simdata::Vector3>::CRef b_AngularVelocityBody;
	DataChannel<simdata::Vector3>::CRef b_AccelerationBody;
	DataChannel<double>::CRef b_QBar;
	DataChannel<bool>::CRef b_TakeoffLandingGains;
	DataChannel<bool>::CRef b_CatIII;
	TypeID m_FilteredAlphaID;
	ControlNode::Ref m_FilteredAlpha;
	double m_AlphaBreak1;
	double m_AlphaBreak2;
	simdata::Table1 m_PitchRateSchedule;
	LeadLagFilter m_PitchRateDeltaFilter;
	LeadLagFilter m_GLimitFilter;
	LagFilter m_RollRateFilter;
};

void PitchLimiterControl::evaluate(Timer const &timer) {
	CSP_LOG(APP, DEBUG, "PitchLimiterControl.evaluate ");
	// pitch axis modification for nasa control system b (see fig. 28, page 131)
	double roll_rate = simdata::toDegrees(b_AngularVelocityBody->value().y());
	m_RollRateFilter.update(0.15 * std::abs(roll_rate), timer.dt());
	double delta_alpha = simdata::clampTo(m_RollRateFilter.value() - 3.0, 0.0, 5.4);
	double alpha_f = m_FilteredAlpha->step(timer) + delta_alpha;

	double pitch_rate = simdata::toDegrees(b_AngularVelocityBody->value().x());
	m_PitchRateDeltaFilter.update(pitch_rate, timer.dt());

	double alpha_break1 = m_AlphaBreak1;
	double alpha_break2 = m_AlphaBreak2;
	double g_minus_1 = b_GForce->value() - 1.0;  // omits 50/(s+50) lag filter
	double pitch_rate_control = 0.0;
	if (b_TakeoffLandingGains->value()) {  // pitch rate command system for TO/L gains
		pitch_rate_control = 1.8 * pitch_rate; // ad-hoc multiplier
		g_minus_1 = 0.0;
		alpha_break1 = 10.0;
		// what is alpha_break2, and are any of the slopes different?
	}
	double reduced_pitch_rate = m_PitchRateSchedule[static_cast<float>(b_QBar->value())] * 0.7 * m_PitchRateDeltaFilter.value();
	double alpha_break1_compensation = reduced_pitch_rate + alpha_f - alpha_break1;
	double g_limit = std::max(0.0, 0.322 * alpha_break1_compensation) + 0.334 * m_PitchRateDeltaFilter.value() + g_minus_1;
	m_GLimitFilter.update(g_limit, timer.dt());
	double alpha_break2_compensation = std::max(0.0, alpha_f + reduced_pitch_rate - alpha_break2);
	setOutput(alpha_break2_compensation + m_GLimitFilter.value() + pitch_rate_control, timer);
}

void PitchLimiterControl::importChannels(Bus* bus) {
	assert(b_AngularVelocityBody.isNull());
	b_AngularVelocityBody = bus->getChannel(bus::Kinetics::AngularVelocityBody);
	b_AccelerationBody = bus->getChannel(bus::Kinetics::AccelerationBody);
	b_QBar = bus->getChannel(bus::FlightDynamics::QBar);
	b_GForce = bus->getChannel(bus::FlightDynamics::GForce);
	b_TakeoffLandingGains = bus->getChannel(bus::F16::TakeoffLandingGains);
	b_CatIII = bus->getChannel(bus::F16::CatIII);
}

void PitchLimiterControl::link(MapID &map) {
	// Lag(clampTo(alpha, -5deg, 30deg), 10.0) implemented externally
	m_FilteredAlpha = map[m_FilteredAlphaID];
	assert(m_FilteredAlpha.valid());
}

SIMDATA_REGISTER_INTERFACE(PitchLimiterControl)



class RollLimiterControl: public ControlNode {
public:
	SIMDATA_OBJECT(RollLimiterControl, 0, 0)
	EXTEND_SIMDATA_XML_INTERFACE(RollLimiterControl, ControlNode)
		SIMDATA_XML("roll_command_schedule", RollLimiterControl::m_RollCommandSchedule, true)
	END_SIMDATA_XML_INTERFACE
	RollLimiterControl(): m_RollCommandFilter(10.0) { }
	virtual void importChannels(Bus* bus);
private:
	virtual void evaluate(Timer const &timer);
	DataChannel<simdata::Vector3>::CRef b_AngularVelocityBody;
	DataChannel<double>::CRef b_Alpha;
	DataChannel<double>::CRef b_QBar;
	DataChannel<double>::CRef b_ElevatorDeflection;
	DataChannel<double>::CRef b_RollInput;
	DataChannel<bool>::CRef b_TakeoffLandingGains;
	DataChannel<bool>::CRef b_CatIII;
	simdata::Table1 m_RollCommandSchedule;
	LagFilter m_RollCommandFilter;
};

void RollLimiterControl::evaluate(Timer const &timer) {
	// roll axis modification for nasa control system b (see fig. 35, page 146)
	double qbar_limit = -0.0115 * std::min(0.0, b_QBar->value() - 10500.0);
	double alpha_limit = 4.0 * std::max(0.0, simdata::toDegrees(b_Alpha->value()) - 15.0);
	double htail_limit = 4.0 * std::max(0.0, simdata::toDegrees(b_ElevatorDeflection->value()) - 5.0);
	double limit = qbar_limit + alpha_limit + htail_limit;
	double roll_rate = simdata::toDegrees(b_AngularVelocityBody->value().y());
	limit = limit * simdata::clampTo((std::abs(roll_rate) - 30.0) / 50.0, 0.0, 1.0);
	limit = simdata::clampTo(limit, 0.0, 228.0);
	limit = 308.0 - limit;

	double roll_command = m_RollCommandSchedule[static_cast<float>(b_RollInput->value() * 80.0)];
	if (b_TakeoffLandingGains->value()) {
		roll_command = 0.50 * roll_command;
	} else {
		roll_command = simdata::clampTo(roll_command, -limit, limit);
		if (b_CatIII->value()) roll_command *= 0.40;  // not sure about this!
	}
	// XXX omits positive and negative rate feedback loops
	m_RollCommandFilter.update(roll_command, timer.dt());
	double roll_signal = m_RollCommandFilter.value() - roll_rate;
	// XXX omits high alpha spin correction
	// roll_signal = 8.34 * raw_rate_f;  // at alpha >= 29
	double deflection = simdata::clampTo(0.12 * roll_signal, -21.5, 21.5);
	setOutput(deflection, timer);
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

SIMDATA_REGISTER_INTERFACE(RollLimiterControl)
