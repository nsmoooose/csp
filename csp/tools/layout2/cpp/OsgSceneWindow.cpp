#include <osgGA/TrackballManipulator>
#include <csp/tools/layout2/cpp/Scene.h>
#include <csp/tools/layout2/cpp/OsgSceneWindow.h>

namespace csp {
namespace layout {

OsgSceneWindow::OsgSceneWindow() : m_Scene(new Scene()) {
	setSceneData(m_Scene->getRootNode().get());
}

void OsgSceneWindow::moveCameraToHome() {
	osg::ref_ptr<osgGA::TrackballManipulator> manipulator = getTrackballManipulator();
}

FeatureGraph* OsgSceneWindow::graph() {
	return m_Scene->graph();
}

void OsgSceneWindow::Frame() {
	osg::ref_ptr<osgGA::TrackballManipulator> manipulator = getTrackballManipulator();
	osg::Vec3 cameraTarget = manipulator->getCenter();
	m_Scene->updateDynamicGrid(cameraTarget, getCameraPosition());

	OsgGraphicsWindow::Frame();
}

} // namespace layout
} // namespace csp
