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
 * @file StyleBuilder.cpp
 *
 **/

#include <csp/cspsim/wf/Style.h>
#include <csp/cspsim/wf/StyleBuilder.h>
#include <csp/cspsim/wf/Window.h>

CSP_NAMESPACE

namespace wf {

template<class T>
bool buildStyleFromCssClass(Style& styleToBuild, T& propertyToBuild, const Style& classStyle) {
	// Calculate the offset to the member variable from the destination struct.	
	// By using the offset we can safely check and fetch a value from actual style that
	// exists on the control.
	size_t offset = reinterpret_cast<const char*>(&propertyToBuild) - reinterpret_cast<const char*>(&styleToBuild);
	const T* sourceProperty = reinterpret_cast<const T*>(reinterpret_cast<const char*>(&classStyle) + offset);
	
	// Are there any value set on the style struct?
	if(sourceProperty->get_ptr() != NULL) {
		// Yes a value was found. Lets assign that value to the destination struct.
		propertyToBuild = (*sourceProperty);
		return true;
	}
	return false;
}

template<class T>
void buildInheritedValue(Style& styleToBuild, T& propertyToBuild, const Control* control) {
	const Style& controlStyle = control->getStyle();

	if(!buildStyleFromCssClass(styleToBuild, propertyToBuild, controlStyle)) {
		// No there was no value. Lets check the parent control to see if that control
		// has any value.
		const Control* parent = control->getParent();
		if(parent != NULL) {
			buildInheritedValue(styleToBuild, propertyToBuild, parent);
		}
	}
}

template<class T>
void buildStyleForSingleProperty(Style& styleToBuild, T& propertyToBuild, const Control* control) {
	if(propertyToBuild) {
		// There was already a value on the style. This will override all
		// other styles.
		return;
	}
	
	const Container* parent = control->getParent();
	if(parent != NULL) {
		buildInheritedValue(styleToBuild, propertyToBuild, parent);
	}

	// Retreive the window control. It is mandatory for the
	// rest of the method call to be completed.
	const Window* window = Window::getWindow(control);
	if(window == NULL) {
		return;
	}

	// Lets fetch value from class with the same name as the 
	// control class. All labels can share same values with
	// the "Label" style name. This style is more specific 
	// than the inherited one. This style will override
	// inherited values.
	const std::string className = control->getName();
	optional<Style> cssStyle = window->getNamedStyle(className);
	if(cssStyle) {
		buildStyleFromCssClass(styleToBuild, propertyToBuild, *cssStyle);
	}

	// Lets fetch value from a specified class if it exists.
	// This is the most specific style. This style will override
	// both inherited style and control specific style.
	const optional<std::string> cssClassName = control->getCssClass();
	if(cssClassName) {
		optional<Style> cssStyle = window->getNamedStyle(*cssClassName);
		if(cssStyle) {
			buildStyleFromCssClass(styleToBuild, propertyToBuild, *cssStyle);
		}
	}
}

template<class T>
void buildStyleForSinglePropertyWithState(Style& styleToBuild, T& propertyToBuild, 
	const Control* control, const std::string& stateName) {
	if(propertyToBuild) {
		// There was already a value on the style. This will override all
		// other styles.
		return;
	}
	
	// Build an inherited value starting with the current control.
	buildInheritedValue(styleToBuild, propertyToBuild, control);

	// Retreive the window control. It is mandatory for the
	// rest of the method call to be completed.
	const Window* window = Window::getWindow(control);
	if(window == NULL) {
		return;
	}

	// Lets fetch value from class with the same name as the 
	// control class. All labels can share same values with
	// the "Label" style name. This style is more specific 
	// than the inherited one. This style will override
	// inherited values.
	const std::string className = control->getName() + ":" + stateName;
	optional<Style> cssStyle = window->getNamedStyle(className);
	if(cssStyle) {
		buildStyleFromCssClass(styleToBuild, propertyToBuild, *cssStyle);
	}

	// Lets fetch value from a specified class if it exists.
	// This is the most specific style. This style will override
	// both inherited style and control specific style.
	const optional<std::string> cssClassName = control->getCssClass() + ":" + stateName;
	if(cssClassName) {
		optional<Style> cssStyle = window->getNamedStyle(*cssClassName);
		if(cssStyle) {
			buildStyleFromCssClass(styleToBuild, propertyToBuild, *cssStyle);
		}
	}
}

Style StyleBuilder::buildStyle(const Control* control) {
	// This method will imitate CSS to some extent. Well the
	// most important things is here anyway...
	
	// First we try to get any value set directly on the control or any of the 
	// parents.
	Style style = control->getStyle();
	buildStyleForSingleProperty(style, style.fontFamily, control);
	buildStyleForSingleProperty(style, style.fontSize, control);
	buildStyleForSingleProperty(style, style.color, control);
	buildStyleForSingleProperty(style, style.backgroundColor, control);
	buildStyleForSingleProperty(style, style.backgroundColorTopLeft, control);
	buildStyleForSingleProperty(style, style.backgroundColorTopRight, control);
	buildStyleForSingleProperty(style, style.backgroundColorBottomLeft, control);
	buildStyleForSingleProperty(style, style.backgroundColorBottomRight, control);

	buildStyleForSingleProperty(style, style.backgroundImage, control);

	buildStyleForSingleProperty(style, style.borderWidth, control);
	buildStyleForSingleProperty(style, style.borderTopWidth, control);
	buildStyleForSingleProperty(style, style.borderBottomWidth, control);
	buildStyleForSingleProperty(style, style.borderLeftWidth, control);
	buildStyleForSingleProperty(style, style.borderRightWidth, control);
	
	buildStyleForSingleProperty(style, style.borderColor, control);
	buildStyleForSingleProperty(style, style.borderTopColor, control);
	buildStyleForSingleProperty(style, style.borderBottomColor, control);
	buildStyleForSingleProperty(style, style.borderLeftColor, control);
	buildStyleForSingleProperty(style, style.borderRightColor, control);

	buildStyleForSingleProperty(style, style.visible, control);
	return style;
}

void StyleBuilder::buildStyle(Style& style, const Control* control, const std::string& stateName) {
	buildStyleForSinglePropertyWithState(style, style.fontFamily, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.fontSize, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.color, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.backgroundColor, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.backgroundColorTopLeft, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.backgroundColorTopRight, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.backgroundColorBottomLeft, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.backgroundColorBottomRight, control, stateName);

	buildStyleForSinglePropertyWithState(style, style.backgroundImage, control, stateName);

	buildStyleForSinglePropertyWithState(style, style.borderWidth, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.borderTopWidth, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.borderBottomWidth, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.borderLeftWidth, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.borderRightWidth, control, stateName);
	
	buildStyleForSinglePropertyWithState(style, style.borderColor, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.borderTopColor, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.borderBottomColor, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.borderLeftColor, control, stateName);
	buildStyleForSinglePropertyWithState(style, style.borderRightColor, control, stateName);

	buildStyleForSinglePropertyWithState(style, style.visible, control, stateName);
}

} // namespace wf

CSP_NAMESPACE_END
