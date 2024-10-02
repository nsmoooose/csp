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
 * @file HID.h
 *
 **/

/**
 * @TODO add joystick hat support
 * @todo map loading / parsing
 * @todo map caching
 * @todo automatic map swapping based on player object type
 * @todo rotary banding
 * @todo axis calibration, detents, and non-linear corrections
 * @todo finish the definition of the map scripting language
 * @todo complete the python tool to convert map scripts to binary
 * @todo add xml support to the python script tool
 * @todo add TrackIR support
 * @todo Force Feedback support.
 */

#include <csp/cspsim/input/EventMapping.h>

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/Ref.h>

#include <string>
#include <vector>

namespace csp {
namespace input {

class InputInterface;


/**
 * class HID - Base class for Human Interface Device mapping and scripting.
 *
 * This class provides an interface for handling various input events,
 * and implements an event dispatch routine.
 */
class HID: public Referenced
{
public:

	HID() {}

	/**
	 * Keyboard event handler
	 */
	virtual bool onEvent(RawEvent::Keyboard const &) = 0;

	/**
	 * Joystick button handler
	 */
	virtual bool onEvent(RawEvent::JoystickButton const &) = 0;
	
	/**
	 * Joystick axis handler
	 */
	virtual bool onEvent(RawEvent::JoystickAxis const &) = 0;
	
	/**
	 * Mouse movement handler
	 */
	virtual bool onEvent(RawEvent::MouseMotion const &) = 0;
	
	/**
	 * Mouse button handler
	 */
	virtual bool onEvent(RawEvent::MouseButton const &) = 0;

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
	 * Translate events.
	 *
	 * Call this once for each event.  Currently, the modifier state of
	 * key releases is translated to match the state when the key was
	 * pressed.
	 */
	static void translate(RawEvent::Keyboard &event);

protected:
	virtual ~HID() {}
};



/**
 * class VirtualHID - Simulates a composite, programmable human interface device.
 *
 * VirtualHID instances use event mappings to translate raw input events into
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
	
	virtual bool onEvent(RawEvent::Keyboard const &event);
	virtual bool onEvent(RawEvent::JoystickButton const &event);
	virtual bool onEvent(RawEvent::JoystickAxis const &event);
	virtual bool onEvent(RawEvent::MouseMotion const &event);
	virtual bool onEvent(RawEvent::MouseButton const &event);
	virtual void onUpdate(double dt);


protected:
	virtual ~VirtualHID();
	virtual void setScript(EventMapping::Script const *s, float x = -1, float y = -1, bool drag = false);
	virtual void setVirtualMode(int mode);
	virtual void setJoystickModifier(int jmod);

	int m_VirtualMode;
	bool m_JoystickModifier;
	Ref<const EventMapping> m_Map;

	InputInterface *m_Object;

	EventMapping::Script const *m_ActiveScript;
	double m_ScriptTime;

	float m_MouseEventX;
	float m_MouseEventY;
	bool m_MouseEventDrag;
};

} // namespace input
} // namespace csp
