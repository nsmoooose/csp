// Combat Simulator Project - FlightSim Demo
// Copyright (C) 2002, 2004 The Combat Simulator Project
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
 * @brief Provides an interface for objects that can handle input events.
 *
 * Subclasses of InputInterface can handle keyboard, mouse, and joystick
 * events.  The raw events are typically processed by the VirtualHID
 * layer, and converted into MapEvents.  The input interface binds
 * handler methods to specific event types, and dispatches incoming
 * events to the appropriate handler.  Input interfaces are inherited,
 * so events may be handled anywhere in the class hierarchy of a given
 * instance.  The event dispatch method is virtual, allowing subclasses
 * to define special event handling routines (such as dispatch to
 * member instances that implement InputInterface).
 *
 **/

#ifndef __INPUTINTERFACE_H__
#define __INPUTINTERFACE_H__

#include <map>
#include <string>
#include <cassert>
#include <sigc++/sigc++.h>

#include <SDL/SDL_events.h>
#include <SimData/HashUtility.h>

#include "MapEvent.h"


// forward declaration
class InputInterface;


typedef sigc::slot<void, int, int> ActionEventSlot;
typedef sigc::signal<void, int, int> ActionEventSignal;
typedef sigc::slot<void, int, int, int, int> MotionEventSlot;
typedef sigc::signal<void, int, int, int, int> MotionEventSignal;
typedef sigc::slot<void, double> AxisEventSlot;
typedef sigc::signal<void, double> AxisEventSignal;


/** Abstract adapter for action event handlers (internal)
 */
struct ActionAdapter {
	virtual void call(InputInterface *, int, int)=0;
};

/** Abstract adapter for motion event handlers (internal)
 */
struct MotionAdapter {
	virtual void call(InputInterface *, int, int, int, int)=0;
};

/** Abstract adapter for axis event handlers (internal)
 */
struct AxisAdapter {
	virtual void call(InputInterface *, double)=0;
};

/** Adapter for calling a class method action event handler. (internal)
 *
 *  Binds a method to be called when an action event is dispatched
 *  to an object.  The method takes no arguments and returns void.
 */
template <class TARGET_CLASS>
class ActionAdapterT: public ActionAdapter {
	void (TARGET_CLASS::*m_Handler)();
public:
	ActionAdapterT(void (TARGET_CLASS::*method)()): m_Handler(method) { }
	virtual void call(InputInterface *obj, int, int) {
		TARGET_CLASS *target = dynamic_cast<TARGET_CLASS*>(obj);
		if (target) (target->*m_Handler)();
	}
};

/** Adapter for calling a class method click event handler. (internal)
 *
 *  Binds a method to be called when a click event is dispatched
 *  to an object.  The method takes two integer arguments that are
 *  the x and y mouse coordinates when the click occured, and returns
 *  void.
 */
template <class TARGET_CLASS>
class ClickAdapterT: public ActionAdapter {
	void (TARGET_CLASS::*m_Handler)(int, int);
public:
	ClickAdapterT(void (TARGET_CLASS::*method)(int, int)): m_Handler(method) { }
	virtual void call(InputInterface *obj, int x, int y) {
		TARGET_CLASS *target = dynamic_cast<TARGET_CLASS*>(obj);
		if (target) (target->*m_Handler)(x, y);
	}
};

/** Adapter for calling a class method mouse-motion handler. (internal)
 *
 *  Binds a method to be called when a mouse motion event is dispatched
 *  to an object.  The method takes four integer arguments: the new x
 *  and y coordinates and the relative motion in x and y.  The method
 *  returns void.
 */
template <class TARGET_CLASS>
class MotionAdapterT: public MotionAdapter {
	void (TARGET_CLASS::*m_Handler)(int, int, int, int);
public:
	MotionAdapterT(void (TARGET_CLASS::*method)(int, int, int, int)): m_Handler(method) { }
	virtual void call(InputInterface *obj, int x, int y, int dx, int dy) {
		TARGET_CLASS *target = dynamic_cast<TARGET_CLASS*>(obj);
		if (target) (target->*m_Handler)(x, y, dx, dy);
	}
};

/** Adapter for calling a class method axis handler. (internal)
 *
 *  Binds a method to be called when a axis event (e.g. joystick motion)
 *  is dispatched to an object.  The method takes a floating point argument
 *  that represents the position of the axis (ranging from 0 to 1), and
 *  returns void.
 */
