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

#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>
#include <csp/tools/layout2/cpp/OsgGraphicsWindow.h>

namespace csp {
namespace layout {

class WxGraphicsWindowEmbedded : public osgViewer::GraphicsWindowEmbedded {
public:
	WxGraphicsWindowEmbedded(OsgGraphicsWindow* window, int x, int y, int width, int height)
		: osgViewer::GraphicsWindowEmbedded(x, y, width, height)
		, m_Window(window) {
	}

	virtual bool makeCurrentImplementation() {
		m_Window->SetCurrent();
		return true;
	}

	virtual void swapBuffersImplementation() {
		m_Window->SwapBuffers();
	}

protected:
	OsgGraphicsWindow* m_Window;
};

class OsgGraphicsWindow::Implementation {
public:
	osg::ref_ptr<osgViewer::Viewer> m_Viewer;
	osg::ref_ptr<osgViewer::GraphicsWindow> m_GraphicsWindow;
	ProjectionKind m_projectionKind;

	void init(OsgGraphicsWindow* window, ProjectionKind projectionKind, osg::ref_ptr<osgGA::CameraManipulator> manipulator) {
		m_Viewer = new osgViewer::Viewer;
		m_projectionKind = projectionKind;
		setUpViewerAsEmbeddedInWxWindow(window, 50, 70, 300, 200);

		m_Viewer->addEventHandler(new osgViewer::StatsHandler);

		m_Viewer->setCameraManipulator(manipulator.get());
	}

	bool setUpViewerAsEmbeddedInWxWindow(OsgGraphicsWindow* window, int x, int y, int width, int height) {
		// inspired by osgViewer::Viewer::setUpViewerAsEmbeddedInWindow()
		m_Viewer->setThreadingModel( osgViewer::Viewer::SingleThreaded );
		m_GraphicsWindow = new WxGraphicsWindowEmbedded(window, x, y, width, height);
		m_Viewer->getCamera()->setViewport( new osg::Viewport(0, 0, width, height) );

		switch ( m_projectionKind ) {
			case Perspective:
				setProjectionMatrixAsPerspective(width, height);
				break;

			case Ortho:
				setProjectionMatrixAsOrtho(width, height);
				m_Viewer->getCamera()->setProjectionResizePolicy( osg::Camera::FIXED );
				break;
		}

		m_Viewer->getCamera()->setGraphicsContext( m_GraphicsWindow.get() );
		return m_GraphicsWindow.valid();
	}

	void setProjectionMatrixAsPerspective(double width, double height)
	{
		m_Viewer->getCamera()->setProjectionMatrixAsPerspective( 30.0, width/height, 1.0, 10000.0 );
	}

	void setProjectionMatrixAsOrtho(double width, double height)
	{
		m_Viewer->getCamera()->setProjectionMatrixAsOrtho( -width/2.0, width/2.0, -height/2.0, height/2.0, 10000.0, 0.0 );
	}
};

OsgGraphicsWindow::OsgGraphicsWindow(ProjectionKind projectionKind, osg::ref_ptr<osgGA::CameraManipulator> manipulator) : m_Implementation(new Implementation()) {
	m_Implementation->init(this, projectionKind, manipulator);
}

OsgGraphicsWindow::~OsgGraphicsWindow() {
	delete m_Implementation;
}

void OsgGraphicsWindow::clearSignals() {
	SetCurrent.clear();
	SwapBuffers.clear();
}

osg::Vec3 OsgGraphicsWindow::getCameraPosition() const {
	const osg::Matrix & view_matrix = m_Implementation->m_Viewer->getCamera()->getViewMatrix();
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
	m_Implementation->m_GraphicsWindow->getEventQueue()->windowResize(0, 0, width, height);
	m_Implementation->m_GraphicsWindow->resized(0, 0, width, height);

	if ( m_Implementation->m_projectionKind == Ortho )
	{
		m_Implementation->setProjectionMatrixAsOrtho(width, height);
	}
}

void OsgGraphicsWindow::handleKeyDown(int key) {
	m_Implementation->m_GraphicsWindow->getEventQueue()->keyPress(key);
}

void OsgGraphicsWindow::handleKeyUp(int key) {
	m_Implementation->m_GraphicsWindow->getEventQueue()->keyRelease(key);
}

void OsgGraphicsWindow::handleMouseMotion(int x, int y) {
	m_Implementation->m_GraphicsWindow->getEventQueue()->mouseMotion(x, y);
}

void OsgGraphicsWindow::handleMouseButtonDown(int x, int y, int button) {
	m_Implementation->m_GraphicsWindow->getEventQueue()->mouseButtonPress(x, y, button);
}

void OsgGraphicsWindow::handleMouseButtonUp(int x, int y, int button) {
	m_Implementation->m_GraphicsWindow->getEventQueue()->mouseButtonRelease(x, y, button);
}

void OsgGraphicsWindow::handleMouseWheelRotation(int wheelRotation) {
	m_Implementation->m_GraphicsWindow->getEventQueue()->mouseScroll( wheelRotation < 0 ? osgGA::GUIEventAdapter::SCROLL_DOWN : osgGA::GUIEventAdapter::SCROLL_UP );
}

osg::ref_ptr<osgGA::CameraManipulator> OsgGraphicsWindow::getManipulator() {
	return m_Implementation->m_Viewer->getCameraManipulator();
}

osg::ref_ptr<osg::Node> OsgGraphicsWindow::getSceneData() {
	return m_Implementation->m_Viewer->getSceneData();
}

void OsgGraphicsWindow::setSceneData(osg::Node* node) {
	m_Implementation->m_Viewer->setSceneData(node);
}

} // namespace layout
} // namespace csp
