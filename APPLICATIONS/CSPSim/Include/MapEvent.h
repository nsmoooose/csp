// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002, 2003 The Combat Simulator Project
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
 * @file MapEvent.h
 *
 **/

#ifndef __MAPEVENT_H__
#define __MAPEVENT_H__

#include <string>


struct MapEvent {
	struct ID_CommandEvent {
		int x, y;
	};

	struct ID_AxisEvent {
		double value;
	};

	struct ID_MotionEvent {
		int x, y, dx, dy;
	};

	enum { 
		NO_EVENT,
		ID_COMMAND_EVENT, 
		ID_AXIS_EVENT, 
		ID_MOTION_EVENT 
	};
	std::string id;
	unsigned int type;
	union {
		ID_CommandEvent id_command;
		ID_AxisEvent id_axis;
		ID_MotionEvent id_motion;
	};

	static MapEvent CommandEvent(std::string const &id_, int x_, int y_) {
		MapEvent e;
		e.type = ID_COMMAND_EVENT;
		e.id = id_;
		e.id_command.x = x_;
		e.id_command.y = y_;
		return e;
	}

	static MapEvent AxisEvent(std::string const &id_, double value_) {
		MapEvent e;
		e.type = ID_AXIS_EVENT;
		e.id = id_;
		e.id_axis.value = value_;
		return e;
	}

	static MapEvent MotionEvent(std::string const &id_, int x_, int y_, int dx_, int dy_) {
		MapEvent e;
		e.type = ID_MOTION_EVENT;
		e.id = id_;
		e.id_motion.x = x_;
		e.id_motion.y = y_;
		e.id_motion.dx = dx_;
		e.id_motion.dy = dy_;
		return e;
	}
};


#endif // __MAPEVENT_H__