template <class TARGET_CLASS>
class AxisAdapterT: public AxisAdapter {
	void (TARGET_CLASS::*m_Handler)(double);
public:
	AxisAdapterT(void (TARGET_CLASS::*method)(double)): m_Handler(method) { }
	virtual void call(InputInterface *obj, double x) {
		TARGET_CLASS *target = dynamic_cast<TARGET_CLASS*>(obj);
		if (target) (target->*m_Handler)(x);
	}
};


/** A dispatch mapping that is instance specific and constructed at runtime (as
 *  opposed to InputInterfaceDispatch which is a static class property).  This
 *  dispatcher uses arbitrary sigc callbacks, so the handlers do not have to be
 *  InputInterface subclass methods.
 */
class RuntimeDispatch {
public:
	bool onMapEvent(MapEvent const &);
	void bindAction(std::string const &id, ActionEventSlot const &callback);
	void bindMotion(std::string const &id, MotionEventSlot const &callback);
	void bindAxis(std::string const &id, AxisEventSlot const &callback);
private:
	typedef std::map<std::string, ActionEventSignal> ActionCallbacks;
	typedef std::map<std::string, MotionEventSignal> MotionCallbacks;
	typedef std::map<std::string, AxisEventSignal> AxisCallbacks;
	ActionCallbacks m_ActionCallbacks;
	MotionCallbacks m_MotionCallbacks;
	AxisCallbacks m_AxisCallbacks;
};


/** Mappings for dispatching input events to handlers defined in InputInterface
 *  subclasses.  (internal)   This mapping is class static, such that it is
 *  defined at compile time for a class using the DECLARE_INPUT_INTERFACE
 *  macros below.  Static dispatch maps conserve space since they are shared
 *  by all instances of an InputInterface subclass.  InputInterface instances
 *  may also have dynamic dispatch maps if needed.
 */
class InputInterfaceDispatch {
private:
	typedef HASH_MAPS<const char *, ActionAdapter*, HASH<const char *>, simdata::eqstr>::Type ActionMap;
	typedef HASH_MAPS<const char *, MotionAdapter*, HASH<const char *>, simdata::eqstr>::Type MotionMap;
	typedef HASH_MAPS<const char *, AxisAdapter*, HASH<const char *>, simdata::eqstr>::Type AxisMap;

	// we support three different types of events, each with a different handler signature.
	// the maps are from string event identifiers to the corresponding handler adapters.  the
	// adapters are allocated during construction, and freed when this instance is destroyed.
	ActionMap m_Actions;
	MotionMap m_Motions;
	AxisMap m_Axes;

public:

	/** Destructor; frees all handler adapters.
	 */
	~InputInterfaceDispatch();

	/** Construct a new instance, initializing it with a static callback function.
	 *
	 *  Construction is handled automatically by the DECLARE_INPUT_INTERFACE macro;
	 *  you should never need to construct an InputInterfaceDispatch instance
	 *  directly.
	 */
	InputInterfaceDispatch(void (&bind)(InputInterfaceDispatch &)) {
		bind(*this);
	}

	/** Bind an action event handler to an event identifier.
	 *
	 *  This method is called automatically by the BIND_ACTION macro; you should
	 *  never need to call it directly.
	 */
	void bindAction(const char *id, ActionAdapter *adapter) {
		assert(adapter != 0);
		m_Actions[id] = adapter;
	}

	/** Bind a mouse motion handler to an event identifier.
	 *
	 *  This method is called automatically by the BIND_MOTION macro; you should
	 *  never need to call it directly.
	 */
	void bindMotion(const char *id, MotionAdapter *adapter) {
		assert(adapter != 0);
		m_Motions[id] = adapter;
	}

	/** Bind a axis event handler to an event identifier.
	 *
	 *  This method is called automatically by the BIND_AXIS macro; you should
	 *  never need to call it directly.
	 */
	void bindAxis(const char *id, AxisAdapter *adapter) {
		assert(adapter != 0);
		m_Axes[id] = adapter;
	}

	/** Execute an action event handler for the specified object.
	 *
	 *  @param id the event identifier string.
	 *  @param obj the object that received the event.
	 *  @param x the x coordinate of the mouse at the time of the event.
	 *  @param y the y coordinate of the mouse at the time of the event.
	 *  @return true if the event was dispatched to a handler; false otherwise.
	 */
	bool callAction(std::string const &id, InputInterface *obj, int x, int y) const {
		ActionMap::const_iterator iter = m_Actions.find(id.c_str());
		if (iter == m_Actions.end()) return false;
		iter->second->call(obj, x, y);
		return true;
	}

