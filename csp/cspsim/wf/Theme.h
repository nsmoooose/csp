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
 * @file Theme.h
 *
 **/

#ifndef __CSPSIM_WF_THEME_H__
#define __CSPSIM_WF_THEME_H__

#include <csp/csplib/util/Referenced.h>

#include <osg/ref_ptr>
#include <osg/Vec4>
#include <vector>

namespace osg {
	class Group;
	class Geode;
	class Geometry;
	class Switch;
}

CSP_NAMESPACE

namespace wf {

struct Point;
struct Size;
class Button;
class CheckBox;
class Label;
class ListBox;
class ListBoxItem;
class Tab;
class TabPage;
class Window;

class Theme : public Referenced {
public:
	virtual osg::Group* buildButton(const Button* button) const =0;
	virtual osg::Group* buildCheckBox(const CheckBox* checkBox) const =0;
	virtual osg::Group* buildLabel(const Label* label) const =0;
	virtual osg::Group* buildTab(const Tab* tab) const =0;
	virtual osg::Switch* buildTabButton(const Tab* tab, const TabPage* page, int index) const =0;
	virtual osg::Group* buildTabPage(const TabPage* page) const =0;
	virtual osg::Group* buildWindow(const Window* window) const =0;
	virtual osg::Group* buildListBox(const ListBox* listBox) const =0;
	virtual osg::Switch* buildListBoxItem(const ListBox* listBox, const ListBoxItem* listBoxItem) const =0;

	virtual float getBorderWidth() const =0;
	virtual float getCaptionHeight() const =0;
	virtual std::string getCaptionFont() const =0;

	virtual Size getWindowClientAreaSize(const Window* window) const =0;
	virtual Point getWindowClientAreaLocation(const Window* window) const =0;

	virtual Size getTabPageClientAreaSize(const Tab* tab) const =0;
	virtual Point getTabPageClientAreaLocation(const Tab* tab) const =0;

	/*
	
	virtual osg::Geode buildButton(
		const Button& button	// The button to generate geometry for.
		)=0;
	*/
	virtual ~Theme() {}
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_THEME_H__
