#ifndef __CSPSIM_CLOUDS_VISITORS_UPDATECLOUDMODELVISITOR_H__
#define __CSPSIM_CLOUDS_VISITORS_UPDATECLOUDMODELVISITOR_H__

#include <osg/NodeVisitor>

namespace csp {
namespace weather {
namespace clouds {
namespace visitors {

/* Visits all childnodes and calls UpdateModel to force all 
necessary geometry to be created. */
class UpdateCloudModelVisitor : public osg::NodeVisitor {
public:
	UpdateCloudModelVisitor();
	virtual void apply(osg::Node& node);
};

} // end namespace visitors
} // end namespace clouds
} // end namespace weather
} // end namespace csp

#endif
