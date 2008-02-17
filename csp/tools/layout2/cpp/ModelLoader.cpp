// CSPLayout
// Copyright 2003-2005 Mark Rose <mkrose@users.sourceforge.net>
//
// Based in part on osgpick sample code
// OpenSceneGraph - Copyright (C) 1998-2003 Robert Osfield
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
#include <csp/csplib/data/Object.h>
#include <csp/csplib/util/Log.h>
#include <csp/cspsim/theater/FeatureModel.h>
#include <csp/cspsim/theater/FeatureSceneGroup.h>
#include <csp/cspsim/theater/ElevationCorrection.h>
#include <csp/cspsim/theater/LayoutTransform.h>
#include <csp/cspsim/Animation.h>
#include <csp/cspsim/ObjectModel.h>

#include <csp/tools/layout2/cpp/Handle.h>
#include <csp/tools/layout2/cpp/ModelLoader.h>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <iostream>

using csp::FeatureModel;
using csp::FeatureSceneGroup;
using csp::LayoutTransform;
using csp::ElevationCorrection;

osg::ref_ptr<osg::Node> csp::layout::getFeatureModel(csp::Object *object) {
	csp::Ref<csp::Object> objref(object);
	csp::Ref<csp::FeatureModel> model(objref);
	assert(model.valid());
	osg::ref_ptr<FeatureSceneGroup> group = new FeatureSceneGroup;
	LayoutTransform transform;
	ElevationCorrection correction(0);
	model->addSceneModel(group.get(), transform, correction);
	group->addChild(Handle::makeHandle(osg::Vec4(0, 1, 0, 1)));
	return group.get();
}

osg::ref_ptr<osg::Node> csp::layout::getObjectModel(csp::Object *object) {
	csp::Ref<csp::Object> objref(object);
	csp::Ref<csp::ObjectModel> model(objref);
	assert(model.valid());
	return model->getModel();
}
