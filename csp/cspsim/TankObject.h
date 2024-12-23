#pragma once
// Combat Simulator Project
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

#include <csp/cspsim/DynamicObject.h>

namespace csp {

/** Outline of a tank object class -- not functional.
 */
class TankObject : public DynamicObject {
public:
	CSP_DECLARE_OBJECT(TankObject)

	TankObject();
	virtual ~TankObject();
	virtual void getStats(std::vector<std::string> &/*stats*/) const { }

	double getMaxViewingRange() { return max_viewing_range; }
	double getMaxViewingAngle() { return max_viewing_angle; }
	double getMaxFiringRange() { return max_firing_range; }

	/**
	 * movement states for the tank object
	 * 0 - stopped
	 * 1 - forward
	 * 2 - backward
	 * 3 - turning right
	 * 4 - turning left
	 */
	void setMovementState(unsigned int state) { movement_state = state; }

	virtual void onRender();

protected:
	virtual double onUpdate(double);
	virtual void registerChannels(Bus *bus);
	virtual void bindChannels(Bus *bus);

	double gun_angle;

	double max_viewing_range;
	double max_viewing_angle;
	double max_firing_range;

	double forward_speed;
	double backward_speed;
	double turn_speed;

	unsigned movement_state;
	char bits;

	DataChannel<double>::RefT b_Heading;
};

} // namespace csp
