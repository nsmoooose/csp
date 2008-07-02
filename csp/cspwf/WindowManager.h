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
 * @file WindowManager.h
 *
 **/

#ifndef __CSPSIM_WF_WINDOWMANAGER_H__
#define __CSPSIM_WF_WINDOWMANAGER_H__

#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/Referenced.h>
#include <csp/csplib/util/WeakReferenced.h>
#include <csp/cspwf/Window.h>

#include <osg/ref_ptr>
#include <osg/Vec4>
#include <vector>

namespace osg {
	class Group;
	class State;
}

namespace osgUtil {
	class SceneView;
}

namespace csp {
namespace wf {

class Serialization;

class CSPWF_EXPORT WindowManager : public Referenced, public WeakReferenced {
public:
	WindowManager(osg::State* state, int screenWidth, int screenHeight);
	virtual ~WindowManager();
	
	virtual osgUtil::SceneView* getSceneView();

	virtual bool onClick(int x, int y);
	virtual bool onMouseMove(int x, int y, int dx, int dy);
	
	virtual void show(Window* window);
	virtual void close(Window* window);
	virtual void closeAll();
	
	virtual Size getScreenSize() const;
	
	virtual Window* getById(const std::string& id);

	virtual void onUpdate(float dt);
	virtual void onRender();
	
	virtual Point getMousePosition() const;
	virtual Control* getControlAtPosition(int x, int y);
	
	template <class Type> 
	bool windowIsOpen() const {
		WindowVector::const_iterator window = m_Windows.begin();
		for(;window != m_Windows.end();++window) {
			Type* type = dynamic_cast<Type*>(window->get());
			if(type != NULL) return true;
		}
		return false;
	}
	
	template <class Type>
	Ref<Type> getWindowByType() {
		WindowVector::const_iterator window = m_Windows.begin();
		for(;window != m_Windows.end();++window) {
			Type* type = dynamic_cast<Type*>(window->get());
			if(type != NULL) return *window;
		}
		return Ref<Type>();
	}
	
	template <class Type>
	void closeByType() {
		Ref<Type> window = getWindowByType<Type>();
		while(window.valid()) {
			close(window.get());
			window = getWindowByType<Type>();
		}
	}
	
	bool isAnyWindowOpen() {
		return m_Windows.size() > 0;
	}
		
private:
	int m_ScreenWidth;
	int m_ScreenHeight;
	osg::ref_ptr<osgUtil::SceneView> m_View;
	osg::ref_ptr<osg::Group> m_Group;

	WindowVector m_Windows;
	
	// This is the control that the mouse is currently hovering above.
	Ref<Control> m_HoverControl;
	Point m_MousePosition;
	
	void removeStateAndRebuildGeometry(const std::string& state, Control* control);
	void addStateAndRebuildGeometry(const std::string& state, Control* control);
	
	void calculateScreenSizeAndScale(Size& size, float& scale) const;
};

} // namespace wf
} // namespace csp

#endif // __CSPSIM_WF_WINDOWMANAGER_H__
