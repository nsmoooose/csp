// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002 The Combat Simulator Project
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
 * @file TankObject.cpp
 *
 **/

#include "KineticsChannels.h"
#include "TankObject.h"
#include "ObjectModel.h"
#include "SystemsModel.h"

#include <csp/csplib/util/Log.h>
#include <csp/csplib/data/ObjectInterface.h>


using namespace std;

SIMDATA_XML_BEGIN(TankObject)
	SIMDATA_DEF("forward_speed", forward_speed, false)
	SIMDATA_DEF_BIT("test0", bits, 1, false)
	SIMDATA_DEF_BIT("test1", bits, 2, false)
	SIMDATA_DEF_BIT("test2", bits, 4, false)
SIMDATA_XML_END


TankObject::TankObject(): DynamicObject(TYPE_MUD_UNIT)
{
	CSP_LOG(OBJECT, DEBUG, "TankObject::TankObject() ...");
	b_Heading = DataChannel<double>::newLocal(bus::Kinetics::Heading, 0.0);

//	m_ObjectType = TANK_OBJECT_TYPE;
	m_ObjectName = "TANK";

	movement_state = 1;
	gun_angle = 45.0f;
	max_viewing_range = 2000.0;
	max_viewing_angle = 60.0;
	max_firing_range = 1000.0;
	forward_speed = 5.0;
	backward_speed = 2.0;
	turn_speed = 0.15;
	bits = 0;
}

TankObject::~TankObject()
{

}
void TankObject::registerChannels(Bus * bus) {
	CSP_LOG(OBJECT, DEBUG, "TankObject::registerChannels() ...");

	DynamicObject::registerChannels(bus);
//	if (bus->valid()) {
//		bus->registerChannel(b_Heading.get());
//	}
}

void TankObject::bindChannels(Bus*) {
	CSP_LOG(OBJECT, DEBUG, "TankObject::bindChannels() ...");
//	DynamicObject::bindChannels(bus);
}

double TankObject::onUpdate(double dt)
{
	return DynamicObject::onUpdate(dt);
}


void TankObject::onRender() {
	DynamicObject::onRender();
}

