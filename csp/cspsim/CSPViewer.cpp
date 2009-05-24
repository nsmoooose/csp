#include <osg/Group>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <csp/cspsim/CSPViewer.h>
#include <csp/cspwf/WindowManagerEventHandler.h>
#include <csp/cspwf/WindowManagerViewer.h>

namespace csp {

CSPViewer::CSPViewer() {
	m_Viewer = new osgViewer::Viewer;
	m_Viewer->setSceneData(new osg::Group);
}

CSPViewer::CSPViewer(int left, int top, int width, int height) {
	m_Viewer = new osgViewer::Viewer;
	m_Viewer->setSceneData(new osg::Group);
	m_Viewer->setUpViewInWindow(left, top, width, height);
}

CSPViewer::~CSPViewer() {
}

void CSPViewer::run() {
	m_Viewer->run();
}

void CSPViewer::addChild(osg::Node* node) {
	osg::Group* group = dynamic_cast<osg::Group*>(m_Viewer->getSceneData());
	if(group) {
		group->addChild(node);
	}
}

void CSPViewer::addStatsHandler() {
	m_Viewer->addEventHandler(new osgViewer::StatsHandler());
}

void CSPViewer::addHelpHandler() {
	m_Viewer->addEventHandler(new osgViewer::HelpHandler());
}

void CSPViewer::addWindowManagerEventHandler(wf::WindowManagerViewer* windowManager) {
	wf::WindowManagerEventHandler* handler = new wf::WindowManagerEventHandler(windowManager);
	m_Viewer->addEventHandler(handler);
}

} // namespace csp
