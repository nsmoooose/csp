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


#include "View.h"
#include "DynamicGrid.h"
#include "RegionSelect.h"
#include "FeatureGraph.h"
#include "PickHandler.h"
#include "ViewEventHandler.h"
#include "ViewManipulator.h"

#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif


#ifdef _MSC_VER
	#ifdef _DEBUG
		#undef _DEBUG
		#include <Python.h>
		#define _DEBUG
	#else
		#include <Python.h>
	#endif
#else
	#include <Python.h>
	#include <unistd.h>
#endif

#include <osgUtil/Optimizer>
#include <osgDB/ReadFile>
#include <osgProducer/Viewer>

#include <osg/Material>
#include <osg/Geode>
#include <osg/BlendFunc>
#include <osg/LineWidth>
#include <osg/Depth>
#include <osg/Projection>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/MatrixTransform>
#include <osg/PrimitiveSet>
#include <osg/LightSource>
#include <osg/Geometry>
#include <osg/Drawable>
#include <osg/ref_ptr>
#include <osg/Vec3>
#include <OpenThreads/Thread>

#include <iostream>
#include <cstdio>
#include <cmath>


View::View(): m_Viewer(0), m_FeatureGraph(0), m_Manipulator(0) {
	// TODO move elsewhere in startup logic
	OpenThreads::Thread::Init();
}

int View::init(int argc, char **argv) {
	// use an ArgumentParser object to manage the program arguments.
	// TODO argument parsing is now handled in python, so the argument parsing
	// and usage code below should be removed or updated at some point.
	osg::ArgumentParser arguments(&argc, argv);
	std::string description, usage;
	description = arguments.getApplicationName() + " is a utility for composing theater features.";
	usage = arguments.getApplicationName() + " [options]";
	arguments.getApplicationUsage()->setDescription(description);
	arguments.getApplicationUsage()->setCommandLineUsage(usage);
	arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");

	// construct the mvc components.
	m_FeatureGraph = new FeatureGraph;
	m_Viewer = new osgProducer::Viewer(arguments);
	m_Manipulator = new ViewManipulator(this);

	// set up the viewer and add our event handlers
	m_Viewer->setUpViewer(osgProducer::Viewer::HEAD_LIGHT_SOURCE);
	m_Viewer->getEventHandlerList().push_back(new ViewEventHandler(this, m_Viewer));
	m_Viewer->addCameraManipulator(m_Manipulator);

	// get details on keyboard and mouse bindings used by the viewer.
	m_Viewer->getUsage(*arguments.getApplicationUsage());

	// if user request help write it out to cout.
	if (arguments.read("-h") || arguments.read("--help")) {
		arguments.getApplicationUsage()->write(std::cout);
		return 1;
	}

	// any option left unread are converted into errors to write out later.
	arguments.reportRemainingOptionsAsUnrecognized();

	// report any errors if they have occured when parsing the program aguments.
	if (arguments.errors()) {
		arguments.writeErrorMessages(std::cout);
		return 1;
	}

	// add the handler for doing the picking
	m_Picker = new PickHandler(this);
	m_Viewer->getEventHandlerList().push_front(m_Picker);

	// add lights, ground, grid, etc. to the scene.
	prepareScene();

	//m_Viewer->setBackgroundColor(osg::Vec3(0.7,0.7,0.8));
	m_Manipulator->setHomePosition(osg::Vec3d(0,0,100), osg::Vec3d(0,0,0), osg::Vec3d(0,1,0), /*autoComputeHomePosition=*/false);

	// default to windowed
	Producer::CameraConfig* cfg = m_Viewer->getCameraConfig();
	for( unsigned int i = 0; i < cfg->getNumberOfCameras(); ++i ) {
		Producer::Camera *cam = cfg->getCamera(i);
		Producer::RenderSurface* rs = cam->getRenderSurface();
		if (rs->isFullScreen()) {
			#ifndef WIN32
			rs->useBorder(true);
			rs->setWindowRectangle(240,220,800,600);
			#else
			rs->fullScreen(false);
			rs->setWindowRectangle(240,220,800,600);
			#endif
		}
	}
	return 0;
}

