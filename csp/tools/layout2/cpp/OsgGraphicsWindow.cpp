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

#include <iostream>
#include <sstream>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <csp/tools/layout2/cpp/OsgGraphicsWindow.h>

namespace csp {
namespace layout {

class OsgGraphicsWindow::Implementation : public osgViewer::GraphicsWindow {
public:
	OsgGraphicsWindow* m_Window;
	osg::ref_ptr<osgViewer::Viewer> m_Viewer;
	osg::ref_ptr<osgGA::TrackballManipulator> m_Manipulator;

	void init(OsgGraphicsWindow* window) {
		m_Window = window;

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

OsgGraphicsWindow::OsgGraphicsWindow() : m_Implementation(new Implementation()) {
	m_Implementation->init(this);
}

OsgGraphicsWindow::~OsgGraphicsWindow() {
	delete m_Implementation;
}

osg::Vec3 OsgGraphicsWindow::getCameraPosition() const {
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

void OsgGraphicsWindow::Frame() {
	m_Implementation->m_Viewer->frame();
}

void OsgGraphicsWindow::setSize(int width, int height) {
	// update the window dimensions, in case the window has been resized.
	m_Implementation->getEventQueue()->windowResize(0, 0, width, height);
	m_Implementation->resized(0, 0, width, height);
}

void OsgGraphicsWindow::handleKeyDown(int key) {
	m_Implementation->getEventQueue()->keyPress(key);
}

void OsgGraphicsWindow::handleKeyUp(int key) {
	m_Implementation->getEventQueue()->keyRelease(key);
}

void OsgGraphicsWindow::handleMouseMotion(int x, int y) {
	m_Implementation->getEventQueue()->mouseMotion(x, y);
}

void OsgGraphicsWindow::handleMouseButtonDown(int x, int y, int button) {
	m_Implementation->getEventQueue()->mouseButtonPress(x, y, button);
}

void OsgGraphicsWindow::handleMouseButtonUp(int x, int y, int button) {
	m_Implementation->getEventQueue()->mouseButtonRelease(x, y, button);
}

std::string OsgGraphicsWindow::getTrackballInformation() {
	osgGA::TrackballManipulator* manipulator = dynamic_cast<osgGA::TrackballManipulator*>(m_Implementation->m_Viewer->getCameraManipulator());

	std::stringstream stream;
	osg::Vec3 center = manipulator->getCenter();
	osg::Quat rotation = manipulator->getRotation();
	stream << "Center: " << center._v[0] << ", " << center._v[1] << ", " << center._v[2];
	stream << " Distance: " << manipulator->getDistance();
	stream << " Rotation: " << rotation._v[0] << ", " << rotation._v[1] << ", " << rotation._v[2] << ", " << rotation._v[3];
	stream << std::ends;
	
	return stream.str();
}

osg::ref_ptr<osgGA::TrackballManipulator> OsgGraphicsWindow::getManipulator() {
	return m_Implementation->m_Manipulator;
}

osg::ref_ptr<osg::Node> OsgGraphicsWindow::getSceneData() {
	return m_Implementation->m_Viewer->getSceneData();
}

void OsgGraphicsWindow::setSceneData(osg::Node* node) {
	m_Implementation->m_Viewer->setSceneData(node);
}

} // namespace layout
} // namespace csp
