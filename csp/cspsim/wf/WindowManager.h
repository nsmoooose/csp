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
#include <csp/cspsim/wf/Window.h>

#include <osg/ref_ptr>
#include <osg/Vec4>
#include <vector>

namespace osg {
	class Group;
}

namespace osgUtil {
	class SceneView;
}

CSP_NAMESPACE

namespace wf {

class Serialization;
class Theme;

class WindowManager : public Referenced {
public:
	WindowManager(osgUtil::SceneView* view, Theme* theme, Serialization* serializer);
	virtual ~WindowManager();

	virtual bool pick(int x, int y);
	virtual void show(Window* window);
	virtual void close(Window* window);
	
	template <class Type> 
	bool windowIsOpen() const {
		WindowVector::const_iterator window = m_Windows.begin();
		for(;window != m_Windows.end();++window) {
			Type* type = dynamic_cast<Type*>(window->get());
			if(type != NULL) return true;
		}
		return false;
	}
	
	bool isAnyWindowOpen() {
		return m_Windows.size() > 0;
	}

	//! Returns the default theme of this window manager.
	virtual Theme* getTheme() const;
	
	virtual Serialization* getSerializer() const;
	
private:
	osg::ref_ptr<osgUtil::SceneView> m_View;
	Ref<Theme> m_Theme;
	Ref<Serialization> m_Serializer;
	osg::ref_ptr<osg::Group> m_Group;

	WindowVector m_Windows;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_WINDOWMANAGER_H__
