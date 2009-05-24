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
 * @file Control.h
 *
 **/

#ifndef __CSPSIM_WF_CONTROL_H__
#define __CSPSIM_WF_CONTROL_H__

#include <set>

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/Referenced.h>
#include <csp/csplib/util/Signal.h>
#include <csp/csplib/util/WeakRef.h>
#include <csp/csplib/util/WeakReferenced.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/cspwf/Rectangle.h>
#include <csp/cspwf/Serialization.h>
#include <csp/cspwf/Signal.h>
#include <csp/cspwf/Style.h>

#include <osg/ref_ptr>
#include <osg/Vec4>

namespace osg {
	class Geometry;
	class Group;
	class Node;
	class MatrixTransform;
}

namespace csp {
namespace wf {

class Container;
class Control;
class WindowManager;

typedef std::vector<Ref<Control> > ControlVector;

struct EventArgs {
	EventArgs() : handled(false) {}
	bool handled;
};

struct ClickEventArgs : EventArgs {
	ClickEventArgs(Control* c, int X, int Y) : control(c), x(X), y(Y) {}

	Ref<Control> control;
	int x, y;
};

struct HoverEventArgs : EventArgs {
	int x, y;
};

struct MouseEventArgs : EventArgs {
	MouseEventArgs(Control* c, int X, int Y) :
		control(c), x(X), y(Y) {}

	Ref<Control> control;
	int x, y;
};

struct MouseButtonEventArgs : MouseEventArgs {
	MouseButtonEventArgs(Control* c, int b, int X, int Y) :
		MouseEventArgs(c, X, Y), button(b) {}

	int button;
};

typedef sigc::signal<void, ClickEventArgs&> ClickSignal;
typedef sigc::signal<void, HoverEventArgs&> HoverSignal;

typedef sigc::signal<void, MouseEventArgs&> MouseMoveSignal;
typedef sigc::signal<void, MouseButtonEventArgs&> MouseDownSignal;
typedef sigc::signal<void, MouseButtonEventArgs&> MouseUpSignal;

/** The base class for all controls that exists.
 *
 *  All user interface controls must inherit from this class to be managed by the
 *  window framework.
 *
 */
class CSPWF_EXPORT Control : public Referenced, public WeakReferenced {
public:
	Control(std::string name);
	virtual ~Control();

	/** Disposing all resources to free up memory and prepare this object for 
	 * deletion. All signals is also disposed to break circular references.
	 * Override this method to dispose other valuable resources (child controls
	 * or control specific signals).
	 */
	virtual void dispose();

	/** Returns the id of this control. Each control can be given an unique id
	 * to be able to find it in the control hierchy using the Container::getById() 
	 * method.
	 */
	virtual std::string getId() const;
	/** Sets a new id for this control. This id makes it easy to find the control
	 * again using the Container::getById() method.
	 */
	virtual void setId(const std::string& id);

	/** Retreives the name of this class. This name is used together with 
	 * serialization to be able to instantiate the correct class.
	 */
	virtual std::string getName() const;

	/** Returns the Container that holds this Control.
	 */
	virtual Container* getParent();
	
	/** Returns the Container that holds this Control.
	 */
	virtual const Container* getParent() const;
	
	/** Sets a new parent for this control. The parent is held using a
	 * WeakRef to avoid circular references.
	 */
	virtual void setParent(Container* parent);
	
	/** Retreives the WindowManager that this Control belongs to. Note
	 * that this method may return a NULL value if the Window isn't displayed
	 * yet.
	 */
	virtual WindowManager* getWindowManager();

	/** Returns the CssClass to use for this Control. Each Window holds a map 
	 * with named Style objects that you can refer to from any Control. If no
	 * CssClass is assigned we are using inheritence to build the Style object
	 * used to render the Control geometry.
	 */
	virtual optional<std::string> getCssClass() const;
	
	/** Sets a new CssClass to use for this Control. */
	virtual void setCssClass(const optional<std::string>& cssClass);

	/** Returns the node that represents this control. */
	virtual osg::Group* getNode();

	virtual float getZPos() const;
	virtual void setZPos(float zPos);

