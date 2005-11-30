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
 * @file PanelHUD.cpp
 *
 **/


#include <csp/cspsim/f16/PanelHUD.h>
#include <csp/csplib/data/ObjectInterface.h>

CSP_NAMESPACE

CSP_XML_BEGIN(PanelHUD)
CSP_XML_END

DEFINE_INPUT_INTERFACE(PanelHUD)

PanelHUD::PanelHUD() { }
PanelHUD::~PanelHUD() { }

void PanelHUD::registerChannels(Bus* bus) {
	b_DataSwitch = bus->registerSharedDataChannel<EnumLink>("HUD.DataSwitch", Enum<DataSwitchSettings>("DATA"));
	b_FlightPathMarkerSwitch = bus->registerSharedDataChannel<EnumLink>("HUD.FlightPathMarkerSwitch", Enum<FlightPathMarkerSettings>("ATT/FPM"));
	b_ScalesSwitch = bus->registerSharedDataChannel<EnumLink>("HUD.ScalesSwitch", Enum<ScalesSwitchSettings>("VAH"));
	b_VelocitySwitch = bus->registerSharedDataChannel<EnumLink>("HUD.VelocitySwitch", Enum<VelocitySwitchSettings>("CAS"));
	b_AltitudeSwitch = bus->registerSharedDataChannel<EnumLink>("HUD.AltitudeSwitch", Enum<AltitudeSwitchSettings>("BARO"));
}

void PanelHUD::importChannels(Bus*) {
}

void PanelHUD::onCycleDataSwitch() {
	b_DataSwitch->value().cycle();
}

void PanelHUD::onCycleFlightPathMarkerSwitch() {
	b_FlightPathMarkerSwitch->value().cycle();
}

void PanelHUD::onCycleScalesSwitch() {
	b_ScalesSwitch->value().cycle();
}

void PanelHUD::onCycleVelocitySwitch() {
	b_VelocitySwitch->value().cycle();
}

void PanelHUD::onCycleAltitudeSwitch() {
	b_AltitudeSwitch->value().cycle();
}

const Enumeration PanelHUD::DataSwitchSettings("OFF DATA");
const Enumeration PanelHUD::FlightPathMarkerSettings("OFF FPM ATT/FPM");
const Enumeration PanelHUD::ScalesSwitchSettings("OFF VAH VV/VAH");
const Enumeration PanelHUD::VelocitySwitchSettings("GND TAS CAS");
const Enumeration PanelHUD::AltitudeSwitchSettings("AUTO BARO RADAR");

CSP_NAMESPACE_END

