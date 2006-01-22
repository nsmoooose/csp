// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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
 * @file GroundAvoidanceAdvisory.cpp
 *
 **/

#include <csp/cspsim/f16/GroundAvoidanceAdvisory.h>
#include <csp/cspsim/f16/F16Channels.h>
#include <csp/cspsim/ConditionsChannels.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/LandingGearChannels.h>

#include <csp/csplib/util/Conversions.h>
#include <csp/csplib/util/Log.h>
#include <csp/csplib/util/Math.h>
#include <csp/csplib/data/ObjectInterface.h>

CSP_NAMESPACE

CSP_XML_BEGIN(GroundAvoidanceAdvisory)
CSP_XML_END

GroundAvoidanceAdvisory::GroundAvoidanceAdvisory():
	m_MinimumDescentRate(convert::ft_m(960.0)/60.0),
	m_RollRate(toRadians(120.0)),
	m_InverseG(1.0 / (4.0 * 9.8)),
	m_DescentWarningState(DISARMED) {
}

void GroundAvoidanceAdvisory::registerChannels(Bus* bus) {
	b_AltitudeAdvisory = bus->registerLocalPushChannel<bool>("F16.GroundAvoidance.AltitudeAdvisory", false);
	b_AdvanceAltitudeAdvisory = bus->registerLocalDataChannel<bool>("F16.GroundAvoidance.AdvanceAdvisory", false);
	b_DescentWarningAfterTakeoff = bus->registerLocalPushChannel<bool>("F16.GroundAvoidance.DescentWarningAfterTakeoff", false);
	b_PullupAnticipation = bus->registerLocalDataChannel<double>("F16.GroundAvoidance.PullupAnticipation", 0.0);
}

void GroundAvoidanceAdvisory::importChannels(Bus* bus) {
	b_Position = bus->getChannel(bus::Kinetics::Position);
	b_Velocity = bus->getChannel(bus::Kinetics::Velocity);
	b_GroundZ = bus->getChannel(bus::Kinetics::GroundZ);
	b_Roll = bus->getChannel(bus::Kinetics::Roll);
	b_Pitch = bus->getChannel(bus::Kinetics::Pitch);
	b_CAS = bus->getChannel(bus::Conditions::CAS);
	b_GearHandleUp = bus->getChannel(bus::F16::GearHandleUp);
	b_WOW = bus->getChannel(bus::LandingGear::WOW);
	b_CaraAlow = bus->getChannel("F16.CaraAlow");
}

double GroundAvoidanceAdvisory::onUpdate(double dt) {
	updateTakeoff(dt);
	if (!b_GearHandleUp->value()) {
		reset();
		return 0.5;
	}
	const double descent_velocity = -b_Velocity->value().z();
	updateDescentWarning(descent_velocity);
	if (descent_velocity < m_MinimumDescentRate * 0.5) {
		reset();
		return 0.5;
	}
	if (descent_velocity < m_MinimumDescentRate) {
		reset();
		return 0.2;
	}
	updateAltitudeAdvisories(descent_velocity);
	return (b_PullupAnticipation->value() > 0.0) ? 0.0 : 0.2;
}

void GroundAvoidanceAdvisory::reset() {
	b_AltitudeAdvisory->pushOnChange(false);
	b_AdvanceAltitudeAdvisory->value() = false;
	b_PullupAnticipation->value() = 0.0;
}

void GroundAvoidanceAdvisory::updateAltitudeAdvisories(const double descent_velocity) {
	const double advance_altitude_loss = descent_velocity * 2.0;
	const double reaction_altitude_loss = descent_velocity * 1.0;  // TODO 0.45 in A-G mode
	const double roll_altitude_loss = descent_velocity * std::abs(b_Roll->value()) / m_RollRate;
	const double recovery_altitude_loss = b_Velocity->value().length2() * (1.0 - cos(b_Pitch->value())) * m_InverseG;
	const double predicted_altitude_loss = advance_altitude_loss + reaction_altitude_loss + roll_altitude_loss + recovery_altitude_loss;
	const double cas_factor = clampTo(375.0 - convert::mps_kts(b_CAS->value()), 0.0, 50.0);
	const double buffer_factor = cas_factor * (0.125 / 50.0);
	const double buffer_offset = convert::ft_m(cas_factor * 2.0 + 50.0);
	const double buffer = buffer_factor * predicted_altitude_loss + buffer_offset;
	const double alow = convert::ft_m(b_CaraAlow->value());
	const double maximum_loss = b_Position->value().z() - b_GroundZ->value() - buffer - alow;
	const double advance_alert_loss = std::max(maximum_loss - (predicted_altitude_loss - advance_altitude_loss), 0.0);
	b_AltitudeAdvisory->pushOnChange(maximum_loss < (predicted_altitude_loss - advance_altitude_loss));
	b_AdvanceAltitudeAdvisory->value() = maximum_loss < predicted_altitude_loss;
	b_PullupAnticipation->value() = 1.0 - clampTo(advance_alert_loss / descent_velocity * 0.125, 0.0, 1.000001);
}

void GroundAvoidanceAdvisory::updateTakeoff(const double dt) {
	if (b_WOW->value()) {
		m_RunwayAltitude = b_Position->value().z();
		m_TakeoffElapsedTime = 0.0;
		m_DescentWarningState = ENABLED;
		b_DescentWarningAfterTakeoff->pushOnChange(false);
	} else if (m_DescentWarningState != DISARMED) {
		m_TakeoffElapsedTime += dt;
		const double msl_above_runway = b_Position->value().z() - m_RunwayAltitude;
		if ((m_TakeoffElapsedTime > 180.0) || (msl_above_runway > convert::ft_m(10000.0))) {
			m_DescentWarningState = DISARMED;
		} else if (msl_above_runway > convert::ft_m(300.0)) {
			m_DescentWarningState = ARMED;
		}
	}
}

void GroundAvoidanceAdvisory::updateDescentWarning(const double descent_velocity) {
	if (m_DescentWarningState == DISARMED) return;
	if ((m_DescentWarningState == ARMED) && (descent_velocity > 0.0)) {
		const double msl_above_runway = b_Position->value().z() - m_RunwayAltitude;
		const double impact_time = msl_above_runway / descent_velocity;
		if (impact_time < 30.0) {
			b_DescentWarningAfterTakeoff->pushOnChange(true);
		} else if (impact_time > 35.0) {  // hysteresis (may not be corrrect)
			if (b_DescentWarningAfterTakeoff->value()) {
				b_DescentWarningAfterTakeoff->pushOnChange(false);
				m_DescentWarningState = DISARMED;
			}
		}
	} else {
		if (b_DescentWarningAfterTakeoff->value()) {
			b_DescentWarningAfterTakeoff->pushOnChange(false);
			m_DescentWarningState = DISARMED;
		}
	}
}

CSP_NAMESPACE_END

