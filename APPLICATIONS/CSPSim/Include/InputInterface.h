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
 * @file InputInterface.h
 *
 **/

#ifndef __INPUTINTERFACE_H__
#define __INPUTINTERFACE_H__

#include <string>

#include <SDL/SDL_events.h>

#include <SimData/HashUtility.h>

#include "HID.h"

//class VirtualHID;

using simdata::eqstr;

class InputInterface {
public:
	typedef void (*ActionAdapter)(InputInterface *, int, int);
	typedef void (*MotionAdapter)(InputInterface *, int, int, int, int);
	typedef void (*AxisAdapter)(InputInterface *, double);

	InputInterface() {
		m_Actions.clear();
		m_Motions.clear();
		m_Axes.clear();
	}

	virtual ~InputInterface() {}
	virtual bool OnKey(SDL_KeyboardEvent const &) { return false; }
	virtual bool OnJoystickButton(SDL_JoyButtonEvent const &) { return false; }
	virtual bool OnJoystickAxisMotion(SDL_JoyAxisEvent const &) { return false; }
	virtual bool OnMouseMove(SDL_MouseMotionEvent const &) { return false; }
	virtual bool OnMouseButton(SDL_MouseButtonEvent const &) { return false; }

	virtual bool OnCommand(std::string const &id, int x, int y);
	virtual bool OnAxis(std::string const &id, double value);
	virtual bool OnMotion(std::string const &id, int x, int y, int dx, int dy);

protected:
	typedef HASH_MAPS<const char *, ActionAdapter, HASH<const char *>, eqstr>::Type ActionMap;
	typedef HASH_MAPS<const char *, MotionAdapter, HASH<const char *>, eqstr>::Type MotionMap;
	typedef HASH_MAPS<const char *, AxisAdapter, HASH<const char *>, eqstr>::Type AxisMap;
	
	ActionMap m_Actions;
	MotionMap m_Motions;
	AxisMap m_Axes;
	std::string m_LastMotionID;
	MotionAdapter m_LastMotionAdapter;
};


#define ACTION_INTERFACE(obj_class, method) \
virtual void method(); \
static void on##method(InputInterface* obj, int, int) {\
	obj_class* target = dynamic_cast<obj_class*>(obj);\
	assert(target); \
	target->method();\
}

#define CLICK_INTERFACE(obj_class, method) \
virtual void method(int, int); \
static void on##method(InputInterface* obj, int x, int y) { \
	obj_class* target = dynamic_cast<obj_class*>(obj);\
	assert(target); \
	target->method(x, y);\
}

#define MOTION_INTERFACE(obj_class, method) \
virtual void method(int, int, int, int); \
static void on##method(InputInterface* obj, int x, int y, int dx, int dy) {\
	obj_class* target = dynamic_cast<obj_class*>(obj);\
	assert(target); \
	target->method(x, y, dx, dy);\
}

#define AXIS_INTERFACE(obj_class, method) \
virtual void method(double); \
static void on##method(InputInterface* obj, double value) {\
	obj_class* target = dynamic_cast<obj_class*>(obj);\
	assert(target); \
	target->method(value);\
}

#define BIND_ACTION(id, method) \
	m_Actions[id] = &on##method;
	
#define BIND_AXIS(id, method) \
	m_Axes[id] = &on##method;

#define BIND_MOTION(id, method) \
	m_Motions[id] = &on##method;
	

#endif // __INPUTINTERFACE_H__

