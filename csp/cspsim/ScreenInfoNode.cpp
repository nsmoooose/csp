#include <csp/cspsim/ScreenInfoNode.h>

void csp::ScreenInfoNode::setUp(osg::Group* scene) {
	osg::ref_ptr<osg::Group> group = new osg::Group();
	group->setName("info_group");
	scene->addChild(group.get());
}

osg::Group* csp::ScreenInfoNode::getGroup(osg::Group* scene) {
	unsigned int childCount = scene->getNumChildren();
	for(unsigned int index=0;index<childCount;++index) {
		osg::Node* node = scene->getChild(index);
		if(node->getName() == "info_group") {
			return dynamic_cast<osg::Group*>(node);
		}
	}
	return NULL;
}

void csp::ScreenInfoNode::tearDown(osg::Group* scene) {
	osg::ref_ptr<osg::Group> group = getGroup(scene);
	if(group.valid()) {
		scene->removeChild(group.get());
	}
}
