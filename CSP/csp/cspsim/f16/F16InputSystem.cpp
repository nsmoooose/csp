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
 * @file F16InputSystem.cpp
 *
 **/


#include "F16InputSystem.h"


SIMDATA_REGISTER_INTERFACE(F16InputSystem)


void AircraftInputSystem::setAirbrake(double x) {
	m_AirbrakeInput.setValue(x);
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


