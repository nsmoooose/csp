// Combat Simulator Project
// Copyright (C) 2002, 2003, 2005 The Combat Simulator Project
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

#include <csp/lib/util/Namespace.h>

#include <string>

CSP_NAMESPACE


/** Event wrapper struct that can hold parameters for a command, axis, or mouse event.
 *  Also defines the event structs that are passed by to handler methods.  See
 *  InputInterface for details.
 */
struct MapEvent {

	// Command events cover keyboard, mouse button, and joystick button events.  Via
	// VirtualHID these input events trigger command scripts that can generate one
	// or more command events.  The event parameters correspond to the state at the
	// time the input event occurred, not the time at which the command event is
	// dispatched (e.g., a script may include delayed execution).
	struct CommandEvent {
		// mouse coordinates at the time the event occurred.
		int x, y;
		// true if the mouse has been moved with a button pressed immediately prior
		// to this event.
		bool drag;
	};

	// Command events can be connected to action or click handlers.  An action handler
	// takes no arguments.  A click handler takes a CommandEvent argument.  We define
	// a ClickEvent alias only to make the interface of click handlers more intuitive.
	typedef CommandEvent ClickEvent;

	// An axis event corresponds to motion of a joystick (or similar device) along one
	// axis.  The value parameter is normalized to [-1.0, 1.0].
	struct AxisEvent {
		double value;
	};

	// Motion events are generated by movement of the mouse or similar devices.
	struct MotionEvent {
		// coordinates and relative motion
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
		CommandEvent id_command;
		AxisEvent id_axis;
		MotionEvent id_motion;
	};

	// Factory to construct a new CommandEvent.
	static MapEvent MapCommandEvent(std::string const &id, int x, int y, bool drag) {
		MapEvent e;
		e.type = ID_COMMAND_EVENT;
		e.id = id;
		e.id_command.x = x;
		e.id_command.y = y;
		e.id_command.drag = drag;
		return e;
	}

	// Factory to construct a new AxisEvent.
	static MapEvent MapAxisEvent(std::string const &id, double value) {
		MapEvent e;
		e.type = ID_AXIS_EVENT;
		e.id = id;
		e.id_axis.value = value;
		return e;
	}

	// Factory to construct a new MotionEvent.
	static MapEvent MapMotionEvent(std::string const &id, int x, int y, int dx, int dy) {
		MapEvent e;
		e.type = ID_MOTION_EVENT;
		e.id = id;
		e.id_motion.x = x;
		e.id_motion.y = y;
		e.id_motion.dx = dx;
		e.id_motion.dy = dy;
		return e;
	}
};


CSP_NAMESPACE_END

#endif // __MAPEVENT_H__