View::~View() {
	if (m_Viewer) delete m_Viewer;
	if (m_Manipulator) delete m_Manipulator;
	if (m_Picker) delete m_Picker;
	if (m_FeatureGraph) delete m_FeatureGraph;
}

void View::run() {
	if (!m_Viewer) return;
	m_Quit = false;
#ifndef _MSC_VER
	timespec sleeptime;
	sleeptime.tv_sec = 0;
	sleeptime.tv_nsec = 1000000;
#endif
	m_Viewer->realize();
	while (!m_Viewer->done() && !m_Quit) {
		if (trylock() == 0) {
			m_Viewer->sync();
			m_Viewer->update();
			m_Viewer->frame();
			m_DynamicGrid->setLook(getCameraTarget(), getCameraPosition());
			unlock();
		}
		Py_BEGIN_ALLOW_THREADS;
		OpenThreads::Thread::YieldCurrentThread();
		Py_END_ALLOW_THREADS;
	}

	// XXX
	// Intentionally leak Viewer to prevent an xlib error on exit:
	//
	//   Xlib: unexpected async reply (sequence 0x2e)!
	//
	// More likely than not we are only masking the problem, which
	// probably results from asynchronous xlib access from the 2d
	// and 3d threads.  This hasn't caused any other noticible problems
	// and by not deleting the viewer we get a clean exit.
	//
	//delete m_Viewer;
	m_Viewer = 0;
}

void View::quit() {
	if (m_Viewer) {
		m_Viewer->setDone(true);
	}
	m_Quit = true;  // needed?
}

void View::setCamera(float x0, float y0, float z0, float x1, float y1, float z1) {
	Producer::Vec3 from(x0, y0, z0);
	Producer::Vec3 to(x1, y1, z1);
	Producer::Vec3 up = (to-from)^((to-from)^Producer::Vec3(0,0,-1));
	if (up.length() < 1e-3) {
		up = (to-from)^((to-from)^Producer::Vec3(0,-1,0));
	}
	Producer::Matrix view_matrix;
	view_matrix.makeLookAt(from ,to, up);
	assert(m_Viewer);
	m_Viewer->setViewByMatrix(view_matrix);
}

void View::setViewAxis(osg::Vec3 const &axis, osg::Vec3 const &up) {
	osg::Vec3 eye = getCameraPosition();
	osg::Vec3 target = getCameraTarget();
	float d = getCameraDistance();
	eye = target + axis * (((target-eye)*axis < 0) ? d : - d);
	setCamera(eye.x(), eye.y(), eye.z(), target.x(), target.y(), target.z());
}

void View::setViewX() {
	setViewAxis(osg::Vec3(1,0,0), osg::Vec3(0,0,1));
}

void View::setViewY() {
	setViewAxis(osg::Vec3(0,1,0), osg::Vec3(0,0,1));
}

void View::setViewZ() {
	//setViewAxis(osg::Vec3(0,0,-1), osg::Vec3(0,1,0));
	osg::Vec3 up = osg::Vec3(0,1,0);
	osg::Vec3 eye = getCameraPosition();
	osg::Vec3 target = getCameraTarget();
	float d = getCameraDistance();
	eye = target + osg::Vec3(0,0,d);
	//m_Viewer->setViewByLookat(eye.x(), eye.y(), eye.z(), target.x(), target.y(), target.z(), up.x(), up.y(), up.z());
	setCamera(eye.x(), eye.y(), eye.z(), target.x(), target.y(), target.z());
}

osg::Vec3 View::getCameraPosition() const {
	assert(m_Viewer);
	const Producer::Matrix::value_type *view_matrix = m_Viewer->getCamera(0)->getPositionAndAttitudeMatrix();
	Producer::Matrix matrix(view_matrix);
	Producer::Vec3 offset(matrix(3, 0), matrix(3, 1), matrix(3, 2));
	matrix.invert(matrix);
	matrix(3, 0) = 0.0;
	matrix(3, 1) = 0.0;
	matrix(3, 2) = 0.0;
	Producer::Vec3 eye = -offset * matrix;
	return osg::Vec3(eye[0], eye[1], eye[2]);
}

