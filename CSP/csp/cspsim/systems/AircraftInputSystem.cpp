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

CSP_NAMESPACE

CSP_XML_BEGIN(AircraftInputSystem)
CSP_XML_END

DEFINE_INPUT_INTERFACE(AircraftInputSystem)

AircraftInputSystem::AircraftInputSystem(): m_ThrottleInput(0.2 /*rate*/, 1.0 /*decay*/, 0.0 /*min*/, 1.0 /*max*/)
{
}

void AircraftInputSystem::registerChannels(Bus *bus) {
	m_PitchInput.connect(bus, bus::ControlInputs::PitchInput);
	m_RollInput.connect(bus, bus::ControlInputs::RollInput);
	m_RudderInput.connect(bus, bus::ControlInputs::RudderInput);
	m_LeftBrakeInput.connect(bus, bus::ControlInputs::LeftBrakeInput);
	m_RightBrakeInput.connect(bus, bus::ControlInputs::RightBrakeInput);
	m_ThrottleInput.connect(bus, bus::ControlInputs::ThrottleInput);
	m_AirbrakeInput.connect(bus, bus::ControlInputs::AirbrakeInput);
}

void AircraftInputSystem::importChannels(Bus *) {
}


double AircraftInputSystem::onUpdate(double dt)
{
	m_PitchInput.update(dt);
	m_RollInput.update(dt);
	m_RudderInput.update(dt);
	m_LeftBrakeInput.update(dt);
	m_RightBrakeInput.update(dt);
	m_ThrottleInput.update(dt);
	m_AirbrakeInput.update(dt);
	return 0.05;
}


// input event callbacks

void AircraftInputSystem::setAirbrake(MapEvent::AxisEvent const &event) {
	m_AirbrakeInput.setValue(event.value);
}

void AircraftInputSystem::setThrottle(MapEvent::AxisEvent const &event) {
	m_ThrottleInput.setValue(0.5 *(1.0 - event.value));
}

void AircraftInputSystem::setRudder(MapEvent::AxisEvent const &event) {
	m_RudderInput.setValue(event.value);
}

void AircraftInputSystem::IncRudder() {
	m_RudderInput.setIncrement(1.0);
}

void AircraftInputSystem::noIncRudder() {
	m_RudderInput.stopIncrement();
	m_RudderInput.setDecay(30);
}

void AircraftInputSystem::DecRudder() {
	m_RudderInput.setIncrement(-1.0);
}

void AircraftInputSystem::noDecRudder() {
	m_RudderInput.stopDecrement();
	m_RudderInput.setDecay(30);
}

void AircraftInputSystem::setRoll(MapEvent::AxisEvent const &event) {
	m_RollInput.setValue(event.value);
}

void AircraftInputSystem::setPitch(MapEvent::AxisEvent const &event) {
	m_PitchInput.setValue(event.value);
}

void AircraftInputSystem::IncPitch() {
	m_PitchInput.setIncrement(1.0);
}	

void AircraftInputSystem::noIncPitch() {
	m_PitchInput.stopIncrement();
	m_PitchInput.setDecay(30);
}

void AircraftInputSystem::DecPitch() {
	m_PitchInput.setIncrement(-1.0);
}

void AircraftInputSystem::noDecPitch() {
	m_PitchInput.stopDecrement();
	m_PitchInput.setDecay(30);
}

void AircraftInputSystem::IncRoll() {
	m_RollInput.setIncrement(1.0);
}

void AircraftInputSystem::noIncRoll() {
	m_RollInput.stopIncrement();
	m_RollInput.setDecay(30);
}

void AircraftInputSystem::DecRoll() {
	m_RollInput.setIncrement(-1.0);
}

void AircraftInputSystem::noDecRoll() {
	m_RollInput.stopDecrement();
	m_RollInput.setDecay(30);
}

void AircraftInputSystem::IncThrottle() {
	m_ThrottleInput.setIncrement(1.0);
}

void AircraftInputSystem::noIncThrottle() {
	m_ThrottleInput.stopIncrement();
}

void AircraftInputSystem::DecThrottle() {
	m_ThrottleInput.setIncrement(-1.0);
}

void AircraftInputSystem::noDecThrottle() {
	m_ThrottleInput.stopDecrement();
}

void AircraftInputSystem::WheelBrakePulse() {
	m_LeftBrakeInput.setValue(1.0);
	m_LeftBrakeInput.setDecay(30);
	m_RightBrakeInput.setValue(1.0);
	m_RightBrakeInput.setDecay(30);
}

void AircraftInputSystem::WheelBrakeOn() {
	m_LeftBrakeInput.setValue(1.0);
	m_RightBrakeInput.setValue(1.0);
	m_WheelBrakes = true;
}

void AircraftInputSystem::WheelBrakeOff() {
	m_LeftBrakeInput.setValue(0.0);
	m_RightBrakeInput.setValue(0.0);
	m_WheelBrakes = false;
}

void AircraftInputSystem::WheelBrakeToggle() {
	if (m_WheelBrakes) {
		WheelBrakeOff();
	} else {
		WheelBrakeOn();
	}
}

void AircraftInputSystem::setWheelBrake(MapEvent::AxisEvent const &event) {
	m_LeftBrakeInput.setValue(event.value);
	m_RightBrakeInput.setValue(event.value);
}

void AircraftInputSystem::OpenAirbrake() {
	m_AirbrakeInput.setValue(1.0);
}

void AircraftInputSystem::CloseAirbrake() {
	m_AirbrakeInput.setValue(0.0);
}

void AircraftInputSystem::DecAirbrake() {
	double v = std::max(0.0, m_AirbrakeInput.getValue() - 0.1);
	m_AirbrakeInput.setValue(v);
}

void AircraftInputSystem::IncAirbrake() {
	double v = std::min(0.0, m_AirbrakeInput.getValue() + 0.1);
	m_AirbrakeInput.setValue(v);
}

CSP_NAMESPACE_END

