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
#include <csp/cspsim/wf/Rectangle.h>
#include <csp/cspsim/wf/Serialization.h>
#include <csp/cspsim/wf/Signal.h>
#include <csp/cspsim/wf/Style.h>

#include <osg/ref_ptr>
#include <osg/Vec4>

namespace osg {
	class Geometry;
	class Group;
	class Node;
	class MatrixTransform;
}

CSP_NAMESPACE

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
	ClickEventArgs(int X, int Y) : x(X), y(Y) {}
	int x, y;
};

struct HoverEventArgs : EventArgs {
	int x, y;
};

typedef sigc::signal<void, ClickEventArgs&> ClickSignal;	
typedef sigc::signal<void, HoverEventArgs&> HoverSignal;	

/** The base class for all controls that exists.
 *
 *  All user interface controls must inherit from this class to be managed by the 
 *  window framework.
 *
 */
class CSPSIM_EXPORT Control : public Referenced, public WeakReferenced {
public:
	Control();
	virtual ~Control();
	
	virtual void dispose();

	virtual std::string getId() const;
	virtual void setId(const std::string& id);
		
	virtual std::string getName() const;
	
	virtual Container* getParent();
	virtual const Container* getParent() const;
	virtual void setParent(Container* parent);
	virtual WindowManager* getWindowManager();
	
	virtual optional<std::string> getCssClass() const;
	virtual void setCssClass(const optional<std::string>& cssClass);

	/** Function that generates all geometry that is used to display the widget.
	 * 	buildGeometry is called just before the window is displayed. 
	 */
	virtual void buildGeometry();
	
	/** Returns the node that represents this control. */
	virtual osg::Group* getNode();
	
	virtual float getZPos() const;
	virtual void setZPos(float zPos);

	virtual const Point& getLocation() const;
	virtual void setLocation(const Point& point);

	virtual const Size& getSize() const;
	virtual void setSize(const Size& size);
	
	virtual const Style& getStyle() const;
	virtual Style& getStyle();
	
	virtual void addState(const std::string& state);
	virtual void removeState(const std::string& state);
	virtual std::string getState() const;

	/** The following members is signals that any class
	 * can listen to. */
	virtual Signal* getClickSignal();
	ClickSignal Click;
	HoverSignal Hover;
	
	// Fires the click signal.
	virtual void onClick(ClickEventArgs& event);
	virtual void onHover(HoverEventArgs& event);

	template<class Archive>
	void serialize(Archive & ar) {
		ar & make_nvp("@Id", m_Id);
		ar & make_nvp("@CssClass", m_CssClass);
		ar & make_nvp("@LocationX", m_Point.x);
		ar & make_nvp("@LocationY", m_Point.y);
		ar & make_nvp("@LocationZ", m_ZPos);
		ar & make_nvp("@SizeWidth", m_Size.width);
		ar & make_nvp("@SizeHeight", m_Size.height);
		ar & make_nvp("Style", m_Style);
	}	
	
protected:

private:
	std::string m_Id;
	
	typedef std::set<std::string> StateSet;
	StateSet m_States; 

	osg::ref_ptr<osg::MatrixTransform> m_TransformGroup;
	optional<std::string> m_CssClass;

	float m_ZPos;
	Point m_Point;
	Size m_Size;
	
	WeakRef<Container> m_Parent;
	Style m_Style;
	bool m_Visible;
	
	Ref<Signal> m_ClickSignal;
	
	void updateMatrix();
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_CONTROL_H__


