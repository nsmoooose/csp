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
 * @file ControlGeometryBuilder.h
 *
 **/

#ifndef __CSPSIM_WF_CONTROLGEOMETRYBUILDER_H__
#define __CSPSIM_WF_CONTROLGEOMETRYBUILDER_H__

#include <osg/ref_ptr>
#include <osg/Vec4>
#include <string>
#include <vector>
#include <csp/csplib/util/Namespace.h>

namespace osg {
	class Group;
	class Geode;
	class Geometry;
	class Switch;
}

namespace osgText {
	class Text;
}

namespace csp {
namespace wf {

struct Point;
struct Size;
class Button;
class Check;
class CheckBox;
class Control;
class Image;
class Label;
class ListBox;
class ListBoxItem;
class Model;
class Style;
class Tab;
class TabPage;
class TabHeader;
class Window;

class ControlGeometryBuilder {
public:
	ControlGeometryBuilder();
	virtual ~ControlGeometryBuilder();
	
	virtual void buildControl(osg::Geode* geode, float& z, const Style* style, const Control* control) const;
	virtual osg::Group* buildButton(const Button* button) const;
	virtual osg::Group* buildCheck(const Check* check) const;
	virtual osg::Group* buildCheckBox(const CheckBox* checkBox) const;
	virtual osg::Group* buildGenericControl(const Control* control) const;
	virtual osg::Group* buildImage(const Image* image) const;
	virtual osg::Group* buildLabel(const Label* label) const;
	virtual osg::Group* buildListBox(const ListBox* listBox) const;
	virtual osg::Group* buildListBoxItem(const ListBoxItem* listBoxItem) const;
	virtual osg::Group* buildModel(const Model* model) const;
	virtual osg::Group* buildTab(const Tab* tab) const;
	virtual osg::Group* buildTabHeader(const TabHeader* header) const;
	virtual osg::Group* buildTabPage(const TabPage* page) const;
	virtual osg::Group* buildWindow(const Window* window) const;
	virtual Size getSizeOfText(const std::string& text, const std::string& fontFamily, float fontSize) const;
	virtual Size calculateSize(const Control* control, const Style* style) const;
	virtual Point calculateLocation(const Control* control, const Style* style) const;
private:
	void getNextLayer(float& z) const;

	virtual osg::Geometry* buildRectangle(
		float x1, float y1, float x2, float y2, float z,
		const osg::Vec4& c1, const osg::Vec4& c2, const osg::Vec4& c3, const osg::Vec4& c4) const;

	virtual osg::Geometry* buildRectangle(
		float x1, float y1, float x2, float y2, float z, float lineWidth,
		const osg::Vec4& outerColor, const osg::Vec4& innerColor,
		bool left=true, bool top=true, bool right=true, bool bottom=true) const;
		
	virtual osgText::Text* buildText(const std::string& text, const std::string& fontFamily, float fontSize, osg::Vec4& color) const;
};

} // namespace wf
} // namespace csp

#endif // __CSPSIM_WF_CONTROLGEOMETRYBUILDER_H__

