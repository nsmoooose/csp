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
 * @file StyleBuilder.h
 *
 **/

#ifndef __CSPSIM_WF_STYLEBUILDER_H__
#define __CSPSIM_WF_STYLEBUILDER_H__

#include <csp/csplib/util/Ref.h>

namespace csp {

namespace wf {

class Control;
class Style;

/** Class that is building up the style to use for a specific control.
 * It is acheived by checking inherited styles, styles set per control
 * or a named cssClass. 
 */
class StyleBuilder {
public:
	/** Builds a style object for the control sent as a parameter. The entire
	 * control hierchy is checked (style properties can be inherited from parent
	 * control or a named class). 
	 */
	static Ref<Style> buildStyle(const Control* control);
	
	/** Method used to build a style with an object that is in a particilar state.
	 * style = The original state style set on the control.
	 * control = The control that we are building style from.
	 * stateName = The name of the state. Eg: selected, disabled, hover etc
	 */
	static void buildStyle(Style* style, const Control* control, const std::string& stateName);
};

} // namespace wf

} // namespace csp

#endif // __CSPSIM_WF_STYLEBUILDER_H__
