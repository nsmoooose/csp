// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002, 2003 The Combat Simulator Project
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


#include "Systems/AircraftInputSystem.h"


SIMDATA_REGISTER_INTERFACE(AircraftInputSystem)

DEFINE_INPUT_INTERFACE(AircraftInputSystem);

AircraftInputSystem::AircraftInputSystem(): 
	m_ThrottleInput(0.2)
{
}

void AircraftInputSystem::registerChannels(Bus *bus) {
	m_PitchInput.connect(bus, "ControlInputs.PitchInput");
	m_RollInput.connect(bus, "ControlInputs.RollInput");
	m_RudderInput.connect(bus, "ControlInputs.RudderInput");
	m_LeftBrakeInput.connect(bus, "ControlInputs.LeftBrakeInput");
	m_RightBrakeInput.connect(bus, "ControlInputs.RightBrakeInput");
	m_ThrottleInput.connect(bus, "ControlInputs.ThrottleInput");
	m_AirbrakeInput.connect(bus, "ControlInputs.AirbrakeInput");
}

void AircraftInputSystem::importChannels(Bus *bus) {
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

void AircraftInputSystem::setAirbrake(double x) { 
	m_AirbrakeInput.setValue(x); 
}

void AircraftInputSystem::setThrottle(double x) { 
	m_ThrottleInput.setValue(0.5 *(1.0 - x)); 
}

void AircraftInputSystem::setRudder(double x)
{ 
	m_RudderInput.setValue(x); 
}

void AircraftInputSystem::IncRudder() { 
	m_RudderInput.setIncrement(1.0); 
}

void AircraftInputSystem::noIncRudder() { 
	m_RudderInput.stopIncrement();
	m_RudderInput.setDecay(-30);
}

void AircraftInputSystem::DecRudder() { 
	m_RudderInput.setIncrement(-1.0); 
}

void AircraftInputSystem::noDecRudder() { 
	m_RudderInput.stopDecrement();
	m_RudderInput.setDecay(30);
}

void AircraftInputSystem::setRoll(double x)
{ 
	m_RollInput.setValue(x); 
}

void AircraftInputSystem::setPitch(double x)
{ 
	m_PitchInput.setValue(x); 
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

void AircraftInputSystem::setWheelBrake(double x) {
	m_LeftBrakeInput.setValue(x);
	m_RightBrakeInput.setValue(x);
}

void AircraftInputSystem::OpenAirbrake() {
	m_AirbrakeInput.setValue(1.0);
}

void AircraftInputSystem::CloseAirbrake() {
	m_AirbrakeInput.setValue(0.0);
}

void AircraftInputSystem::DecAirbrake() {
	double v = std::max<double>(0.0, m_AirbrakeInput.getValue() - 0.1);
	m_AirbrakeInput.setValue(v);
}

void AircraftInputSystem::IncAirbrake() {
	double v = std::min<double>(0.0, m_AirbrakeInput.getValue() + 0.1);
	m_AirbrakeInput.setValue(v);
}


