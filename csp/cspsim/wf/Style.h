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
 * @file Style.h
 *
 **/

#ifndef __CSPSIM_WF_STYLE_H__
#define __CSPSIM_WF_STYLE_H__

#include <osg/Vec4>
#include <string>

#include <csp/csplib/util/Optional.h>
#include <csp/csplib/util/Referenced.h>
#include <csp/cspsim/wf/Serialization.h>

CSP_NAMESPACE

namespace wf {
	
class Control;
class StyleBuilder;

/** This object defines all visual information about a Control.
 * Color, font, size, border, width is some of the properties that can be set.
 * Each Control has a Style object assigned with this information. Note that 
 * you must use the StyleBuilder class in order to get the actual Style object
 * in use. Many things isn't set directly on a control. They are set globaly
 * instead. The basic strategy is something that resembles html and css
 */
class Style : public Referenced {
public:
	friend class Control;
	friend class StyleBuilder;

	/** Describes the unit to use for properties like width, height, left and top. */
	enum Unit {
		Pixels,
		Percentage
	};
	
	/** A unit (pixels, percentage) and a value describing a width, height or position
	 * of a control. */
	struct UnitValue {
		UnitValue() : unit(Pixels), value(0.0f) {}
		UnitValue(const Unit& unit_in, const float& value_in) : unit(unit_in), value(value_in) {}
		
		Unit unit;
		float value;
		
		bool operator==(const UnitValue& rhs) {
			return unit == rhs.unit && value == rhs.value;
		}
	};

	optional<std::string> getFontFamily() const;
	void setFontFamily(const optional<std::string>& value);
	
	optional<double> getFontSize() const;
	void setFontSize(const optional<double>& value);
	
	optional<osg::Vec4> getColor() const;
	void setColor(const optional<osg::Vec4>& value);
	
	optional<std::string> getTextHorizontalAlign() const;
	void setTextHorizontalAlign(const optional<std::string>& value);
	
	optional<std::string> getTextVerticalAlign() const;
	void setTextVerticalAlign(const optional<std::string>& value);
	
	optional<osg::Vec4> getBackgroundColor() const;
	void setBackgroundColor(const optional<osg::Vec4>& value);
	
	optional<osg::Vec4> getBackgroundColorTopLeft() const;
	void setBackgroundColorTopLeft(const optional<osg::Vec4>& value); 
	
	optional<osg::Vec4> getBackgroundColorTopRight() const;
	void setBackgroundColorTopRight(const optional<osg::Vec4>& value);
	
	optional<osg::Vec4> getBackgroundColorBottomLeft() const;
	void setBackgroundColorBottomLeft(const optional<osg::Vec4>& value);
	
	optional<osg::Vec4> getBackgroundColorBottomRight() const;
	void setBackgroundColorBottomRight(const optional<osg::Vec4>& value);
	
	optional<std::string> getBackgroundImage() const;
	void setBackgroundImage(const optional<std::string>& value);
	
	optional<float> getBorderWidth() const;
	void setBorderWidth(const optional<float>& value); 
 
	optional<float> getBorderTopWidth() const;
	void setBorderTopWidth(const optional<float>& value);
	
	optional<float> getBorderBottomWidth() const;
	void setBorderBottomWidth(const optional<float>& value); 

	optional<float> getBorderLeftWidth() const;
	void setBorderLeftWidth(const optional<float>& value); 
	 
	optional<float> getBorderRightWidth() const;
	void setBorderRightWidth(const optional<float>& value); 

	optional<osg::Vec4> getBorderColor() const;
	void setBorderColor(const optional<osg::Vec4>& value); 

	optional<osg::Vec4> getBorderTopColor() const;
	void setBorderTopColor(const optional<osg::Vec4>& value); 

	optional<osg::Vec4> getBorderBottomColor() const;
	void setBorderBottomColor(const optional<osg::Vec4>& value); 

	optional<osg::Vec4> getBorderLeftColor() const;
	void setBorderLeftColor(const optional<osg::Vec4>& value); 

	optional<osg::Vec4> getBorderRightColor() const;
	void setBorderRightColor(const optional<osg::Vec4>& value); 

