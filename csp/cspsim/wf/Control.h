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

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/Referenced.h>
#include <csp/csplib/util/Signal.h>
#include <csp/csplib/data/Vector3.h>
#include <csp/cspsim/wf/Point.h>
#include <csp/cspsim/wf/Serialization.h>

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

class Control;
class Theme;
class WindowManager;

typedef std::vector<Ref<Control> > ControlVector;

struct Rect {
	float x0, y0, x1, y1;
	Rect() {}
	Rect(float x0_, float y0_, float x1_, float y1_): x0(x0_), y0(y0_), x1(x1_), y1(y1_) {}
	inline float width() const { return x1 - x0; }
	inline float height() const { return y0 - y1; }
};

/** The base class for all controls that exists.
 *
 *  All user interface controls must inherit from this class to be managed by the 
 *  window framework.
 *
 */
class Control : public Referenced {
public:
	typedef sigc::signal<void> loadSignal;	
	loadSignal load;

	Control(Theme* theme);
	virtual ~Control();

	virtual void onLoad();

	virtual const std::string& getId() const;
	virtual void setId(const std::string& id);
	
	virtual Control* getParent();
	virtual void setParent(Control* parent);
	virtual Theme* getTheme();
	virtual WindowManager* getWindowManager();

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
	
	template<class Archive>
	void serialize(Archive & ar)	{
		ar & make_nvp("@Id", m_Id);
		ar & make_nvp("@LocationX", m_Point.m_X);
		ar & make_nvp("@LocationY", m_Point.m_Y);
		ar & make_nvp("@LocationZ", m_ZPos);
		ar & make_nvp("@SizeWidth", m_Size.m_W);
		ar & make_nvp("@SizeHeight", m_Size.m_H);
	}	
	
protected:

private:
	const Ref<Theme> m_Theme;
	std::string m_Id;

	osg::ref_ptr<osg::MatrixTransform> m_TransformGroup;

	float m_ZPos;
	Point m_Point;
	Size m_Size;
	
	Control* m_Parent;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_CONTROL_H__


