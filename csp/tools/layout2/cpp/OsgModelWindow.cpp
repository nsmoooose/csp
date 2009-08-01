#include <osgDB/ReadFile>
#include <csp/tools/layout2/cpp/OsgModelWindow.h>

namespace csp {
namespace layout {

void OsgModelWindow::loadModel(const std::string& file) {
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(file.c_str());
	if(node.valid()) {
		setSceneData(node.get());
	}
}

} // namespace layout
} // namespace csp
