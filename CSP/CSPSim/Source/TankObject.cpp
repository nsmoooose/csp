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


#include "TankObject.h"


using namespace std;

SIMDATA_REGISTER_INTERFACE(TankObject)


TankObject::TankObject(): DynamicObject(TYPE_MUD_UNIT)
{
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