osg::Vec3 View::getCameraTarget() const {
	return m_Manipulator->center();
}

float View::getCameraDistance() const {
	return m_Manipulator->distance();
}

void View::centerViewOnNode(LayoutNodePath *path, LayoutNode *node) {
	m_Manipulator->centerViewOnNode(path, node);
}

void View::screenToSurface(float x, float y, float &surface_x, float &surface_y) {
	surface_x = surface_y = 0.0;
	float pixel_x, pixel_y;
	assert(m_Viewer);
	if (m_Viewer->computePixelCoords(x, y, 0 /*camera_num*/, pixel_x, pixel_y)) {
		Producer::Camera *camera = m_Viewer->getCamera(0);
		int pr_wx, pr_wy;
		unsigned pr_width, pr_height;
		camera->getProjectionRectangle(pr_wx, pr_wy, pr_width, pr_height);

		// convert into clip coords.
		float rx = 2.0f*(pixel_x - (float)pr_wx)/(float)pr_width-1.0f;
		float ry = 2.0f*(pixel_y - (float)pr_wy)/(float)pr_height-1.0f;

		osgProducer::OsgSceneHandler* sh = dynamic_cast<osgProducer::OsgSceneHandler*>(camera->getSceneHandler());
		osgUtil::SceneView* sv = sh?sh->getSceneView():0;
		osg::Matrixd vum;
		if (sv != 0) {
			vum.set(sv->getViewMatrix() * sv->getProjectionMatrix());
		} else {
			vum.set(osg::Matrixd(camera->getViewMatrix()) * osg::Matrixd(camera->getProjectionMatrix()));
		}
		osg::Matrixd inverseMVPW;
		inverseMVPW.invert(vum);
		osg::Vec3 near_ = osg::Vec3(rx, ry, -1.0) * inverseMVPW;
		osg::Vec3 far_ = osg::Vec3(rx, ry, 1.0) * inverseMVPW;
		osg::Vec3 ray = near_ - far_;
		if (ray.z() != 0) {
			surface_x = near_.x() - near_.z() * (ray.x() / ray.z());
			surface_y = near_.y() - near_.z() * (ray.y() / ray.z());
		}
	}
}

bool View::computePixelCoords(float x, float y, float& pixel_x, float& pixel_y) {
	assert(m_Viewer);
	return m_Viewer->computePixelCoords(x, y, 0 /*camera_num*/, pixel_x, pixel_y);
}

void View::updateMouseCoordinates(float xnorm, float ynorm) {
	screenToSurface(xnorm, ynorm, m_MouseX, m_MouseY);
}

void View::getMouseCoordinates(float &x, float &y) {
	x = m_MouseX;
	y = m_MouseY;
}

void View::setMoveMode() {
	m_Manipulator->setMoveMode();
}

void View::setRotateMode() {
	m_Manipulator->setRotateMode();
}

CallbackId View::addCallback(ViewCallback *callback) {
	assert(callback);
	m_ViewCallbacks.push_back(callback);
	return reinterpret_cast<CallbackId>(callback);
}

void View::removeCallback(CallbackId id) {
	for (ViewCallbackList::iterator iter = m_ViewCallbacks.begin(); iter != m_ViewCallbacks.end(); ++iter) {
		if ((*iter) == reinterpret_cast<ViewCallback*>(id)) {
			m_ViewCallbacks.erase(iter);
			return;
		}
	}
	assert(id == 0);
}

void View::signalEditMode() {
	ViewManipulator::Mode mode = m_Manipulator->getMode();
	for (ViewCallbackList::iterator iter = m_ViewCallbacks.begin(); iter != m_ViewCallbacks.end(); ++iter) {
		switch (mode) {
			case ViewManipulator::MOVE:
				(*iter)->onMoveMode();
				break;
			case ViewManipulator::ROTATE:
				(*iter)->onRotateMode();
				break;
			default:
				break;
		}
	}
}

