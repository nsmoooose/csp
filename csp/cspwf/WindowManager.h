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

namespace csp {
namespace wf {

class Serialization;

/** Class that handles all windows opened.
 * It is responsible for how windows are opened and closed. It is also
 * responsible for sending all events to the right control. This class is abstract
 * and doesn't provide a full implementation. WindowManagerViewer is a
 * full implementation and can be used with osgViewer::Viewer based source code.
 */
class CSPWF_EXPORT WindowManager : public Referenced, public WeakReferenced {
public:
	WindowManager(int width, int height);
	virtual ~WindowManager();

	/** Fires the click event on proper control. Arguments are in screen
	 * coordinates. */
	virtual bool onClick(int x, int y);

	/** Fires the mouse move event on proper controls. Arguments are in screen
	 * coordinates. */
	virtual bool onMouseMove(int x, int y);

	/** Fires the mouse button down event. Arguments are in screen
	 * coordinates. */
	virtual bool onMouseDown(int x, int y, int button);

	/** Fires the mouse button released event. Arguments are in screen
	 * coordinates. */
	virtual bool onMouseUp(int x, int y, int button);

	/** Does the layout process for a window. This is normally only done
	 * once but can be called several times when the screen has been resized.
	 */
	virtual void performLayout(Window* window);

	/** Shows the window sent as an argument.
	 */
	virtual void show(Window* window);

	/** Closes the window. Note that a window is not usable after it has been
	 * closed. A close means that all resources and child controls are disposed
	 * and the window will be deleted when the ref count is zero.
	 */
	virtual void close(Window* window);

	/** Closes all windows.
	 */
	virtual void closeAll();

	/** Rearranges all the windows and put this instance on top.
	 */
	virtual void onTop(Window* window);

	/** Returns the screen size as the window manager knows it.
	 */
	virtual Size getScreenSize() const;

	/** Returns the window by searching for the id. If the window isn't found
	 * NULL is returned.
	 */
	virtual Window* getById(const std::string& id);

	virtual Point getMousePosition() const;

	/** Returns the control at the specified screen coordinates.
	 */
	virtual Control* getControlAtPosition(int x, int y)=0;

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

protected:
	int m_ScreenWidth;
	int m_ScreenHeight;
	osg::ref_ptr<osg::Group> m_Group;

	WindowVector m_Windows;

	// This is the control that the mouse is currently hovering above.
	Ref<Control> m_HoverControl;
	Point m_MousePosition;

	void removeStateAndRebuildGeometry(const std::string& state, Control* control);
	void addStateAndRebuildGeometry(const std::string& state, Control* control);

	void calculateScreenSizeAndScale(Size& size, float& scale) const;

	virtual osg::Group* getWindowNode();
};

} // namespace wf
} // namespace csp

#endif // __CSPSIM_WF_WINDOWMANAGER_H__
