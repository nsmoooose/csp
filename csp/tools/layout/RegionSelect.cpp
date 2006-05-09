// CSPLayout
// Copyright 2005 Mark Rose <mkrose@users.sourceforge.net>
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, US


#include "RegionSelect.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineStipple>
#include <osg/MatrixTransform>
#include <osg/PrimitiveSet>
#include <osg/Projection>

#include <cmath>
#include <cassert>
#include <iostream>

RegionSelect::RegionSelect() {
	osg::Projection *projection = new osg::Projection;
	projection->setMatrix(osg::Matrix::ortho2D(-1, 1, -1, 1));
	osg::MatrixTransform *model_view_abs = new osg::MatrixTransform;
	model_view_abs->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	model_view_abs->setMatrix(osg::Matrix::identity());

	model_view_abs->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	model_view_abs->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	model_view_abs->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	model_view_abs->getOrCreateStateSet()->setRenderBinDetails(11, "RenderBin");

	_vertex_array = new osg::Vec3Array(4);
	(*_vertex_array)[0].set(0.0, 0.0, 0.0);
	(*_vertex_array)[1].set(0.0, 0.0, 0.0);
	(*_vertex_array)[2].set(0.0, 0.0, 0.0);
	(*_vertex_array)[3].set(0.0, 0.0, 0.0);
	osg::Geode *geode = new osg::Geode();
	_box = new osg::Geometry();
	_box->setVertexArray(_vertex_array.get());

	osg::Vec4Array *color_array = new osg::Vec4Array;
	color_array->push_back(osg::Vec4(1.0, 1.0, 1.0, 0.5));
	_box->setColorArray(color_array);
	_box->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::Vec3Array *normal_array = new osg::Vec3Array;
	normal_array->push_back(osg::Vec3(0.0, -1.0, 0.0));
	_box->setNormalArray(normal_array);
	_box->setNormalBinding(osg::Geometry::BIND_OVERALL);

	_box->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 4));
	addChild(projection);
	projection->addChild(model_view_abs);
	model_view_abs->addChild(geode);
	geode->addDrawable(_box.get());
	osg::LineStipple *stipple = new osg::LineStipple;
	stipple->setPattern(0x5555);
	geode->getOrCreateStateSet()->setAttributeAndModes(stipple);
}

void RegionSelect::hide() {
	setNodeMask(0);
}

void RegionSelect::show() {
	setNodeMask(~0);
}

void RegionSelect::update(float x0, float y0, float x1, float y1) {
	assert(_vertex_array.valid());
	(*_vertex_array)[0].set(x0, y0, -1.0);
	(*_vertex_array)[1].set(x1, y0, -1.0);
	(*_vertex_array)[2].set(x1, y1, -1.0);
	(*_vertex_array)[3].set(x0, y1, -1.0);
	_box->setVertexArray(_vertex_array.get());
}


