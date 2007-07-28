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
 * @file Check.cpp
 *
 **/

#include <csp/cspsim/wf/Check.h>
#include <csp/cspsim/wf/ControlGeometryBuilder.h>

#include <osg/Group>

CSP_NAMESPACE

namespace wf {

Check::Check() {
}

Check::~Check() {
}

std::string Check::getName() const {
	return "Check";
}

void Check::buildGeometry() {
	Control::buildGeometry();

	// Build our own check control and add it to the group.
	ControlGeometryBuilder geometryBuilder;
	osg::ref_ptr<osg::Group> check = geometryBuilder.buildCheck(this);

	// When the control is invisible there will not be any geometry created.
	if(check.valid()) {
		getNode()->addChild(check.get());		
	}
}

} // namespace wf

CSP_NAMESPACE_END
