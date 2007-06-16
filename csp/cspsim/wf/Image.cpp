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
 * @file Image.cpp
 *
 **/

#include <csp/cspsim/wf/ControlGeometryBuilder.h>
#include <csp/cspsim/wf/Image.h>
#include <osg/Group>

CSP_NAMESPACE

namespace wf {

Image::Image() {
}

Image::~Image() {
}

std::string Image::getName() const {
	return "Image";
}

void Image::buildGeometry() {
	// Make sure that all our child controls onInit() is called.
	Control::buildGeometry();	
	
	ControlGeometryBuilder geometryBuilder;
	osg::ref_ptr<osg::Group> image = geometryBuilder.buildImage(this);
	getNode()->addChild(image.get());
}

} // namespace wf

CSP_NAMESPACE_END
