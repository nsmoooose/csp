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
 * @file AircraftInputSystem.cpp
 *
 **/


#include <csp/cspsim/systems/AircraftInputSystem.h>
#include <csp/cspsim/ControlInputsChannels.h>

#include <csp/csplib/data/ObjectInterface.h>

namespace csp {

CSP_XML_BEGIN(AircraftInputSystem)
CSP_XML_END

DEFINE_INPUT_INTERFACE(AircraftInputSystem)


class AircraftInputSystem::CombinedInput {
public:
	CombinedInput(double rate=0.4, double decay=0.9, double minimum=-1.0, double maximum=1.0):
		m_Rate(rate),
		m_Decay(decay),
		m_DecayCount(0),
		m_Increment(0.0),
		m_Minimum(minimum),
		m_Maximum(maximum) { }

	void connect(Bus *bus, std::string const &name) {
		m_Channel = bus->registerLocalDataChannel<double>(name, 0.0);
	}

	void update(double dt) {
		double v = m_Channel->value();
		v += m_Increment * dt * m_Rate;
		if (m_DecayCount > 0) {
			m_DecayCount--;
			if (m_Increment == 0.0) {
				if (m_DecayCount == 0) {
					v = 0.0;
				} else {
					v *= m_Decay;
				}
			}
		}
		m_Channel->value() = clampTo(v, m_Minimum, m_Maximum);
	}

