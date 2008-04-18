#include "../CloudBox.h"
#include "UpdateCloudModelVisitor.h"

namespace csp {
namespace clouds {
namespace visitors {

UpdateCloudModelVisitor::UpdateCloudModelVisitor() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {
}

void UpdateCloudModelVisitor::apply(osg::Node& node) {
	CloudBox* cloudBox = dynamic_cast<CloudBox*>(&node);
	if(cloudBox != NULL) {
		cloudBox->UpdateModel();
	}
	traverse(node);
}

} // end namespace visitors
} // end namespace clouds
} // end namespace csp