	virtual const Point getLocation() const;

	virtual const Size getSize() const;

	virtual const Ref<Style> getStyle() const;
	virtual Ref<Style> getStyle();
	
	/** Returns the enabled status of the Control. If the control is disabled it has
	 * the disabled state added. No click signals are fired either.
	 */
	virtual bool getEnabled() const;
	
	/** Sets the enabled status of the Control. If the control is disabled the 
	 * disabled state is added. No click signals are fired either.
	 */
	virtual void setEnabled(bool enabled);

	/** Adds a state to the Control. Example: By adding the state "disabled" you can
	 * use the Label:disabled CssClass to change the appearence of the control. A
	 * Control can have several states assigned. Example: "disabled" and "hover". Then
	 * you should use the following CssClass: "Label:disabled:hover"
	 */
	virtual void addState(const std::string& state);
	
	/** Removes an existing state from this Control. */
	virtual void removeState(const std::string& state);
	
	/** Retreives all states as a single string. Each state is separated with a : 
	 * and sorted in alphabetic order.
	 */
	virtual std::string getState() const;

	/** The following members is signals that any class
	 * can listen to. */
	virtual Signal* getClickSignal();
	ClickSignal Click;
	HoverSignal Hover;

	MouseMoveSignal MouseMove;
	MouseDownSignal MouseDown;
	MouseUpSignal MouseUp;

	/** Fires the click signal. This method is normally called from WindowManager
	 * and shouldn't be used directly. */
	virtual void onClick(ClickEventArgs& event);

	/** Fires the hover signal. This method is normally called from WindowManager
	 * and shouldn't be used directly. */
	virtual void onHover(HoverEventArgs& event);

	/** Fires the mouse move signal. This method is normally called from WindowManager
	 * and shouldn't be used directly. */
	virtual void onMouseMove(MouseEventArgs& event);

	/** Fires the mouse down signal. This method is normally called from WindowManager
	 * and shouldn't be used directly. */
	virtual void onMouseDown(MouseButtonEventArgs& event);

	/** Fires the mouse up signal. This method is normally called from WindowManager
	 * and shouldn't be used directly. */
	virtual void onMouseUp(MouseButtonEventArgs& event);

	template<class Archive>
	void serialize(Archive & ar) {
		ar & make_nvp("@Id", m_Id);
		ar & make_nvp("@Enabled", m_Enabled);
		ar & make_nvp("@CssClass", m_CssClass);
		ar & make_nvp("@LocationX", m_Style->left);
		ar & make_nvp("@LocationY", m_Style->top);
		ar & make_nvp("@LocationZ", m_ZPos);
		ar & make_nvp("@SizeWidth", m_Style->width);
		ar & make_nvp("@SizeHeight", m_Style->height);
		ar & make_nvp("Style", m_Style);
	}

	/** Layout and geometric logic will be suspended. This is very usefull
	 * when you are changing multiple properties that affect properties like 
	 * size, width, position etc. When you are finished with your changes
	 * you should call resumeLayout() for your changes to take effect.
	 */ 
	virtual void suspendLayout();
	
	/** Resumes layout of your controls. All changes you make to properties
	 * like size, width, position etc will take effect immediatly.
	 */
	virtual void resumeLayout();
	
	/** Performs all pending layout and geometric changes. It generates all
	 * geometry that is used to display the Control. It is called just before 
	 * the window is displayed.
	 */
	virtual void performLayout();
	
	/** Returns true if layout has been suspended. 
	 */
	virtual bool layoutSuspended();
	
	void updateMatrix();

protected:

private:
	std::string m_Id;
	std::string m_Name;
	bool m_Enabled;

	typedef std::set<std::string> StateSet;
	StateSet m_States;

	osg::ref_ptr<osg::MatrixTransform> m_TransformGroup;
	optional<std::string> m_CssClass;

	float m_ZPos;

	WeakRef<Container> m_Parent;
	Ref<Style> m_Style;
	bool m_Visible;

	Ref<Signal> m_ClickSignal;

};

} // namespace wf
} // namespace csp

#endif // __CSPSIM_WF_CONTROL_H__


