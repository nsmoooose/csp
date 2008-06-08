#ifndef __CSPSIM_CLOUDS_VISITORS_REMOVEREDUNDANTSPRITESVISITOR_H__
#define __CSPSIM_CLOUDS_VISITORS_REMOVEREDUNDANTSPRITESVISITOR_H__

#include <osg/NodeVisitor>

namespace csp {
namespace weather {
namespace clouds {

class CloudBox;

namespace visitors {

/* Removes redundant sprites if they are too close to each other. */
class RemoveRedundantCloudSpritesVisitor : public osg::NodeVisitor {
public:
	RemoveRedundantCloudSpritesVisitor(float threshold);
	virtual void apply(osg::Node& node);

private:
	float m_Threshold;
	void RemoveSprites(CloudBox* cloudBox);
};

} // end namespace visitors
} // end namespace clouds
} // end namespace weather
} // end namespace csp

#endif
