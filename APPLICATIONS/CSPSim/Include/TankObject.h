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
 * @file TankObject.h
 *
 **/

#ifndef __TANKOBJECT_H__
#define __TANKOBJECT_H__

#include "DynamicObject.h"

// movement states for the tank object
// 0 - stopped
// 1 - forward
// 2 - backward
// 3 - turning right
// 4 - turning left


/**
 * class TankObject
 *
 * @author unknown
 */
class TankObject : public DynamicObject
{
public:
	SIMDATA_OBJECT(TankObject, 0, 0);

	EXTEND_SIMDATA_XML_INTERFACE(TankObject, DynamicObject)
		SIMDATA_XML("forward_speed", TankObject::forward_speed, false)
	END_SIMDATA_XML_INTERFACE

	
public:

	TankObject();
	virtual ~TankObject();
	virtual void dump();
	virtual void onUpdate(double dt);
	virtual unsigned int onRender();

	virtual void initialize();
	double getMaxViewingRange() { return max_viewing_range; }
	double getMaxViewingAngle() { return max_viewing_angle; }
	double getMaxFiringRange() { return max_firing_range; }

	void setMovementState(unsigned int state) { movement_state = state; }


protected:

	void doMovement(double dt);

	double gun_angle;

	double max_viewing_range;
	double max_viewing_angle;
	double max_firing_range;

	double forward_speed;
	double backward_speed;
	double turn_speed;

	unsigned int movement_state;

};



#endif // __TANKOBJECT_H__

