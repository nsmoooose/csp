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
#include <csp/cspsim/wf/Serialization.h>

CSP_NAMESPACE

namespace wf {

struct Style {
	optional<std::string> fontFamily;
	optional<double> fontSize;
	optional<osg::Vec4> color;
		
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
	
	optional<std::string> horizontalAlign;
	optional<std::string> verticalAlign;
	
	optional<bool> visible;
	
	template<class Archive>
	void serialize(Archive & ar) {
		ar & make_nvp("FontFamily", fontFamily);
		ar & make_nvp("FontSize", fontSize);
		ar & make_nvp("Color", color);
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

		ar & make_nvp("Visible", visible);
	}
	
	bool operator==(const Style& style) const {
		return equals(style);
	}
	
	bool operator!=(const Style& style) const {
		return !equals(style);
	}
	
	bool equals(const Style& style) const {
		return
			fontFamily.equals(style.fontFamily) &&
			fontSize.equals(style.fontSize) &&
			color.equals(style.color) &&
				
			backgroundColor.equals(style.backgroundColor) &&
			backgroundColorTopLeft.equals(style.backgroundColorTopLeft) &&
			backgroundColorTopRight.equals(style.backgroundColorTopRight) &&
			backgroundColorBottomLeft.equals(style.backgroundColorBottomLeft) &&
			backgroundColorBottomRight.equals(style.backgroundColorBottomRight) &&
			
			backgroundImage.equals(style.backgroundImage) &&
				
			borderWidth.equals(style.borderWidth) &&
			borderTopWidth.equals(style.borderTopWidth) &&
			borderBottomWidth.equals(style.borderBottomWidth) &&
			borderLeftWidth.equals(style.borderLeftWidth) &&
			borderRightWidth.equals(style.borderRightWidth) &&
		
			borderColor.equals(style.borderColor) &&
			borderTopColor.equals(style.borderTopColor) &&
			borderBottomColor.equals(style.borderBottomColor) &&
			borderLeftColor.equals(style.borderLeftColor) &&
			borderRightColor.equals(style.borderRightColor) &&
			
			horizontalAlign.equals(style.horizontalAlign) &&
			verticalAlign.equals(style.verticalAlign) &&
			
			visible.equals(style.visible);
	}		
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_STYLE_H__
