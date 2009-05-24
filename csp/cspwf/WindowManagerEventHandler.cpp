#include <csp/cspwf/WindowManagerEventHandler.h>
#include <csp/cspwf/WindowManagerViewer.h>

#include <iostream>

namespace csp {
namespace wf {

WindowManagerEventHandler::WindowManagerEventHandler(WindowManagerViewer* windowManager) : 
	m_WindowManager(windowManager) {
}

WindowManagerEventHandler::~WindowManagerEventHandler() {
}

bool WindowManagerEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&, osg::Object*, osg::NodeVisitor*) {
	switch(ea.getEventType()) {
	case osgGA::GUIEventAdapter::PUSH:
		return m_WindowManager->onMouseDown(
			static_cast<int>(ea.getX()), 
			static_cast<int>(ea.getYmax() - ea.getY()),
			ea.getButton());
	case osgGA::GUIEventAdapter::RELEASE:
		return m_WindowManager->onMouseUp(
			static_cast<int>(ea.getX()),
			static_cast<int>(ea.getYmax() - ea.getY()),
			ea.getButton());
	case osgGA::GUIEventAdapter::DRAG:
	case osgGA::GUIEventAdapter::MOVE:
		return m_WindowManager->onMouseMove(
			static_cast<int>(ea.getX()), 
			static_cast<int>(ea.getYmax() - ea.getY()));
	default:
		return false;
	}
}

} // namespace wf
} // namespace csp

