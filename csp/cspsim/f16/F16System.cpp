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

#include <csp/cspsim/f16/F16System.h>
#include <csp/cspsim/f16/F16Channels.h>
#include <csp/cspsim/AnimationSequence.h>
#include <csp/cspsim/FlightDynamicsChannels.h>
#include <csp/cspsim/FuelManagementSystem.h>
#include <csp/cspsim/input/InputEventChannel.h>
#include <csp/cspsim/KineticsChannels.h>
#include <csp/cspsim/LandingGearChannels.h>
#include <csp/cspsim/NavigationChannels.h>
#include <csp/cspsim/sound/SoundEffect.h>
#include <csp/cspsim/SystemsModel.h>

// for testing stores release
#include <csp/cspsim/stores/StoresManagementSystem.h>
#include <csp/cspsim/stores/Stores.h>

#include <csp/csplib/data/Date.h>
#include <csp/csplib/data/ObjectInterface.h>
#include <csp/csplib/util/Conversions.h>
#include <csp/csplib/util/Math.h>


namespace csp {

CSP_XML_BEGIN(F16System)
	CSP_DEF("fuel_door_sequence", m_FuelDoorSequence, false)
	CSP_DEF("canopy_sequence", m_CanopySequence, false)
CSP_XML_END

DEFINE_INPUT_INTERFACE(F16System)


F16System::F16System(): m_CatIII(false) {
}

void F16System::registerChannels(Bus* bus) {
	b_TrailingEdgeFlapExtension = bus->registerLocalDataChannel<double>(bus::F16::TrailingEdgeFlapExtension, 0.0);
	b_AirbrakeLimit = bus->registerLocalDataChannel<double>(bus::F16::AirbrakeLimit, 60.0);
	b_AltFlaps = bus->registerSharedDataChannel<bool>(bus::F16::AltFlaps, false);
	b_CatIII = bus->registerSharedDataChannel<bool>(bus::F16::CatIII, false);
	b_ManualPitchOverride = bus->registerSharedDataChannel<bool>(bus::F16::ManualPitchOverride, false);
	b_ManualPitchOverrideActive = bus->registerSharedDataChannel<bool>(bus::F16::ManualPitchOverrideActive, false);
	b_StandbyGains = bus->registerLocalDataChannel<bool>(bus::F16::StandbyGains, false);
	b_TakeoffLandingGains = bus->registerLocalDataChannel<bool>(bus::F16::TakeoffLandingGains, false);
	b_NavigationSystem = bus->registerSharedDataChannel<Ref<NavigationSystem> >("Navigation", new NavigationSystem);
	b_MasterMode = bus->registerChannel(new f16::MasterModeSelection);
	bus->registerSharedDataChannel<double>("F16.CaraAlow", 0.0);
	bus->registerSharedDataChannel<double>("F16.MslFloor", 0.0);
	bus->registerSharedDataChannel<double>("F16.TfAdv", 0.0);
	if (m_FuelDoorSequence.valid()) {
		m_FuelDoorSequence->registerChannels(bus);
	}
	if (m_CanopySequence.valid()) {
		m_CanopySequence->registerChannels(bus);
	}
}

void F16System::importChannels(Bus* bus) {
	b_NoseLandingGearWOW = bus->getChannel(bus::LandingGear::selectWOW("FrontGear"));
	b_LeftMainLandingGearWOW = bus->getChannel(bus::LandingGear::selectWOW("LeftGear"));
	b_RightMainLandingGearWOW = bus->getChannel(bus::LandingGear::selectWOW("RightGear"));
	b_GearFullyRetracted = bus->getChannel(bus::LandingGear::FullyRetracted);
	b_GearFullyExtended = bus->getChannel(bus::LandingGear::FullyExtended);
	b_GearExtendSelected = bus->getChannel(bus::LandingGear::GearExtendSelected);
	b_GearHandleUp = bus->getChannel(bus::F16::GearHandleUp);
	b_Airspeed = bus->getChannel(bus::FlightDynamics::Airspeed);
	b_Alpha = bus->getChannel(bus::FlightDynamics::Alpha);
	b_Pitch = bus->getChannel(bus::Kinetics::Pitch);
	b_Position = bus->getChannel(bus::Kinetics::Position);
	b_Velocity = bus->getChannel(bus::Kinetics::Velocity);
	b_PressureAltitude = bus->getChannel(bus::FlightDynamics::PressureAltitude);
	b_FuelDoorSequence = bus->getChannel("Aircraft.FuelDoorSequence.NormalizedTime", false);
	b_AirRefuelSwitch = bus->getChannel("F16.AirRefuelSwitch", false);
	b_EngineRpm = bus->getChannel("Engine.RPM", true);
	b_FuelManagementSystem = bus->getChannel("FuelManagementSystem");

	if (b_AirRefuelSwitch.valid()) {
		b_AirRefuelSwitch->connect(this, &F16System::onAirRefuelSwitch);
	}

	m_LowSpeedWarning = addSoundEffect("low_speed_warning_sound", SoundEffect::HEADSET);
	m_ConfigurationWarning = addSoundEffect("configuration_warning_sound", SoundEffect::HEADSET);
}

double F16System::onUpdate(double dt) {
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
	// TODO the non-animation updates only require infrequent (0.2s) calls, so it might
	// be better to move the sequence updates elsewhere.
	if (m_FuelDoorSequence.valid()) {
		m_FuelDoorSequence->update(dt);
	}
	if (m_CanopySequence.valid()) {
		m_CanopySequence->update(dt);
	}
	updateWarnings(dt);
	return 0.0;
}

void F16System::setCatI() {
	b_CatIII->value() = false;
}

void F16System::setCatIII() {
	b_CatIII->value() = true;
}

void F16System::nextSteerpoint() {
	CSPLOG(DEBUG, APP) << "next steerpoint event";
	if (b_NavigationSystem.valid()) b_NavigationSystem->value()->nextSteerpoint();
}

void F16System::prevSteerpoint() {
	CSPLOG(DEBUG, APP) << "prev steerpoint event";
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

void F16System::onAirRefuelSwitch() {
	if (m_FuelDoorSequence.valid()) {
		if (b_AirRefuelSwitch.valid() && b_AirRefuelSwitch->value() == "OPEN") {
			m_FuelDoorSequence->playForward();
		} else {
			m_FuelDoorSequence->playBackward();
		}
	}
}

void F16System::onEjectCenterTank() {
	// test jettison
	StoresManagementSystem *sms = getModel()->getStoresManagementSystem().get(); // TODO should return a raw ptr
	if (sms) {
		sms->releaseStore(sms->getHardpointByName("HP5").child(0));
		// testing asymmetric loadouts
		sms->releaseStore(sms->getHardpointByName("HP4").child(0));
	}
}

void F16System::onEjectWingTanks() {
	// test jettison
	StoresManagementSystem *sms = getModel()->getStoresManagementSystem().get(); // TODO should return a raw ptr
	if (sms) {
		//sms->releaseStore(sms->getHardpointByName("HP4").child(0));
		sms->releaseStore(sms->getHardpointByName("HP6").child(0));
	}
}

void F16System::canopyToggle() {
	if (m_CanopySequence.valid()) {
		m_CanopySequence->play();
	}
}

void F16System::engageMPO() {
	b_ManualPitchOverride->value() = true;
}

void F16System::disengageMPO() {
	b_ManualPitchOverride->value() = false;
}

void F16System::getInfo(InfoList &) const {
}

// TODO use LocalUpdate to call this only occasionally
void F16System::updateWarnings(double) {
	double alpha = toDegrees(b_Alpha->value());
	double pitch = toDegrees(b_Pitch->value());
	double airspeed = convert::mps_kts(b_Airspeed->value());
	bool horn = false;
	bool tone = false;
	bool light = false;
	if (alpha > 15.0 && (!b_GearHandleUp->value() || b_AltFlaps->value())) {
		tone = true;;
	} else if (b_GearHandleUp->value() && !b_AltFlaps->value() && pitch >= 45.0) {
		double limit = 200.0 - (90.0 - pitch) * (100.0 / 45.0);
		tone = (airspeed < limit);
	}
	if (airspeed < 170.0) {
		static const double altitude_limit = convert::ft_m(10000.0);
		if (b_Position->value().z() < altitude_limit) {
			static const double vv_limit = convert::ft_m(250.0 / 60.0);  // fpm to mps
			if (b_Velocity->value().z() < -vv_limit) {
				if (!b_GearFullyExtended->value()) {
					horn = true;
					light = true;
				} else if (b_TrailingEdgeFlapExtension->value() < toRadians(24.9)) {
					light = true;
				}
			}
		}
	}

	// FIXME replace statics with member varibales
	static bool low = false;
	if (tone != low) {
		low = tone;
		if (m_LowSpeedWarning.valid()) m_LowSpeedWarning->play(tone);
	}
	static bool gear = false;
	if (gear != (horn || light)) {
		gear = !gear;
		if (m_ConfigurationWarning.valid()) m_ConfigurationWarning->play(horn);
	}
}

} // namespace csp

