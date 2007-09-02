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
 * @file Style.cpp
 *
 **/

#include <csp/csplib/util/Ref.h>
#include <csp/cspsim/wf/Style.h>

CSP_NAMESPACE

namespace wf {
	
struct StyleEqualComparer {
	StyleEqualComparer(Style* lhs, Style* rhs) : 
		equal(true), m_lhs(lhs), m_rhs(rhs) {}
	
	template<class T>
	StyleEqualComparer* operator &(std::pair<std::string, T*> nvp) {
		size_t offset = reinterpret_cast<const char*>(nvp.second) - reinterpret_cast<const char*>(m_rhs.get());
		const T* lhs_property = reinterpret_cast<const T*>(reinterpret_cast<const char*>(m_lhs.get()) + offset);

		if(!lhs_property->equals(*nvp.second)) {
			equal = false;
		}
		
		return this;
	}
	
	bool equal;
	Ref<Style> m_lhs;
	Ref<Style> m_rhs;
};

struct StyleCloner {
	StyleCloner(Style* from, Style* to) : m_from(from), m_to(to) {}
	
	template<class T>
	StyleCloner* operator &(std::pair<std::string, T*> nvp) {
		size_t offset = reinterpret_cast<const char*>(nvp.second) - reinterpret_cast<const char*>(m_from.get());
		T* to_property = reinterpret_cast<T*>(reinterpret_cast<char*>(m_to.get()) + offset);

		(*to_property) = *nvp.second;
		return this;
	}
	
