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
 * @file Window.h
 *
 **/

#ifndef __CSPSIM_WF_WINDOW_H__
#define __CSPSIM_WF_WINDOW_H__

#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/wf/SingleControlContainer.h>

namespace osg {
	class Group;
	class Geometry;
}

CSP_NAMESPACE

namespace wf {

class Window;
class WindowManager;

typedef std::vector<Ref<Window> > WindowVector;

class Window : public SingleControlContainer {
public:
	friend WindowManager;
	
	Window();
	Window(std::string caption);
	virtual ~Window();

	virtual void setWindowManager(WindowManager* manager);
	virtual WindowManager* getWindowManager();
	
	virtual const std::string &getCaption() const;
	virtual void setCaption(const std::string &caption);
	
	virtual void buildGeometry();
	virtual void layoutChildControls();
	
	virtual void close();

	template<class Archive>
	void serialize(Archive & ar)	{
		SingleControlContainer::serialize(ar);
		ar & make_nvp("@Caption", m_Caption);
	}

private:
	WindowManager* m_WindowManager;

	std::string m_Caption;	
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_WINDOW_H__


