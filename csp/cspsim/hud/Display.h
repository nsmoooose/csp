#pragma once
// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file Display.h
 *
 **/

#include <csp/csplib/util/Referenced.h>
#include <osg/ref_ptr>

namespace osg { class Node; }
namespace osg { class Group; }
namespace osg { class ClipNode; }

namespace csp {
namespace hud {

/** Base class for vehicle displays that support text, ploting, and video
 *  rendering.  This class simply provides a root node for the display
 *  elements and support for clipping to the display surface.  Subclasses
 *  implement specialized behavior, such as persective transforms for heads
 *  up displays.  Actual rendering is performed by various helper classes,
 *  such as SymbolMaker and Element; see DisplayTools.h.  Note that Display
 *  and its helper classes only provide a rendering interface; detailed
 *  instrument logic should be implemented in System subclasses.
 */
class Display {
public:
	/** Construct a new Display.
	 */
	Display();
	virtual ~Display();

	/** Allow direct copies to facilitate use with DataChannel.  Define
	 *  explicit copy functions to allow ClipNode to be forward declared.
	 */
	Display(Display const &other);
	Display const &operator=(Display const &other);

	/** Get the 3D model of the display.  Elements should be can be added
	 *  directly to this group, although subclasses may provide specialized
	 *  methods for modifying the display.
	 */
	osg::Group *model();

	/** Set the dimension (width, height) of the display in meters.  Elements outside
	 *  of these bounds will be clipped.
	 */
	virtual void setDimensions(float x, float y);

private:

	// CAUTION: update copy operators whenever new member variables are added.
	osg::ref_ptr<osg::ClipNode> m_Root;
};

} // end namespace hud
} // end namespace csp
