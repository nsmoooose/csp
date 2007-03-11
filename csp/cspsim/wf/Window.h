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

#include <map>
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
typedef std::map<std::string, Style> NamedStyleMap;

class Window : public SingleControlContainer {
public:
	Window();
	virtual ~Window();

	virtual std::string getName() const;

	virtual void setWindowManager(WindowManager* manager);
	virtual WindowManager* getWindowManager();
	
	virtual void buildGeometry();
	virtual void layoutChildControls();
	
	virtual void close();
	
	virtual void centerWindow();
	virtual void maximizeWindow();
	
	virtual optional<Style> getNamedStyle(const std::string& name) const;
	virtual void addNamedStyle(const std::string& name, const Style& style);

	static Window* getWindow(Control* control);
	static const Window* getWindow(const Control* control);

	virtual void setTheme(const std::string& theme);
	virtual std::string getTheme() const;
	
	template<class Archive>
	void serialize(Archive & ar)	{
		SingleControlContainer::serialize(ar);
		ar & make_nvp("Styles", m_Styles);
	}
	
private:
	WindowManager* m_WindowManager;
	std::string m_Theme;

	NamedStyleMap m_Styles;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_WINDOW_H__


