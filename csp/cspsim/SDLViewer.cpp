#include <osg/Camera>
#include <osg/State>
#include <osgGA/GUIEventAdapter>
#include <osgViewer/Viewer>
#include <SDL/SDL.h>
#include "SDLViewer.h"

namespace csp {

SDLViewer::SDLViewer(int width, int height, osg::State* state) {
    m_GraphicsWindow = m_Viewer.setUpViewerAsEmbeddedInWindow(0, 0, width, height);
	m_GraphicsWindow->setState(state);
    m_Viewer.realize();

	osg::ref_ptr<osg::Camera> camera = m_Viewer.getCamera();
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
}

osgViewer::Viewer* SDLViewer::getViewer() {
	return &m_Viewer;
}	

void SDLViewer::frame() {
	m_Viewer.frame();
}

} // namespace csp
