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
 * @file Default.h
 *
 **/

#ifndef __CSPSIM_WF_THEMES_DEFAULT_H__
#define __CSPSIM_WF_THEMES_DEFAULT_H__

#include <csp/csplib/util/Referenced.h>
#include <csp/cspsim/wf/Theme.h>

CSP_NAMESPACE

namespace wf {

namespace themes {

class Default : public Theme {
public:
	Default();
	virtual ~Default();

	virtual float getBorderWidth() const;
	virtual float getCaptionHeight() const;
	virtual std::string getCaptionFont() const;

	virtual osg::Group* buildCheckBox(const CheckBox* checkBox) const;
	virtual osg::Group* buildLabel(const Label* label) const;
	virtual osg::Group* buildListBox(const ListBox* listBox) const;
	virtual osg::Switch* buildListBoxItem(const ListBox* listBox, const ListBoxItem* listBoxItem) const;
	virtual osg::Group* buildButton(const Button* button) const;
	virtual osg::Group* buildTab(const Tab* tab) const;
	virtual osg::Switch* buildTabButton(const Tab* tab, const TabPage* page, int index) const;
	virtual osg::Group* buildTabPage(const TabPage* page) const;
	virtual osg::Group* buildWindow(const Window* window) const;
	
	virtual Size getWindowClientAreaSize(const Window* window) const;
	virtual Point getWindowClientAreaLocation(const Window* window) const;
	
	virtual Size getTabPageClientAreaSize(const Tab* tab) const;
	virtual Point getTabPageClientAreaLocation(const Tab* tab) const;
	
private:

	struct ThemeColors {
		ThemeColors() :
			buttonBackgroundColor1(0.745f, 0.835f, 0.635f, 1.0f),
			buttonBackgroundColor2(0.0f, 0.0f, 0.0f, 0.0f),
			buttonBorderColor1(0.0f, 0.0f, 0.0f, 1.0f),
			buttonBorderColor2(0.0f, 0.0f, 0.0f, 0.5f),
			buttonBorderWidth(0.4f),

			labelTextColor(0.0f, 0.0f, 0.0f, 1.0f),
			
			windowBackgroundColor(0.349f, 0.576f, 0.298f, 0.8f),
			windowCaptionTextColor(1.0f, 1.0f, 1.0f, 1.0f),
			windowCaptionBackgroundColor1(0.5f, 0.0f, 0.0f, 1.0f),
			windowCaptionBackgroundColor2(0.1f, 0.0f, 0.0f, 0.4f),
			windowCaptionBorderColor1(0.0f, 0.0f, 0.0f, 1.0f),
			windowCaptionBorderColor2(0.0f, 0.0f, 0.0f, 0.0f),
			windowBorderColor1(0.0f, 0.0f, 0.0f, 1.0f),
			windowBorderColor2(0.0f, 0.0f, 0.0f, 0.0f),
			windowCaptionHeight(7.0f),
			
			tabPageBackgroundColor1(0.745f, 0.835f, 0.635f, 1.0f),
			tabPageBackgroundColor2(0.745f, 0.835f, 0.635f, 0.0f),
			tabBorderColor1(0.0f, 0.0f, 0.0f, 1.0f),
			tabBorderColor2(0.0f, 0.0f, 0.0f, 0.5f),
			tabBorderWidth(0.4f),
			tabButtonHeight(7.0f),
			tabButtonBackgroundColorCurrent(0.745f, 0.835f, 0.635f, 1.0f),
			tabButtonBackgroundColorNotCurrent(0.645f, 0.735f, 0.535f, 1.0f),
			
			selectedItemColor(0.984f, 0.984f, 0.984f, 0.486f),
			notSelectedItemColor1(0.349f, 0.576f, 0.298f, 0.4f),
			notSelectedItemColor2(0.349f, 0.576f, 0.298f, 0.4f),
			
			checkBoxHeight(7.0f)
		{}
	
		osg::Vec4 buttonBackgroundColor1, buttonBackgroundColor2;
		osg::Vec4 buttonBorderColor1, buttonBorderColor2;
		osg::Vec4 buttonDisabledBorderColor;
		osg::Vec4 buttonDisabledTextColor;
		double buttonBorderWidth;

		osg::Vec4 labelTextColor;
		
		osg::Vec4 windowBackgroundColor;
		osg::Vec4 windowCaptionTextColor;
		osg::Vec4 windowCaptionBackgroundColor1, windowCaptionBackgroundColor2;
		osg::Vec4 windowCaptionBorderColor1, windowCaptionBorderColor2;
		osg::Vec4 windowBorderColor1, windowBorderColor2;
		double windowCaptionHeight;
		
		osg::Vec4 tabPageBackgroundColor1, tabPageBackgroundColor2;
		osg::Vec4 tabBorderColor1, tabBorderColor2;
		double tabBorderWidth;
		double tabButtonHeight;
		osg::Vec4 tabButtonBackgroundColorCurrent;
		osg::Vec4 tabButtonBackgroundColorNotCurrent;
		
		osg::Vec4 selectedItemColor;
		osg::Vec4 notSelectedItemColor1;
		osg::Vec4 notSelectedItemColor2;
		
		double checkBoxHeight;
	} m_Colors;

	virtual osg::Geometry* buildRectangle(
		float x1, float y1, float x2, float y2, float z,
		const osg::Vec4& c1, const osg::Vec4& c2, const osg::Vec4& c3, const osg::Vec4& c4) const;

	virtual osg::Geometry* buildRectangle(
		float x1, float y1, float x2, float y2, float z, float lineWidth,
		const osg::Vec4& outerColor, const osg::Vec4& innerColor,
		bool left=true, bool top=true, bool right=true, bool bottom=true) const;

	virtual osg::Geode* buildStar() const;
		
	virtual osg::Geometry* buildTriangle(
		const Point& p1, const Point& p2, const Point& p3, float z,
		const osg::Vec4& c1, const osg::Vec4& c2, const osg::Vec4& c3) const;

protected:
	std::string m_Text;
};

} // namespace themes
} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_THEMES_DEFAULT_H__

