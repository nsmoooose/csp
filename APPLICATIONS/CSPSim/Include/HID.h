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
 * 
 */

#ifndef __HID_H__
#define __HID_H__

#include "SDL_events.h"
#include "SDL_keysym.h"
#include "SDL_keyboard.h"

#include <string>
#include <vector>

#include <SimData/HashUtility.h>

#include "EventMapping.h"


class InputInterface;


/**
 * class HID - Base class for Human Interface Device mapping and scripting.
 *
 */
class HID
{

public:

	HID() {}
	virtual ~HID() {}

	virtual bool OnKey(SDL_KeyboardEvent const &) = 0;
	virtual bool OnJoystickButton(SDL_JoyButtonEvent const &) = 0;
	virtual bool OnJoystickAxisMotion(SDL_JoyAxisEvent const &) = 0;
	virtual bool OnMouseMove(SDL_MouseMotionEvent const &) = 0;
	virtual bool OnMouseButton(SDL_MouseButtonEvent const &) = 0;
	/*
	virtual void OnJoystickHatMotion(int joynum, int hat, int val) = 0;
	*/

	virtual void OnUpdate(double) = 0;
	virtual void bindObject(InputInterface *) = 0;

	virtual bool OnEvent(SDL_Event &event);
};




class VirtualHID: public HID
{
public:
	VirtualHID();
	virtual ~VirtualHID();

	virtual void setMapping(EventMapping const *map);
	virtual void bindObject(InputInterface *object);
	
	virtual bool OnKey(SDL_KeyboardEvent const &event);
	virtual bool OnJoystickButton(SDL_JoyButtonEvent const &event);
	virtual bool OnJoystickAxisMotion(SDL_JoyAxisEvent const &event);
	virtual bool OnMouseMove(SDL_MouseMotionEvent const &event);
	virtual bool OnMouseButton(SDL_MouseButtonEvent const &event);
	virtual void OnUpdate(double dt);


protected:
	virtual void setScript(EventMapping::Script const *s, int x = -1, int y = -1);
	virtual void setVirtualMode(int mode);
	virtual void setJoystickModifier(int jmod);

	int m_VirtualMode;
	bool m_JoystickModifier;
	const EventMapping *m_Map;

	InputInterface *m_Object;

	EventMapping::Script const *m_ActiveScript;
	EventMapping::Script::const_iterator m_ScriptAction;
	double m_ScriptTime;

	int m_MouseEventX;
	int m_MouseEventY;
};





/*
  Right now identifiers like THROTTLE and EXTEND_GEAR are class static enums.
  In principle they are only needed for the bindEvents method.
  
  External tools need to access these values as STRING:INT pairs, and I 
  don't see a safe/easy way to do this right now.

  Moreover, when a class is extended the resulting enums must be set higher
  than the parent enum range (which may change!).  This is rather inelegant.
  It would be better if we could do something like string concatination to
  grow the list of ID's in derived classes.

  Spacing the ranges in derived classes probably a good thing, so that
  the base class id list can grow without breaking other ids.  Once defined
  an id would ideally never change, since such changes require rebuilding of 
  all binary map images.
*/


/*
 map scripts want:

 	define	RETRACT_GEAR    0
	define  EXTEND_GEAR     1
	define  TOGGLE_GEAR     2

	which has to be generated somehow.   the id numbers only have to
	be unique within a given class inheritance chain.

 internally we have enums, #defines, const int, etc. to work with.  it
 would be ideal to keep these values within the classes they refer to
 rather than a namespace so that changes can be made locally.  this
 makes generation of the map defines much harder though.  for namespaces
 we'd have something like:

 namespace map_AircraftObject {
 	const int RETRACT_GEAR = 0;
 	const int EXTEND_GEAR = 1;
 }

 which doesn't really solve the problem, introduces unnecessary variables,
 and separates the constants from their associated class.

 considering that the script namespace is flat, we are "guaranteed" that
 the ids are unique.  so we can put them in a separate file and #include
 them directly into the class?  given a suitable file extension it would
 be easy to scan them all and extract the necessary define commands.

 this works pretty well:

 class A {
 public:
 	enum {
		#include "A.map"
	};
	...
 };

 with A.map in the form:

 X = 2000,
 Y,
 Z,

 which can easily be converted to

 define X 2000
 define Y 2001
 define Z 2002

 the converter should check for duplicate id names with different id values
 and spew copious warnings if any are found.

 probably commenting should be enforced by the converter, so you would have
 to write:

 X = 2000, // Do X blah blah
 Y,        // Do Y blah blah
 Z,        // Do something else

 which would then translate to

 define X 2000  # Do X blah blah
 define Y 2001  # Do Y blah blah
 define Z 2002  # Do something else

 the filename will match the classname so it should be fairly clear what
 each id is used for.

 the enum lists should always end with a comma so they can be concatinated
 with multiple #include directives.
   
*/
  

/*
map x36.axis0 to THROTTLE
map x36.axis4 to HUDSCALE in NAV

band HUDSCALE 

map band.HUDSCALE 0.0 script
map band.HUDSCALE 0.1 script
map band.HUDSCALE 0.5 script

*/

#endif // __HID_H__

