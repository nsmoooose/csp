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
 * @file F16GearDyanimcs.cpp
 *
 **/


#include "F16Channels.h"
#include "F16GearDynamics.h"
#include "GearAnimation.h"

#include <SimData/Math.h>
#include <SimData/Conversions.h>

SIMDATA_REGISTER_INTERFACE(F16GearDynamics)

bool F16GearDynamics::allowGearUp() const {
	// TODO: DN LOCK REL button can override
	return !m_LeftMainLandingGear->getWOW();
}

void F16GearDynamics::GearUp() {
	if (allowGearUp()) {
		GearDynamics::GearUp();
		b_GearHandleUp->push(true);
	}
}

void F16GearDynamics::GearDown() {
	GearDynamics::GearDown();
	b_GearHandleUp->push(false);
}

void F16GearDynamics::registerChannels(Bus *bus) {
	GearDynamics::registerChannels(bus);
	b_WheelSpin = bus->registerLocalDataChannel<bool>(bus::F16::WheelSpin, false);
	b_GearHandleUp = bus->registerLocalPushChannel<bool>(bus::F16::GearHandleUp, false);
	GearDynamics::GearDown();  // sync with channel default
}

void F16GearDynamics::importChannels(Bus *bus) {
	GearDynamics::importChannels(bus);
}

void F16GearDynamics::preSimulationStep(double dt) {
	if (isGearExtendSelected()) {
		b_WheelSpin->value() = m_LeftMainLandingGear->getWheelSpeed() > simdata::convert::kts_mps(60.0);
	}
	GearDynamics::preSimulationStep(dt);
}

void F16GearDynamics::postCreate() {
	GearDynamics::postCreate();
	assert(m_Gear.size() == 3);
	m_NoseLandingGear = m_Gear[0];
	m_LeftMainLandingGear = m_Gear[1];
	m_RightMainLandingGear = m_Gear[2];
	assert(m_NoseLandingGear->getName() == "FrontGear");
	assert(m_LeftMainLandingGear->getName() == "LeftGear");
	assert(m_RightMainLandingGear->getName() == "RightGear");
}