	Ref<Style> m_from;
	Ref<Style> m_to;
};

optional<std::string> Style::getFontFamily() const {
	return fontFamily;
}

void Style::setFontFamily(const optional<std::string>& value) {
	fontFamily = value;
}

optional<double> Style::getFontSize() const {
	return fontSize;
}

void Style::setFontSize(const optional<double>& value) {
	fontSize = value;
}

optional<osg::Vec4> Style::getColor() const {
	return color;
}

void Style::setColor(const optional<osg::Vec4>& value) {
	color = value;
}

optional<std::string> Style::getTextHorizontalAlign() const {
	return textHorizontalAlign;
}

void Style::setTextHorizontalAlign(const optional<std::string>& value) {
	textHorizontalAlign = value;
}
	
optional<std::string> Style::getTextVerticalAlign() const {
	return textVerticalAlign;
}

void Style::setTextVerticalAlign(const optional<std::string>& value) {
	textVerticalAlign = value;
}

optional<osg::Vec4> Style::getBackgroundColor() const {
	return backgroundColor;
}

void Style::setBackgroundColor(const optional<osg::Vec4>& value) {
	backgroundColor = value;
}

optional<osg::Vec4> Style::getBackgroundColorTopLeft() const {
	return backgroundColorTopLeft;
}

void Style::setBackgroundColorTopLeft(const optional<osg::Vec4>& value) {
	backgroundColorTopLeft = value;
} 

optional<osg::Vec4> Style::getBackgroundColorTopRight() const {
	return backgroundColorTopRight;
}

void Style::setBackgroundColorTopRight(const optional<osg::Vec4>& value) {
	backgroundColorTopRight = value;
}

optional<osg::Vec4> Style::getBackgroundColorBottomLeft() const {
	return backgroundColorBottomLeft;
}

void Style::setBackgroundColorBottomLeft(const optional<osg::Vec4>& value) {
	backgroundColorBottomLeft = value;
}

optional<osg::Vec4> Style::getBackgroundColorBottomRight() const {
	return backgroundColorBottomRight;
}

void Style::setBackgroundColorBottomRight(const optional<osg::Vec4>& value) {
	backgroundColorBottomRight = value;
}
	
optional<std::string> Style::getBackgroundImage() const {
	return backgroundImage;
}

void Style::setBackgroundImage(const optional<std::string>& value) {
	backgroundImage = value;
}

optional<float> Style::getBorderWidth() const {
	return borderWidth;
}

void Style::setBorderWidth(const optional<float>& value) {
	borderWidth = value;
} 
 
optional<float> Style::getBorderTopWidth() const {
	return borderTopWidth;
}

void Style::setBorderTopWidth(const optional<float>& value) {
	borderTopWidth = value;
}

optional<float> Style::getBorderBottomWidth() const {
	return borderBottomWidth;
}

void Style::setBorderBottomWidth(const optional<float>& value) {
	borderBottomWidth = value;
} 

optional<float> Style::getBorderLeftWidth() const {
	return borderLeftWidth;
}

void Style::setBorderLeftWidth(const optional<float>& value) {
	borderLeftWidth = value;
} 
	 
optional<float> Style::getBorderRightWidth() const {
	return borderRightWidth;
}

void Style::setBorderRightWidth(const optional<float>& value) {
	borderRightWidth = value;
} 

optional<osg::Vec4> Style::getBorderColor() const {
	return borderColor;
}

void Style::setBorderColor(const optional<osg::Vec4>& value) {
	borderColor = value;
} 

optional<osg::Vec4> Style::getBorderTopColor() const {
	return borderTopColor;
}

void Style::setBorderTopColor(const optional<osg::Vec4>& value) {
	borderTopColor = value;
} 

optional<osg::Vec4> Style::getBorderBottomColor() const {
	return borderBottomColor;
}

void Style::setBorderBottomColor(const optional<osg::Vec4>& value) {
	borderBottomColor = value;
} 

optional<osg::Vec4> Style::getBorderLeftColor() const {
	return borderLeftColor;
}

void Style::setBorderLeftColor(const optional<osg::Vec4>& value) {
	borderLeftColor = value;
} 

optional<osg::Vec4> Style::getBorderRightColor() const {
	return borderRightColor;
}

void Style::setBorderRightColor(const optional<osg::Vec4>& value) {
	borderRightColor = value;
}

optional<std::string> Style::getHorizontalAlign() const {
	return horizontalAlign;
}

void Style::setHorizontalAlign(const optional<std::string>& value) {
	horizontalAlign = value;
}

optional<std::string> Style::getVerticalAlign() const {
	return verticalAlign;
}

void Style::setVerticalAlign(const optional<std::string>& value) {
	verticalAlign = value;
}

optional<bool> Style::getVisible() const {
	return visible;
}

void Style::setVisible(const optional<bool>& value) {
	visible = value;
}
	
optional<Style::UnitValue> Style::getLeft() const {
	return left;
}

void Style::setLeft(const optional<UnitValue>& value) {
	left = value;
}

optional<Style::UnitValue> Style::getTop() const {
	return top;
}

void Style::setTop(const optional<UnitValue>& value) {
	top = value;
}

optional<Style::UnitValue> Style::getHeight() const {
	return height;
}

void Style::setHeight(const optional<UnitValue>& value) {
	height = value;
}

optional<Style::UnitValue> Style::getWidth() const {
	return width;
}

void Style::setWidth(const optional<UnitValue>& value) {
	width = value;
}

bool Style::operator==(const Style& style) const {
	return equals(&style);
}

bool Style::operator!=(const Style& style) const {
	return !equals(&style);
}

bool Style::equals(const Style* style) const {
	// The StyleEqualComparer is dependent on the serialize method that isn't 
	// const. But we know that we aren't changing anything so we removes the
	// const to make it compile.
	Style* rhs = const_cast<Style*>(style);
	Style* lhs = const_cast<Style*>(this);
	StyleEqualComparer comparer(lhs, rhs);
	rhs->serialize(comparer);
	return comparer.equal;
}

Ref<Style> Style::clone() const {
	Ref<Style> to = new Style;
	Style* from = const_cast<Style*>(this);

	StyleCloner cloner(from, to.get());
	from->serialize(cloner);
	return to;
}		

} // namespace wf

CSP_NAMESPACE_END
