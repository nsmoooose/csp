// Combat Simulator Project
// Copyright (C) 2008 The Combat Simulator Project
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
 * @file WindowManagerViewer.cpp
 *
 **/

#include <csp/cspwf/WindowManagerViewer.h>

#include <osg/Group>

namespace csp {
namespace wf {

WindowManagerViewer::WindowManagerViewer() {
	m_Group = new osg::Group();
}

WindowManagerViewer::~WindowManagerViewer() {
}

osg::ref_ptr<osg::Group> WindowManagerViewer::getRootNode() {
	return m_Group;
}

Control* WindowManagerViewer::getControlAtPosition(int /*x*/, int /*y*/) {
	return NULL;
}

} // namespace wf
} // namespace csp
