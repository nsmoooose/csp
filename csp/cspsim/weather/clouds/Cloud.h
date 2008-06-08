#ifndef __CSPSIM_CLOUDS_CLOUD_H__
#define __CSPSIM_CLOUDS_CLOUD_H__

#include <osg/Group>

namespace csp {
namespace weather {
namespace clouds {

/* A cloud is a group of cloud boxes. Each cloud box contains several cloud sprites.
Together these forms a single cloud. */
class Cloud : public osg::Group
{
public:
	Cloud(void);
	virtual ~Cloud(void);

	// Generates all necessary geometry to be able to display this cloud.
	void UpdateModel();

	// All sprites that are closer to each other than the set threshold will be removed.
	void setSpriteRemovalThreshold(float threshold);
	float getSpriteRemovalThreshold();

private:
	float m_SpriteRemovalThreshold;
};

}	// end namespace clouds
} 	// end namespace weather
}	// end namespace csp

#endif
