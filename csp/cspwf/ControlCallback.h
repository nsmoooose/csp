#pragma once
// Combat Simulator Project
// Copyright (C) 2002-2005 The Combat Simulator Project
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
 * @file ControlCallback.h
 *
 **/

#include <osg/NodeCallback>
#include <csp/csplib/util/Ref.h>

namespace csp {
namespace wf {

class Control;

/** Callback attached to controls for hit detection. Used
 * to identify the control that is clicked upon. When clicking is
 * performed the window manager will try to identify the update
 * callback and then call the getControl() method to retrieve
 * the control.
 */
class ControlCallback: public osg::NodeCallback {
public:
	ControlCallback(Control* control);

	/** Returns the control assigned to this callback.
	 */
	Control* getControl();

private:
	Control* m_Control;
};

} // namespace wf
} // namespace csp