	double getValue() const { return m_Channel->value(); }
	void setValue(double value) { m_Channel->value() = value; }
	void setIncrement(double increment) { m_Increment = increment; }
	void stopIncrement() { if (m_Increment > 0.0) m_Increment = 0.0; }
	void stopDecrement() { if (m_Increment < 0.0) m_Increment = 0.0; }
	void setDecay(int decay_count) { m_DecayCount = decay_count; }

private:
	double m_Rate;
	double m_Decay;
	int m_DecayCount;
	double m_Increment;
	double m_Minimum;
	double m_Maximum;
	DataChannel<double>::RefT m_Channel;
};


AircraftInputSystem::AircraftInputSystem():
	m_PitchInput(new CombinedInput),
	m_RollInput(new CombinedInput),
	m_RudderInput(new CombinedInput),
	m_LeftBrakeInput(new CombinedInput),
	m_RightBrakeInput(new CombinedInput),
	m_ThrottleInput(new CombinedInput(/*rate=*/0.2, /*decay=*/1.0, /*min=*/0.0, /*max=*/1.0)),
	m_AirbrakeInput(new CombinedInput) {
}

void AircraftInputSystem::registerChannels(Bus *bus) {
	m_PitchInput->connect(bus, bus::ControlInputs::PitchInput);
	m_RollInput->connect(bus, bus::ControlInputs::RollInput);
	m_RudderInput->connect(bus, bus::ControlInputs::RudderInput);
	m_LeftBrakeInput->connect(bus, bus::ControlInputs::LeftBrakeInput);
	m_RightBrakeInput->connect(bus, bus::ControlInputs::RightBrakeInput);
	m_ThrottleInput->connect(bus, bus::ControlInputs::ThrottleInput);
	m_AirbrakeInput->connect(bus, bus::ControlInputs::AirbrakeInput);
}

void AircraftInputSystem::importChannels(Bus *) {
}


double AircraftInputSystem::onUpdate(double dt) {
	m_PitchInput->update(dt);
	m_RollInput->update(dt);
	m_RudderInput->update(dt);
	m_LeftBrakeInput->update(dt);
	m_RightBrakeInput->update(dt);
	m_ThrottleInput->update(dt);
	m_AirbrakeInput->update(dt);
	return 0.05;
}


// input event callbacks

void AircraftInputSystem::setAirbrake(input::MapEvent::AxisEvent const &event) {
	m_AirbrakeInput->setValue(event.value);
}

void AircraftInputSystem::setThrottle(input::MapEvent::AxisEvent const &event) {
	m_ThrottleInput->setValue(0.5 *(1.0 - event.value));
}

void AircraftInputSystem::setRudder(input::MapEvent::AxisEvent const &event) {
	m_RudderInput->setValue(event.value);
}

void AircraftInputSystem::IncRudder() {
	m_RudderInput->setIncrement(1.0);
}

void AircraftInputSystem::noIncRudder() {
	m_RudderInput->stopIncrement();
	m_RudderInput->setDecay(30);
}

void AircraftInputSystem::DecRudder() {
	m_RudderInput->setIncrement(-1.0);
}

void AircraftInputSystem::noDecRudder() {
	m_RudderInput->stopDecrement();
	m_RudderInput->setDecay(30);
}

void AircraftInputSystem::setRoll(input::MapEvent::AxisEvent const &event) {
	m_RollInput->setValue(event.value);
}

void AircraftInputSystem::setPitch(input::MapEvent::AxisEvent const &event) {
	m_PitchInput->setValue(event.value);
}

void AircraftInputSystem::IncPitch() {
	m_PitchInput->setIncrement(1.0);
}	

void AircraftInputSystem::noIncPitch() {
	m_PitchInput->stopIncrement();
	m_PitchInput->setDecay(30);
}

void AircraftInputSystem::DecPitch() {
	m_PitchInput->setIncrement(-1.0);
}

void AircraftInputSystem::noDecPitch() {
	m_PitchInput->stopDecrement();
	m_PitchInput->setDecay(30);
}

void AircraftInputSystem::IncRoll() {
	m_RollInput->setIncrement(1.0);
}

void AircraftInputSystem::noIncRoll() {
	m_RollInput->stopIncrement();
	m_RollInput->setDecay(30);
}

void AircraftInputSystem::DecRoll() {
	m_RollInput->setIncrement(-1.0);
}

void AircraftInputSystem::noDecRoll() {
	m_RollInput->stopDecrement();
	m_RollInput->setDecay(30);
}

void AircraftInputSystem::IncThrottle() {
	m_ThrottleInput->setIncrement(1.0);
}

void AircraftInputSystem::noIncThrottle() {
	m_ThrottleInput->stopIncrement();
}

void AircraftInputSystem::DecThrottle() {
	m_ThrottleInput->setIncrement(-1.0);
}

void AircraftInputSystem::noDecThrottle() {
	m_ThrottleInput->stopDecrement();
}

void AircraftInputSystem::WheelBrakePulse() {
	m_LeftBrakeInput->setValue(1.0);
	m_LeftBrakeInput->setDecay(30);
	m_RightBrakeInput->setValue(1.0);
	m_RightBrakeInput->setDecay(30);
}

void AircraftInputSystem::WheelBrakeOn() {
	m_LeftBrakeInput->setValue(1.0);
	m_RightBrakeInput->setValue(1.0);
	m_WheelBrakes = true;
}

void AircraftInputSystem::WheelBrakeOff() {
	m_LeftBrakeInput->setValue(0.0);
	m_RightBrakeInput->setValue(0.0);
	m_WheelBrakes = false;
}

void AircraftInputSystem::WheelBrakeToggle() {
	if (m_WheelBrakes) {
		WheelBrakeOff();
	} else {
		WheelBrakeOn();
	}
}

void AircraftInputSystem::setWheelBrake(input::MapEvent::AxisEvent const &event) {
	m_LeftBrakeInput->setValue(event.value);
	m_RightBrakeInput->setValue(event.value);
}

void AircraftInputSystem::OpenAirbrake() {
	m_AirbrakeInput->setValue(1.0);
}

void AircraftInputSystem::CloseAirbrake() {
	m_AirbrakeInput->setValue(0.0);
}

void AircraftInputSystem::DecAirbrake() {
	double v = std::max(0.0, m_AirbrakeInput->getValue() - 0.1);
	m_AirbrakeInput->setValue(v);
}

void AircraftInputSystem::IncAirbrake() {
	double v = std::min(0.0, m_AirbrakeInput->getValue() + 0.1);
	m_AirbrakeInput->setValue(v);
}

} // namespace csp