	/** Execute a mouse motion event handler for the specified object.
	 *
	 *  @param id the event identifier string.
	 *  @param obj the object that received the event.
	 *  @param x the new x coordinate of the mouse.
	 *  @param y the new y coordinate of the mouse.
	 *  @param dx the relative motion of the mouse on the x-axis.
	 *  @param dy the relative motion of the mouse on the y-axis.
	 *  @return true if the event was dispatched to a handler; false otherwise.
	 */
	bool callMotion(std::string const &id, InputInterface *obj, int x, int y, int dx, int dy) const {
		MotionMap::const_iterator iter = m_Motions.find(id.c_str());
		if (iter == m_Motions.end()) return false;
		iter->second->call(obj, x, y, dx, dy);
		return true;
	}

	/** Execute an axis event handler for the specified object.
	 *
	 *  @param id the event identifier string.
	 *  @param obj the object that received the event.
	 *  @param x the new position of the axis [0.0, 1.0]
	 *  @return true if the event was dispatched to a handler; false otherwise.
	 */
	bool callAxis(std::string const &id, InputInterface *obj, double x) const {
		AxisMap::const_iterator iter = m_Axes.find(id.c_str());
		if (iter == m_Axes.end()) return 0;
		iter->second->call(obj, x);
		return true;
	}

};

/** Interface for classes that can handle input events.
 *
 *  InputInterface subclasses can define class-specific handlers using the
 *  various BIND macros.  These bindings map events to methods within the
 *  InputInterface subclass, and do not incur any per-instance runtime
 *  storage costs.  InputInterface also supports dynamic event handlers
 *  that are created at runtime.  There is a per-instance storage cost for
 *  dynamic handlers.  Dynamic handlers override static handlers for the
 *  same event id.
 *
 *  The following is a minimal example of a class that defines handlers
 *  for a few events.  More elaborate behavior can be achieved by overriding
 *  some of the virtual methods in this interface.  The definition of the
 *  event binding relies heavily on macros defined in this header.
 *
 *  @code
 *
 *  // shape.h
 *
 *  class Shape: public InputInterface {
 *
 *    // bind event ids to handlers in this class
 *    DEFINE_INPUT_INTERFACE(Shape, InputInterface)
 *      BIND_ACTION("DRAW", drawShape)
 *      BIND_MOTION("MOVE", moveShape)
 *    END_INPUT_INTERFACE
 *
 *   public:
 *
 *    // event handlers
 *    void drawShape();
 *    void moveShape(int x, int y, int dx, int dy);
 *  };
 *
 *  class Arrow: public Shape {
 *
 *    // bind event ids to handlers in this class;  we also inherit all
 *    // handlers from Shape.
 *    DEFINE_INPUT_INTERFACE(Circle, Shape)
 *      BIND_CLICK("POINT_TO", point)
 *      BIND_AXIS("ADJUST_LENGTH", adjustLength)
 *      BIND_ACTION("TOGGLE_HEAD", toggleHead)
 *    END_INPUT_INTERFACE
 *
 *   public:
 *
 *    // event handlers
 *    void pointTo(int x, int y);
 *    void adjustLength(double x);
 *    void toggleHead();
 *  };
 *
 *  // shape.cc
 *
 *  #include "shape.h"
 *
 *  DECLARE_INPUT_INTERFACE(Shape)
 *  DECLARE_INPUT_INTERFACE(Circle)
 *
 *  @endcode
 *
 *  For more information on processing raw input events and passing them to
 *  InputInterface subclasses, see VirtualHID in HID.h.
 */
class InputInterface {
public:
	InputInterface();
	virtual ~InputInterface();

	/** These methods are called by VirtualHID to allow the InputInterface to access
	 *  the raw input events.  The default implementation returns false, which causes
	 *  VirtualHID to continue processing the events and then call onMapEvent().
	 */
	///{
	virtual bool onKey(SDL_KeyboardEvent const &) { return false; }
	virtual bool onJoystickButton(SDL_JoyButtonEvent const &) { return false; }
	virtual bool onJoystickAxisMotion(SDL_JoyAxisEvent const &) { return false; }
	virtual bool onMouseMove(SDL_MouseMotionEvent const &) { return false; }
	virtual bool onMouseButton(SDL_MouseButtonEvent const &) { return false; }
	///}

	/** Main entry point for dispatching input events.
	 *
	 *  This method is usually called by internally by VirtualHID.  Create a
	 *  VirtualHID instance and call bindObject to attach an InputInterface that
	 *  will receive processed input events.  See VirtualHID for more details.
	 */
	virtual bool onMapEvent(MapEvent const &);

