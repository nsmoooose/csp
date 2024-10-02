#pragma once
// Combat Simulator Project
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

#include <csp/cspsim/Export.h>
#include <csp/cspsim/input/RawEvent.h>
#include <csp/cspsim/input/MapEvent.h>

#include <csp/csplib/util/Namespace.h>
#include <csp/csplib/util/HashUtility.h>

#include <sigc++/sigc++.h>

#include <map>
#include <string>
#include <cassert>


namespace csp {
namespace input {

// forward declaration
class InputInterface;

typedef sigc::slot<void> ActionEventSlot;
typedef sigc::slot<void, MapEvent::ClickEvent const &> ClickEventSlot;
typedef sigc::signal<void, MapEvent::CommandEvent const &> CommandEventSignal;
typedef sigc::slot<void, MapEvent::MotionEvent const &> MotionEventSlot;
typedef sigc::signal<void, MapEvent::MotionEvent const &> MotionEventSignal;
typedef sigc::slot<void, MapEvent::AxisEvent const &> AxisEventSlot;
typedef sigc::signal<void, MapEvent::AxisEvent const &> AxisEventSignal;


/** Abstract adapter for input event handlers (internal)
 */
class EventAdapter {
public:
	virtual ~EventAdapter() {}
	virtual void call(InputInterface *, MapEvent const &event) = 0;
};

/** Abstract adapter for command event handlers (internal)
 */
class CommandAdapter: public EventAdapter {
	virtual void call(InputInterface *, MapEvent::CommandEvent const &)=0;
public:
	virtual void call(InputInterface *obj, MapEvent const &event) {
		if (event.type == MapEvent::ID_COMMAND_EVENT) call(obj, event.id_command);
	}
};

/** Abstract adapter for motion event handlers (internal)
 */
class MotionAdapter: public EventAdapter {
	virtual void call(InputInterface *, MapEvent::MotionEvent const &)=0;
public:
	virtual void call(InputInterface *obj, MapEvent const &event) {
		if (event.type == MapEvent::ID_MOTION_EVENT) call(obj, event.id_motion);
	}
};

/** Abstract adapter for axis event handlers (internal)
 */
class AxisAdapter: public EventAdapter {
	virtual void call(InputInterface *, MapEvent::AxisEvent const &)=0;
public:
	virtual void call(InputInterface *obj, MapEvent const &event) {
		if (event.type == MapEvent::ID_AXIS_EVENT) call(obj, event.id_axis);
	}
};

/** Adapter for calling a class method action event handler. (internal)
 *
 *  Binds a method to be called when an action event is dispatched
 *  to an object.  The method takes no arguments and returns void.
 */
template <class TARGET_CLASS>
class ActionAdapterT: public CommandAdapter {
	void (TARGET_CLASS::*m_Handler)();
public:
	ActionAdapterT(void (TARGET_CLASS::*method)()): m_Handler(method) { }
	virtual void call(InputInterface *obj, MapEvent::CommandEvent const &) {
		TARGET_CLASS *target = dynamic_cast<TARGET_CLASS*>(obj);
		if (target) (target->*m_Handler)();
	}
};

/** Adapter for calling a class method click event handler. (internal)
 *
 *  Binds a method to be called when a click event is dispatched
 *  to an object.  The method takes two integer arguments that are
 *  the x and y mouse coordinates when the click occured and a bool
 *  that indicates if the mouse is being dragged.
 */
template <class TARGET_CLASS>
class ClickAdapterT: public CommandAdapter {
	void (TARGET_CLASS::*m_Handler)(MapEvent::ClickEvent const &);
public:
	ClickAdapterT(void (TARGET_CLASS::*method)(MapEvent::ClickEvent const &)): m_Handler(method) { }
	virtual void call(InputInterface *obj, MapEvent::CommandEvent const &event) {
		TARGET_CLASS *target = dynamic_cast<TARGET_CLASS*>(obj);
		if (target) (target->*m_Handler)(event);
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
	void (TARGET_CLASS::*m_Handler)(MapEvent::MotionEvent const &);
public:
	MotionAdapterT(void (TARGET_CLASS::*method)(MapEvent::MotionEvent const &)): m_Handler(method) { }
	virtual void call(InputInterface *obj, MapEvent::MotionEvent const &event) {
		TARGET_CLASS *target = dynamic_cast<TARGET_CLASS*>(obj);
		if (target) (target->*m_Handler)(event);
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
	void (TARGET_CLASS::*m_Handler)(MapEvent::AxisEvent const &);
public:
	AxisAdapterT(void (TARGET_CLASS::*method)(MapEvent::AxisEvent const &)): m_Handler(method) { }
	virtual void call(InputInterface *obj, MapEvent::AxisEvent const &event) {
		TARGET_CLASS *target = dynamic_cast<TARGET_CLASS*>(obj);
		if (target) (target->*m_Handler)(event);
	}
};


/** A dispatch mapping that is instance specific and constructed at runtime (as
 *  opposed to InputInterfaceDispatch which is a static class property).  This
 *  dispatcher uses arbitrary sigc callbacks, so the handlers do not have to be
 *  InputInterface subclass methods.
 */
class CSPSIM_EXPORT RuntimeDispatch {
public:
	bool onMapEvent(MapEvent const &);
	void bindClick(std::string const &id, ClickEventSlot const &callback);
	void bindAction(std::string const &id, ActionEventSlot const &callback);
	void bindMotion(std::string const &id, MotionEventSlot const &callback);
	void bindAxis(std::string const &id, AxisEventSlot const &callback);
private:
	typedef std::map<std::string, CommandEventSignal> CommandCallbacks;
	typedef std::map<std::string, MotionEventSignal> MotionCallbacks;
	typedef std::map<std::string, AxisEventSignal> AxisCallbacks;
	CommandCallbacks m_CommandCallbacks;
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
class CSPSIM_EXPORT InputInterfaceDispatch {
private:
	typedef HashMap<const char*, EventAdapter*>::Type EventHandlerMap;

	// a map from string event identifiers to the corresponding handler adapters.  the
	// adapters are allocated during construction, and freed when this instance is destroyed.
	EventHandlerMap m_EventHandlers;

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

	/** Bind an event handler to an event identifier.
	 *
	 *  This method is called automatically by the BIND_* macros; you should
	 *  never need to call it directly.
	 */
	void bindEventHandler(const char *id, EventAdapter *adapter) {
		assert(adapter != 0 && m_EventHandlers.find(id) == m_EventHandlers.end());
		m_EventHandlers[id] = adapter;
	}

	/** Execute an event handler for the specified object.
	 *
	 *  @param id the event identifier string.
	 *  @param obj the object that received the event.
	 *  @param event the input event.
	 *  @return true if the event was dispatched to a handler; false otherwise.
	 */
	bool callHandler(InputInterface *obj, MapEvent const &event) const {
		EventHandlerMap::const_iterator iter = m_EventHandlers.find(event.id.c_str());
		if (iter == m_EventHandlers.end()) return false;
		iter->second->call(obj, event);
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
 *    void pointTo(MapEvent::ClickEvent const &event);
 *    void adjustLength(MapEvent::AxisEvent const &event);
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
class CSPSIM_EXPORT InputInterface {
public:
	InputInterface();
	virtual ~InputInterface();

	/** These methods are called by VirtualHID to allow the InputInterface to access
	 *  the raw input events.  The default implementation returns false, which causes
	 *  VirtualHID to continue processing the events and then call onMapEvent().
	 */
	///{
	virtual bool onKey(RawEvent::Keyboard const &) { return false; }
	virtual bool onJoystickButton(RawEvent::JoystickButton const &) { return false; }
	virtual bool onJoystickAxisMotion(RawEvent::JoystickAxis const &) { return false; }
	virtual bool onMouseMove(RawEvent::MouseMotion const &) { return false; }
	virtual bool onMouseButton(RawEvent::MouseButton const &) { return false; }
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

	/** Bind a handler to a click event at runtime.  Using runtime
	 *  handlers (as opposed to the class handlers set by the BIND
	 *  macros) incurs storage costs per instance.
	 */
	void bindClickEvent(std::string const &id, ClickEventSlot const &slot) {
		runtimeDispatch()->bindClick(id, slot);
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
	bool onCommand(std::string const &id, MapEvent::CommandEvent const &);
	bool onAxis(std::string const &id, MapEvent::AxisEvent const &);
	bool onMotion(std::string const &id, MapEvent::MotionEvent const &);

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
	static ::csp::input::InputInterfaceDispatch _InputInterfaceDispatch; \
protected: \
	virtual ::csp::input::InputInterfaceDispatch *_getInputInterfaceDispatch() { return &_InputInterfaceDispatch; } \
	static void _bindInputInterfaceDispatch(::csp::input::InputInterfaceDispatch &_map) { \
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
	::csp::input::InputInterfaceDispatch interface_class::_InputInterfaceDispatch(interface_class::_bindInputInterfaceDispatch);

/** Bind an action handler to an input event identifier string.
 *
 *  @param id the input event identifier string.
 *  @param method a method of the current class taking no parameters
 *    and returning void.
 */
#define BIND_ACTION(id, method) \
	_map.bindEventHandler(id, new ::csp::input::ActionAdapterT<TARGET>(&TARGET::method));

/** Bind a click handler to an input event identifier string.
 *
 *  @param id the input event identifier string.
 *  @param method a method of the current class taking two integer parameters
 *    (the x-y coordinates of the mouse) and returning void.
 */
#define BIND_CLICK(id, method) \
	_map.bindEventHandler(id, new ::csp::input::ClickAdapterT<TARGET>(&TARGET::method));

/** Bind an axis handler to an input event identifier string.
 *
 *  @param id the input event identifier string.
 *  @param method a method of the current class taking a double parameter
 *    ([0.0, 1.0] for the position of the axis) and returning void.
 */
#define BIND_AXIS(id, method) \
	_map.bindEventHandler(id, new ::csp::input::AxisAdapterT<TARGET>(&TARGET::method));

/** Bind a mouse motion handler to an input event identifier string.
 *
 *  @param id the input event identifier string.
 *  @param method a method of the current class taking four integer
 *    parameters (x, y, delta_x, delta_y) and returning void.
 */
#define BIND_MOTION(id, method) \
	_map.bindEventHandler(id, new ::csp::input::MotionAdapterT<TARGET>(&TARGET::method));

} // namespace input
} // namespace csp