	optional<std::string> getHorizontalAlign() const;
	void setHorizontalAlign(const optional<std::string>& value);
	
	optional<std::string> getVerticalAlign() const;
	void setVerticalAlign(const optional<std::string>& value);
	
	optional<bool> getVisible() const;
	void setVisible(const optional<bool>& value);
	
	optional<UnitValue> getLeft() const;
	void setLeft(const optional<UnitValue>& value);
	
	optional<UnitValue> getTop() const;
	void setTop(const optional<UnitValue>& value);

	optional<UnitValue> getHeight() const;
	void setHeight(const optional<UnitValue>& value);

	optional<UnitValue> getWidth() const;
	void setWidth(const optional<UnitValue>& value);

	template<class Archive>
	void serialize(Archive & ar) {
		ar & make_nvp("FontFamily", fontFamily);
		ar & make_nvp("FontSize", fontSize);
		ar & make_nvp("Color", color);
		ar & make_nvp("TextHorizontalAlign", textHorizontalAlign);
		ar & make_nvp("TextVerticalAlign", textVerticalAlign);
		
		ar & make_nvp("BackgroundColor", backgroundColor);
		ar & make_nvp("BackgroundColorTopLeft", backgroundColorTopLeft);
		ar & make_nvp("BackgroundColorTopRight", backgroundColorTopRight);
		ar & make_nvp("BackgroundColorBottomLeft", backgroundColorBottomLeft);
		ar & make_nvp("BackgroundColorBottomRight", backgroundColorBottomRight);
		
		ar & make_nvp("BackgroundImage", backgroundImage);
		
		ar & make_nvp("BorderWidth", borderWidth);
		ar & make_nvp("BorderTopWidth", borderTopWidth);
		ar & make_nvp("BorderBottomWidth", borderBottomWidth);
		ar & make_nvp("BorderLeftWidth", borderLeftWidth);
		ar & make_nvp("BorderRightWidth", borderRightWidth);

		ar & make_nvp("BorderColor", borderColor);
		ar & make_nvp("BorderTopColor", borderTopColor);
		ar & make_nvp("BorderBottomColor", borderBottomColor);
		ar & make_nvp("BorderLeftColor", borderLeftColor);
		ar & make_nvp("BorderRightColor", borderRightColor);

		ar & make_nvp("HorizontalAlign", horizontalAlign);
		ar & make_nvp("VerticalAlign", verticalAlign);

		ar & make_nvp("Left", left);
		ar & make_nvp("Top", top);
		ar & make_nvp("Width", width);
		ar & make_nvp("Height", height);

		ar & make_nvp("Visible", visible);
	}
	
	bool operator==(const Style& style) const;
	
	bool operator!=(const Style& style) const;
	
	bool equals(const Style* style) const;
	
	Ref<Style> clone() const;

private:

	optional<std::string> fontFamily;
	optional<double> fontSize;
	optional<osg::Vec4> color;
	
	// TODO: These should be an enumeration. 
	optional<std::string> textHorizontalAlign;
	optional<std::string> textVerticalAlign;
		
	optional<osg::Vec4> backgroundColor;
	optional<osg::Vec4> backgroundColorTopLeft;
	optional<osg::Vec4> backgroundColorTopRight;
	optional<osg::Vec4> backgroundColorBottomLeft;
	optional<osg::Vec4> backgroundColorBottomRight;
	
	optional<std::string> backgroundImage;
		
	optional<float> borderWidth;
	optional<float> borderTopWidth;
	optional<float> borderBottomWidth;
	optional<float> borderLeftWidth;
	optional<float> borderRightWidth;

	optional<osg::Vec4> borderColor;
	optional<osg::Vec4> borderTopColor;
	optional<osg::Vec4> borderBottomColor;
	optional<osg::Vec4> borderLeftColor;
	optional<osg::Vec4> borderRightColor;
	
	// TODO: These should be an enumeration. 
	optional<std::string> horizontalAlign;
	optional<std::string> verticalAlign;
	
	optional<bool> visible;
		
	optional<UnitValue> left;
	optional<UnitValue> top;
	optional<UnitValue> height;
	optional<UnitValue> width;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_STYLE_H__