void View::updatePosition(float norm_x, float norm_y, float position_x, float position_y, float dx, float dy, float angle) {
	float x, y;
	screenToSurface(norm_x, norm_y, x, y);
	for (ViewCallbackList::iterator iter = m_ViewCallbacks.begin(); iter != m_ViewCallbacks.end(); ++iter) {
		(*iter)->onUpdatePosition(x, y, position_x, position_x, dx, dy, angle);
	}
}

void View::sendKey(std::string const &key) {
	for (ViewCallbackList::iterator iter = m_ViewCallbacks.begin(); iter != m_ViewCallbacks.end(); ++iter) {
		(*iter)->onKey(key);
	}
}

void View::prepareScene() {
	assert(m_Viewer);
	osg::Group *group = new osg::Group;
	makeLights(group);
	makeGround(group);
	m_DynamicGrid = new DynamicGrid();
	m_RegionSelect = new RegionSelect();
	group->addChild(m_RegionSelect.get());
	group->addChild(m_DynamicGrid.get());
	group->addChild(m_FeatureGraph->getScene().get());
	m_Viewer->setSceneData(group);
}

void View::makeLights(osg::Group *group) {
	// add an overhead light
	osg::LightSource *light = new osg::LightSource();
	light->setLight(new osg::Light());
	light->getLight()->setDiffuse(osg::Vec4(1,1,1,1));
	light->getLight()->setPosition(osg::Vec4(0,0,1,0));
	light->getLight()->setDirection(osg::Vec3(0,0,-1));
	light->getLight()->setLightNum(3);
	light->setLocalStateSetModes(osg::StateAttribute::ON);
	light->setStateSetModes(*(m_Viewer->getGlobalStateSet()), osg::StateAttribute::ON);
	group->addChild(light);
}

void View::makeGround(osg::Group *group) {
	osg::Geometry *ground = new osg::Geometry();
	osg::Vec3Array *ground_v = new osg::Vec3Array();
	ground_v->push_back(osg::Vec3(-1e+4, -1e+4, -0.01));
	ground_v->push_back(osg::Vec3(-1e+4, 1e+4, -0.01));
	ground_v->push_back(osg::Vec3(1e+4, 1e+4, -0.01));
	ground_v->push_back(osg::Vec3(1e+4, -1e+4, -0.01));
	ground->setVertexArray(ground_v);
	osg::Vec3Array* color_array = new osg::Vec3Array;
	color_array->push_back(osg::Vec3(1,1,1));
	ground->setColorArray(color_array);
	ground->setColorBinding(osg::Geometry::BIND_OVERALL);
	osg::DrawElementsUShort *ground_p = new osg::DrawElementsUShort(osg::PrimitiveSet::QUADS);
	ground_p->push_back(0);
	ground_p->push_back(1);
	ground_p->push_back(2);
	ground_p->push_back(3);
	ground->addPrimitiveSet(ground_p);
	osg::Geode *ground_g = new osg::Geode();
	ground_g->addDrawable(ground);
	osg::Material *ground_m = new osg::Material();
	ground_m->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.6,0.8,0.6,1.0));
	ground_m->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0,0.0,1.0));
	ground_m->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0,0.0,1.0));
	ground_g->getOrCreateStateSet()->setAttributeAndModes(ground_m, osg::StateAttribute::ON);
	ground_g->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonOffset(1.0, 1.0), osg::StateAttribute::ON);
	ground_g->getOrCreateStateSet()->setRenderBinDetails(-3, "RenderBin");
	group->addChild(ground_g);
}

void View::deleteSelectedNodes() {
	LayoutGroup *active_group = m_FeatureGraph->getActiveGroup();
	if (active_group) {
		LayoutGroup::ChildList selected = active_group->getSelectedChildren();
		if (!selected.empty()) {
			std::string label = "delete feature";
			if (selected.size() > 1) label += 's';
			m_FeatureGraph->runCommand(new InsertDeleteCommand(active_group, selected, false, label));
		}
	}
}

float View::getGridScale() const {
	return m_DynamicGrid->getGridScale();
}

void View::updateRegionSelection(float x0, float y0, float x1, float y1) {
	m_RegionSelect->show();
	m_RegionSelect->update(x0, y0, x1, y1);
}

void View::finishRegionSelection(bool /*cancel*/) {
	m_RegionSelect->hide();
}