	/** Bind a handler to an action event at runtime.  Using runtime
	 *  handlers (as opposed to the class handlers set by the BIND
	 *  macros) incurs storage costs per instance.
	 */
	void bindActionEvent(std::string const &id, ActionEventSlot const &slot) {
		runtimeDispatch()->bindAction(id, slot);
	}

	/** Bind a handler to an axis event at runtime.  Using runtime
	 *  handlers (as opposed to the class handlers set by the BIND
	 *  macros) incurs storage costs per instance.
	 */
	void bindAxisEvent(std::string const &id, AxisEventSlot const &slot) {
		runtimeDispatch()->bindAxis(id, slot);
	}

	/** Bind a handler to a motion event at runtime.  Using runtime
	 *  handlers (as opposed to the class handlers set by the BIND
	 *  macros) incurs storage costs per instance.
	 */
	void bindMotionEvent(std::string const &id, MotionEventSlot const &slot) {
		runtimeDispatch()->bindMotion(id, slot);
	}

protected:

	/** Internal */
	virtual InputInterfaceDispatch* _getInputInterfaceDispatch() { return 0; }

	/** Internal */
	static void _bindInputInterfaceDispatch(InputInterfaceDispatch &) {}

private:
	bool onCommand(std::string const &id, int x, int y);
	bool onAxis(std::string const &id, double value);
	bool onMotion(std::string const &id, int x, int y, int dx, int dy);

	RuntimeDispatch *runtimeDispatch() {
		if (!m_RuntimeDispatch) m_RuntimeDispatch = new RuntimeDispatch;
		return m_RuntimeDispatch;
	}
	RuntimeDispatch *m_RuntimeDispatch;
};


/** Opening statement for defining a mapping from input event identifiers to
 *  event handlers.  Should be placed inside the class declaration of each
 *  InputInterface subclass.  It should be followed by any number of BIND_*
 *  macros, and finally the END_INPUT_INTERFACE macro.  Note that the
 *  access mode (public, protected, private) should be explicitly reset after
 *  this block of macros.
 *
 *  @param this_class The current class (not currently used, but specifying
 *                    both the current class and the base class is probably
 *                    less error prone.
 *  @param base_class The immediate base class than is a subclass of
 *                    InputInterface.  All binding defined in the base class
 *                    (and it's base classes) will be inherited by the current
 *                    class.
 */
#define DECLARE_INPUT_INTERFACE(this_class, base_class) \
private: \
	static InputInterfaceDispatch _InputInterfaceDispatch; \
protected: \
	virtual InputInterfaceDispatch *_getInputInterfaceDispatch() { return &_InputInterfaceDispatch; } \
	static void _bindInputInterfaceDispatch(InputInterfaceDispatch &_map) { \
		typedef this_class TARGET; \
		base_class::_bindInputInterfaceDispatch(_map);

/** Close an input interface declaration block.
 */
#define END_INPUT_INTERFACE \
	}

/** Define an input interface.
 *
 *  Place this macro in the source file for each InputInterface subclass.
 *  It defines and initializes the static dispatch map declared in the
 *  DECLARE_INPUT_INTERFACE block.
 */
#define DEFINE_INPUT_INTERFACE(interface_class) \
	InputInterfaceDispatch interface_class::_InputInterfaceDispatch(interface_class::_bindInputInterfaceDispatch);

/** Bind an action handler to an input event identifier string.
 *
 *  @param id the input event identifier string.
 *  @param method a method of the current class taking no parameters
 *    and returning void.
 */
#define BIND_ACTION(id, method) \
		_map.bindAction(id, new ActionAdapterT<TARGET>(&TARGET::method));

/** Bind a click handler to an input event identifier string.
 *
 *  @param id the input event identifier string.
 *  @param method a method of the current class taking two integer parameters
 *    (the x-y coordinates of the mouse) and returning void.
 */
#define BIND_CLICK(id, method) \
		_map.bindAction(id, new ClickAdapterT<TARGET>(&TARGET::method));

/** Bind an axis handler to an input event identifier string.
 *
 *  @param id the input event identifier string.
 *  @param method a method of the current class taking a double parameter
 *    ([0.0, 1.0] for the position of the axis) and returning void.
 */
#define BIND_AXIS(id, method) \
		_map.bindAxis(id, new AxisAdapterT<TARGET>(&TARGET::method));

/** Bind a mouse motion handler to an input event identifier string.
 *
 *  @param id the input event identifier string.
 *  @param method a method of the current class taking four integer
 *    parameters (x, y, delta_x, delta_y) and returning void.
 */
#define BIND_MOTION(id, method) \
	_map.bindMotion(id, new MotionAdapterT<TARGET>(&TARGET::method));


#endif // __INPUTINTERFACE_H__

