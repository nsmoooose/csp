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
 * @file F16System.cpp
 *
 **/

#include "F16System.h"
#include "F16Channels.h"
#include <CSPSim.h>
#include <FlightDynamicsChannels.h>
#include <NavigationChannels.h>
#include <LandingGearChannels.h>

#include <SimData/Date.h>
#include <SimData/Math.h>
#include <SimData/Conversions.h>


SIMDATA_REGISTER_INTERFACE(F16System)

DEFINE_INPUT_INTERFACE(F16System);

const simdata::Enumeration F16System::MasterModes("NAV AA AG");


F16System::F16System(): m_CatIII(false) {
}

void F16System::registerChannels(Bus* bus) {
	b_TrailingEdgeFlapExtension = bus->registerLocalDataChannel<double>(bus::F16::TrailingEdgeFlapExtension, 0.0);
	b_AirbrakeLimit = bus->registerLocalDataChannel<double>(bus::F16::AirbrakeLimit, 60.0);
	b_AltFlaps = bus->registerSharedDataChannel<bool>(bus::F16::AltFlaps, false);
	b_CatIII = bus->registerSharedDataChannel<bool>(bus::F16::CatIII, false);
	b_StandbyGains = bus->registerLocalDataChannel<bool>(bus::F16::StandbyGains, false);
	b_TakeoffLandingGains = bus->registerLocalDataChannel<bool>(bus::F16::TakeoffLandingGains, false);
	b_NavigationSystem = bus->registerSharedDataChannel<NavigationSystem::Ref>("Navigation", new NavigationSystem);
	b_MasterMode = bus->registerLocalPushChannel<simdata::EnumLink>("MasterMode", MasterMode("NAV"));
	bus->registerSharedDataChannel<double>("F16.CaraAlow", 0.0);
	bus->registerSharedDataChannel<double>("F16.MslFloor", 0.0);
	bus->registerSharedDataChannel<double>("F16.TfAdv", 0.0);
}

void F16System::importChannels(Bus* bus) {
	b_NoseLandingGearWOW = bus->getChannel(bus::LandingGear::selectWOW("FrontGear"));
	b_LeftMainLandingGearWOW = bus->getChannel(bus::LandingGear::selectWOW("LeftGear"));
	b_RightMainLandingGearWOW = bus->getChannel(bus::LandingGear::selectWOW("RightGear"));
	b_GearFullyRetracted = bus->getChannel(bus::LandingGear::FullyRetracted);
	// TODO bus::LandingGear::GearExtendSelected or bus::F16::GearHandleUp?
	b_GearExtendSelected = bus->getChannel(bus::LandingGear::GearExtendSelected);
	b_Airspeed = bus->getChannel(bus::FlightDynamics::Airspeed);
	b_FuelDoorSequence = bus->getChannel("Aircraft.FuelDoorSequence.NormalizedTime", false);
}

double F16System::onUpdate(double) {
	const bool gear_lever_down = b_GearExtendSelected->value();
	const bool gear_fully_retracted  = b_GearFullyRetracted->value();
	if (!gear_fully_retracted || b_AltFlaps->value()) {
		b_TakeoffLandingGains->value() = true;
	} else {
		const bool fuel_door_open = !b_FuelDoorSequence ? false : (b_FuelDoorSequence->value() > 0.01);
		b_TakeoffLandingGains->value() = fuel_door_open;
	}
	b_AirbrakeLimit->value() = (gear_lever_down && !b_NoseLandingGearWOW->value()) ? 43.0 : 60.0;
	if (m_CatIII != b_CatIII->value()) {
		m_CatIII = !m_CatIII;
		// TODO change gains
	}
	return 0.2;
}

void F16System::setCatI() {
	b_CatIII->value() = false;
}

void F16System::setCatIII() {
	b_CatIII->value() = true;
}

void F16System::nextSteerpoint() {
	CSP_LOG(APP, DEBUG, "next steerpoint event");
	if (b_NavigationSystem.valid()) b_NavigationSystem->value()->nextSteerpoint();
}

void F16System::prevSteerpoint() {
	CSP_LOG(APP, DEBUG, "prev steerpoint event");
	if (b_NavigationSystem.valid()) b_NavigationSystem->value()->prevSteerpoint();
}

void F16System::flapsDown() {
	b_AltFlaps->value() = false;
}

void F16System::flapsUp() {
	b_AltFlaps->value() = true;
}

void F16System::flapsToggle() {
	b_AltFlaps->value() = !b_AltFlaps->value();
}