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

#include <strstream>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <csp/tools/layout2/cpp/OsgGraphicsWindow.h>

class csp::layout::OsgGraphicsWindow::Implementation : public osgViewer::GraphicsWindow {
public:
	OsgGraphicsWindow* m_Window;
	osg::ref_ptr<osgViewer::Viewer> m_Viewer;
	osg::ref_ptr<osgGA::TrackballManipulator> m_Manipulator;
	Ref<Scene> m_Scene;

	void init(OsgGraphicsWindow* window, Scene* scene) {
		m_Window = window;
		m_Scene = scene;

		_traits = new GraphicsContext::Traits;
		_traits->x = 50;
		_traits->y = 70;
		_traits->width = 300;
		_traits->height = 200;

		setState( new osg::State );
		getState()->setGraphicsContext(this);

		if (_traits.valid() && _traits->sharedContext)
		{
			getState()->setContextID( _traits->sharedContext->getState()->getContextID() );
			incrementContextIDUsageCount( getState()->getContextID() );   
		}
		else
		{
			getState()->setContextID( osg::GraphicsContext::createNewContextID() );
		}

		m_Viewer = new osgViewer::Viewer;
		m_Viewer->getCamera()->setGraphicsContext(this);
		m_Viewer->getCamera()->setViewport(0,0,300,300);
		m_Viewer->addEventHandler(new osgViewer::StatsHandler);
		m_Viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
		m_Viewer->setSceneData(m_Scene->getRootNode().get());

		m_Manipulator = new osgGA::TrackballManipulator;
		m_Viewer->setCameraManipulator(m_Manipulator.get());

		m_Manipulator->setCenter(osg::Vec3(0, 0, 0));
		m_Manipulator->setDistance(700);
		m_Manipulator->setRotation(osg::Quat(0, 0, 0, 1));
	}

	virtual bool makeCurrentImplementation() {
		m_Window->SetCurrent();
		return true;
	}

	virtual void swapBuffersImplementation() {
		m_Window->SwapBuffers();
	}

	virtual bool valid() const { 
		return true; 
	}

	virtual bool realizeImplementation() { 
		return true; 
	}
	
    virtual bool isRealizedImplementation() const  { 
		return true; 
	}
	
	virtual void closeImplementation() {
	}

	virtual bool releaseContextImplementation() { 
		return true; 
	}
};

csp::layout::OsgGraphicsWindow::OsgGraphicsWindow() : m_Implementation(new Implementation()) {
	m_Implementation->init(this, new Scene);
}

csp::layout::OsgGraphicsWindow::~OsgGraphicsWindow() {
	delete m_Implementation;
}

osg::Vec3 csp::layout::OsgGraphicsWindow::getCameraPosition() const {
	const osg::Matrix view_matrix = m_Implementation->m_Viewer->getCamera()->getViewMatrix();
	osg::Matrix matrix(view_matrix);
	osg::Vec3 offset(matrix(3, 0), matrix(3, 1), matrix(3, 2));
	matrix.invert(matrix);
	matrix(3, 0) = 0.0;
	matrix(3, 1) = 0.0;
	matrix(3, 2) = 0.0;
	osg::Vec3 eye = -offset * matrix;
	return osg::Vec3(eye[0], eye[1], eye[2]);
}

void csp::layout::OsgGraphicsWindow::Frame() {
	osgGA::TrackballManipulator* manipulator = dynamic_cast<osgGA::TrackballManipulator*>(m_Implementation->m_Viewer->getCameraManipulator());
	 osg::Vec3 cameraTarget = manipulator->getCenter();
	m_Implementation->m_Scene->updateDynamicGrid(cameraTarget, getCameraPosition());
	m_Implementation->m_Viewer->frame();
}

void csp::layout::OsgGraphicsWindow::setSize(int width, int height) {
	// update the window dimensions, in case the window has been resized.
	m_Implementation->getEventQueue()->windowResize(0, 0, width, height);
	m_Implementation->resized(0, 0, width, height);
}

void csp::layout::OsgGraphicsWindow::handleKeyDown(int key) {
	m_Implementation->getEventQueue()->keyPress(key);
}

void csp::layout::OsgGraphicsWindow::handleKeyUp(int key) {
	m_Implementation->getEventQueue()->keyRelease(key);
}

void csp::layout::OsgGraphicsWindow::handleMouseMotion(int x, int y) {
	m_Implementation->getEventQueue()->mouseMotion(x, y);
}

void csp::layout::OsgGraphicsWindow::handleMouseButtonDown(int x, int y, int button) {
	m_Implementation->getEventQueue()->mouseButtonPress(x, y, button);
}

void csp::layout::OsgGraphicsWindow::handleMouseButtonUp(int x, int y, int button) {
	m_Implementation->getEventQueue()->mouseButtonRelease(x, y, button);
}

std::string csp::layout::OsgGraphicsWindow::getTrackballInformation() {
	osgGA::TrackballManipulator* manipulator = dynamic_cast<osgGA::TrackballManipulator*>(m_Implementation->m_Viewer->getCameraManipulator());

	std::strstream stream;
	osg::Vec3 center = manipulator->getCenter();
	osg::Quat rotation = manipulator->getRotation();
	stream << "Center: " << center._v[0] << ", " << center._v[1] << ", " << center._v[2];
	stream << " Distance: " << manipulator->getDistance();
	stream << " Rotation: " << rotation._v[0] << ", " << rotation._v[1] << ", " << rotation._v[2] << ", " << rotation._v[3];
	stream << std::ends;
	return stream.str();
}

void csp::layout::OsgGraphicsWindow::moveCameraToHome() {
	m_Implementation->m_Manipulator->setCenter(osg::Vec3(0, 0, 0));
	m_Implementation->m_Manipulator->setDistance(700);
	m_Implementation->m_Manipulator->setRotation(osg::Quat(0, 0, 0, 1));
}

csp::layout::FeatureGraph* csp::layout::OsgGraphicsWindow::graph() {
	return m_Implementation->m_Scene->graph();
}