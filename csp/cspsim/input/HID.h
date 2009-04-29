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
 * @file HID.h
 *
 **/

/**************************************************************
 * TODO  jan 2003
 *
 * add joystick hat support
 * map loading / parsing
 * map caching
 * automatic map swapping based on player object type
 * rotary banding
 * axis calibration, detents, and non-linear corrections
 * finish the definition of the map scripting language
 * complete the python tool to convert map scripts to binary
 * add xml support to the python script tool
 * introduce a new, internal event type with a translation layer
 *   for SDL events.  this will remove the tight binding with
 *   SDL and allow other input libraries to be used as well.
 *
 */

#ifndef __CSPSIM_HID_H__
#define __CSPSIM_HID_H__

#include <csp/cspsim/input/EventMapping.h>

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Ref.h>

#include <SDL/SDL_events.h>
#include <SDL/SDL_keysym.h>
#include <SDL/SDL_keyboard.h>

#include <string>
#include <vector>

namespace csp {
namespace input {

class InputInterface;


/**
 * class HID - Base class for Human Interface Device mapping and scripting.
 *
 * This class provides an interface for handling various SDL input events,
 * and implements an event dispatch routine.
 */
class HID: public Referenced
{
public:

	HID() {}

	/**
	 * SDL key event handler
	 */
	virtual bool onKey(SDL_KeyboardEvent const &) = 0;

	/**
	 * SDL joystick button handler
	 */
	virtual bool onJoystickButton(SDL_JoyButtonEvent const &) = 0;
	
	/**
	 * SDL joystick axis handler
	 */
	virtual bool onJoystickAxisMotion(SDL_JoyAxisEvent const &) = 0;
	
	/**
	 * SDL mouse movement handler
	 */
	virtual bool onMouseMove(SDL_MouseMotionEvent const &) = 0;
	
	/**
	 * SDL mouse button handler
	 */
	virtual bool onMouseButton(SDL_MouseButtonEvent const &) = 0;
	/*
	virtual void onJoystickHatMotion(int joynum, int hat, int val) = 0;
	*/

	/**
	 * Update routine to drive the active script(s).
	 */
	virtual void onUpdate(double) = 0;
	
	/**
	 * Bind to an object.
	 *
	 * The object must implement the InputInterface.  All mapped
	 * events are passed to this object (which may in turn selectively
	 * pass events on to its children).
	 */
	virtual void bindObject(InputInterface *) = 0;

	/**
	 * Primary event dispatch routine.
	 */
	virtual bool onEvent(SDL_Event &event);

	/**
	 * Translate events.
	 *
	 * Call this once for each event.  Currently, the modifier state of
	 * key releases is translated to match the state when the key was
	 * pressed.
	 */
	static void translate(SDL_Event &event);

protected:
	virtual ~HID() {}
};



/**
 * class VirtualHID - Simulates a composite, programmable human interface device.
 *
 * VirtualHID instances use event mappings to translate raw SDL input events into
 * commands that are passed to associated objects.  Each VirtualHID can handle
 * multiple real human interface devices (keyboard, mice, joysticks), and can be 
 * placed into one of several user-defined modes that alters the event mapping.  
 * Events such as button and key presses translate into scripts that are executed 
 * by the VirtualHID to generate a series of commands.  These commands, which are 
 * simple string identifiers, are passed to objects (implementing the InputInterface
 * interface) that understand and can respond to them.
 */
class VirtualHID: public HID
{
public:
	VirtualHID();

	virtual void setMapping(Ref<const EventMapping> map);
	virtual void bindObject(InputInterface *object);
	
	virtual bool onKey(SDL_KeyboardEvent const &event);
	virtual bool onJoystickButton(SDL_JoyButtonEvent const &event);
	virtual bool onJoystickAxisMotion(SDL_JoyAxisEvent const &event);
	virtual bool onMouseMove(SDL_MouseMotionEvent const &event);
	virtual bool onMouseButton(SDL_MouseButtonEvent const &event);
	virtual void onUpdate(double dt);


protected:
	virtual ~VirtualHID();
	virtual void setScript(EventMapping::Script const *s, int x = -1, int y = -1, bool drag = false);
	virtual void setVirtualMode(int mode);
	virtual void setJoystickModifier(int jmod);
	void updateMouseDrag(SDL_MouseMotionEvent const &event);
	bool updateMousePreDrag(SDL_MouseButtonEvent const &event);

	int m_VirtualMode;
	bool m_JoystickModifier;
	Ref<const EventMapping> m_Map;

	InputInterface *m_Object;

	EventMapping::Script const *m_ActiveScript;
	double m_ScriptTime;

	int m_MouseEventX;
	int m_MouseEventY;
	bool m_MouseEventDrag;

	int m_MouseDragStartX;
	int m_MouseDragStartY;
	bool m_MouseDrag;
	bool m_MousePreDrag;
};

} // namespace input
} // namespace csp

#endif // __CSPSIM_HID_H__

