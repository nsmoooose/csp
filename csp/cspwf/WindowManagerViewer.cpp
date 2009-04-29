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

#include <csp/cspwf/ControlCallback.h>
#include <csp/cspwf/WindowManagerViewer.h>

#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include <osg/Camera>
#include <osg/Group>
#include <osg/Group>
#include <osg/LightModel>
#include <osg/MatrixTransform>
#include <osg/TexEnv>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>

#include <iostream>

namespace csp {
namespace wf {

WindowManagerViewer::WindowManagerViewer(int width, int height) : WindowManager(width, height) {
	Size screenSize;
	float screenScale;
	calculateScreenSizeAndScale(screenSize, screenScale);
	if(screenScale != 1.0) {
		osg::ref_ptr<osg::MatrixTransform> scaledGroup = new osg::MatrixTransform;
		osg::Matrix scale;
		scale.makeScale(osg::Vec3(screenScale, screenScale, screenScale));
		scaledGroup->setMatrix(scale);
		m_Group = scaledGroup.get();
	}

/*
  Some information from osghud example:

  All you need to do is create your nodes in a subgraph. Then place an osg::Camera 
  above the subgraph to create an orthographic projection. Set the Camera's 
  ReferenceFrame to ABSOLUTE_RF to ensure it remains independent from any external 
  model view matrices. And set the Camera's clear mask to just clear the depth buffer.
  And finally set the Camera's RenderOrder to POST_RENDER to make sure its drawn last.
*/

    m_Camera = new osg::Camera;
    m_Camera->setProjectionMatrixAsOrtho(0, width, height, 0, -1000, 1000);
    m_Camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    m_Camera->setViewMatrix(osg::Matrix::identity());
    m_Camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    m_Camera->setRenderOrder(osg::Camera::POST_RENDER);

	osg::Matrix view_matrix;
	view_matrix.makeLookAt(osg::Vec3(0, 0, 100.0), osg::Vec3(0.0, 0.0, 0.0), osg::Vec3(0, 1, 0));
	m_Camera->setViewMatrix(view_matrix);

    // we don't want the camera to grab event focus from the viewers main camera(s).
    m_Camera->setAllowEventFocus(false);
	m_Camera->addChild(m_Group.get());

	osg::ref_ptr<osg::StateSet> globalStateSet = new osg::StateSet;
	globalStateSet->setGlobalDefaults();
	globalStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	globalStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	globalStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	// create a light model to eliminate the default ambient light.
	osg::ref_ptr<osg::LightModel> light_model = new osg::LightModel();
	light_model->setAmbientIntensity(osg::Vec4(0.0, 0.0, 0.0, 1.0));
	globalStateSet->setAttributeAndModes(light_model.get(), osg::StateAttribute::ON);

	// set up an alphafunc by default to speed up blending operations.
	osg::ref_ptr<osg::AlphaFunc> alphafunc = new osg::AlphaFunc;
	alphafunc->setFunction(osg::AlphaFunc::GREATER, 0.0f);
	globalStateSet->setAttributeAndModes(alphafunc.get(), osg::StateAttribute::ON);

	// set up an texture environment by default to speed up blending operations.
	osg::ref_ptr<osg::TexEnv> texenv = new osg::TexEnv;
	texenv->setMode(osg::TexEnv::MODULATE);
	globalStateSet->setTextureAttributeAndModes(0, texenv.get(), osg::StateAttribute::ON);

    osg::ref_ptr<osg::BlendFunc> blendFunction = new osg::BlendFunc;
    globalStateSet->setAttributeAndModes(blendFunction.get());
	m_Camera->setStateSet(globalStateSet.get());
}

WindowManagerViewer::~WindowManagerViewer() {
}

osg::ref_ptr<osg::Group> WindowManagerViewer::getRootNode() {
	return m_Camera.get();
}

Control* WindowManagerViewer::getControlAtPosition(int x, int y) {
	osg::Vec3d start = osg::Vec3d(x, y, 1000);
	osg::Vec3d end = osg::Vec3d(x, y, -1000);

	osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(start, end);
	osgUtil::IntersectionVisitor intersectVisitor(intersector.get());

	m_Camera->accept(intersectVisitor);

	if ( intersector->containsIntersections() )
	{
		osgUtil::LineSegmentIntersector::Intersections& intersections = intersector->getIntersections();
		for(osgUtil::LineSegmentIntersector::Intersections::iterator itr = intersections.begin();
			itr != intersections.end();
			++itr)
		{
			const osgUtil::LineSegmentIntersector::Intersection& intersection = *itr;
			for (osg::NodePath::const_reverse_iterator iter = intersection.nodePath.rbegin(); iter != intersection.nodePath.rend(); ++iter) {
				osg::Node *node = *iter;
				osg::NodeCallback *callback = node->getUpdateCallback();
				if (callback) {
					ControlCallback *controlCallback = dynamic_cast<ControlCallback*>(callback);
					if (controlCallback) {
						return controlCallback->getControl();
					}
				}
			}
		}
	}
	return NULL;
}

} // namespace wf
} // namespace csp

