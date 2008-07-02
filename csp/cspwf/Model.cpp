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
 * @file Model.cpp
 *
 **/

#include <csp/cspwf/ControlGeometryBuilder.h>
#include <csp/cspwf/Model.h>
#include <osg/Group>

namespace csp {

namespace wf {

Model::Model() : Control("Model"), m_Scale(1.0) {
}

Model::~Model() {
}

std::string Model::getModelFilePath() const {
	return m_FilePath;
}

void Model::setModelFilePath(std::string& filePath) {
	m_FilePath = filePath;
}

double Model::getScale() const {
	return m_Scale;
}

void Model::setScale(double scale) {
	m_Scale = scale;
}

void Model::buildGeometry() {
	// Make sure that all our child controls onInit() is called.
	Control::buildGeometry();	
	
	ControlGeometryBuilder geometryBuilder;
	osg::ref_ptr<osg::Group> model = geometryBuilder.buildModel(this);
	getNode()->addChild(model.get());
}

} // namespace wf

} // namespace csp
