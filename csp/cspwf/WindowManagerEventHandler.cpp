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

bool WindowManagerEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&) {
	switch(ea.getEventType()) {
	case osgGA::GUIEventAdapter::PUSH:
		m_WindowManager->onClick(
			static_cast<int>(ea.getX()), 
			static_cast<int>(ea.getYmax() - ea.getY()));
		return true;
	case osgGA::GUIEventAdapter::RELEASE:
		return true;
	case osgGA::GUIEventAdapter::MOVE:
		m_WindowManager->onMouseMove(
			static_cast<int>(ea.getX()), 
			static_cast<int>(ea.getYmax() - ea.getY()), 0, 0);
		return true;
	default:
		return false;
	}
}

} // namespace wf
} // namespace csp

