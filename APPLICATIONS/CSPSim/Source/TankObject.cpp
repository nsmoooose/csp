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


#include "SimObject.h"
#include "TankObject.h"
#include "BaseController.h"


using namespace std;

SIMDATA_REGISTER_INTERFACE(TankObject)


TankObject::TankObject(): DynamicObject()
{
//	m_iObjectType = TANK_OBJECT_TYPE;
	m_sObjectName = "TANK";
	m_bOnGround = true;

	movement_state = 1;
	gun_angle = 45.0f;
	max_viewing_range = 2000.0;
	max_viewing_angle = 60.0;
	max_firing_range = 1000.0;
	forward_speed = 5.0;
	backward_speed = 2.0;
	turn_speed = 0.15;
}

TankObject::~TankObject()
{

}

void TankObject::initialize()
{
	// the controller interface needs to be carefully rethought...
	//if (m_pController) {
	//	m_pController->initialize();
	//}
}


void TankObject::dump()
{
	cout << "ID: " << m_iObjectID
	     << ", TYPE: " << m_iObjectType
	     << ", ARMY: " << m_Army
	     << ", GLOPOSX: " << m_GlobalPosition.x
	     << ", GLOPOSY: " << m_GlobalPosition.y
	     << ", GLOPOSZ: " << m_GlobalPosition.z
	     << ", DIRX: " << m_Direction.x
	     << ", DIRY: " << m_Direction.y
	     << ", DIRZ: " << m_Direction.z
	     << ", VELX: " << m_LinearVelocity.x
	     << ", VELY: " << m_LinearVelocity.y
	     << ", VELZ: " << m_LinearVelocity.z
	     << ", GUNANGLE: " << gun_angle
	     << endl;      
}

void TankObject::onUpdate(double dt)
{
	DynamicObject::onUpdate(dt);
}

void TankObject::doMovement(double dt)
{
	DynamicObject::doMovement(dt);

	m_Direction.Normalize(); 
	m_LinearVelocity = m_Speed * m_Direction;
	m_LocalPosition += dt * m_LinearVelocity;
}

unsigned int TankObject::onRender()
{
//	updateScene();
	return 0;
}

int TankObject::updateScene()
{
	DynamicObject::updateScene();
	return 0;
}

