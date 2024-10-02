#pragma once
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

#include <map>
#include <csp/csplib/util/Ref.h>
#include <csp/csplib/util/WeakRef.h>
#include <csp/cspwf/SingleControlContainer.h>
#include <csp/cspwf/StringResourceManager.h>

namespace osg {
	class Group;
	class Geometry;
}

namespace csp {
namespace wf {

class Window;
class WindowManager;

typedef std::vector<Ref<Window> > WindowVector;
typedef std::map<std::string, Ref<Style> > NamedStyleMap;

/** A Control that can be displayed using the WindowManager class. This class is the
 * top level Control that holds all other child controls. This Control has no parent.
 *
 * By default this class only supports a single child control that will take up all
 * of the client area.
 *
 * CanFocus property is true by default for all window controls. This means that
 * the window can receive focus and be put on top when you click on it.
 */
class CSPWF_EXPORT Window : public SingleControlContainer {
public:
	Window();
	virtual ~Window();

	/** Sets the WindowManager that this Window belongs to. */
	virtual void setWindowManager(WindowManager* manager);

	/** Retreives the WindowManager that this Window belongs to. */
	virtual WindowManager* getWindowManager();

	virtual void performLayout();
	virtual void layoutChildControls();

	/** Closes this window. All controls are removed and the window cannot
	 * be reused or displayed again.
	 */
	virtual void close();

	/** Centers the window in the middle of the screen.
	 * Note that this setting also can be set using the Style object.
	 */
	virtual void centerWindow();

	/** Maximizes the window to take up the entire screen.
	 * Note that this setting also can be set using the Style object.
	 */
	virtual void maximizeWindow();

    /** Creates default styles for all window controls.
     * Is very usefull when creating an example application to a
     * very basic visual style.
     */
    virtual void createDefaultStyle();

	/** Returns the Style with the name. A window holds a map with all
	 * named style objects that has been declared within xml file for the
	 * window. All included style files is also added to this map. These
	 * named style objects can then be used by any child control to this
	 * window.
	 */
	virtual optional<Ref<Style> > getNamedStyle(const std::string& name) const;

	/** Adds a named style to the styles map. This named style can then be
	 * used by any child control using the CssClass.
	 */
	virtual void addNamedStyle(const std::string& name, Style* style);

	/** Returns the Window that the Control belongs to. If the control hasn't
	 * been assigned a window yet this method returns NULL.
	 */
	static Window* getWindow(Control* control);

	/** Returns the Window that the Control belongs to. If the control hasn't
	 * been assigned a window yet this method returns NULL.
	 */
	static const Window* getWindow(const Control* control);

	virtual void setTheme(const std::string& theme);
	virtual std::string getTheme() const;

	virtual StringResourceManager* getStringResourceManager();
	virtual const StringResourceManager* getStringResourceManager() const;

	template<class Archive>
	void serialize(Archive & ar)	{
		SingleControlContainer::serialize(ar);
		ar & make_nvp("Styles", m_Styles);
	}

private:
	WeakRef<WindowManager> m_WindowManager;
	std::string m_Theme;
	Ref<StringResourceManager> m_StringResources;

	NamedStyleMap m_Styles;
};

} // namespace wf
} // namespace csp
