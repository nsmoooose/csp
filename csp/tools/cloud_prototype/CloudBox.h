#ifndef __CSPSIM_CLOUDS_CLOUDBOX_H__
#define __CSPSIM_CLOUDS_CLOUDBOX_H__

#include <osg/Group>

/* This represents a box of cloud sprites. This box contains several sprites that are 
billboarded to always face the camera. */
class CloudBox : public osg::Group
{
public:
	CloudBox(void);
	virtual ~CloudBox(void);

	void setDimensions(osg::Vec3 dimensions);
	osg::Vec3 getDimensions();

	// All sprites that are closer to each other than the set
	// threshold will be removed from this cloud box.
	void setSpriteRemovalThreshold(float threshold);
	float getSpriteRemovalThreshold();

	void UpdateModel();

private:
	osg::Vec3 m_Dimensions;
	float m_SpriteRemovalThreshold;

	typedef std::pair<float, osg::Vec3> ColorLevel;
	typedef std::vector<ColorLevel> ColorLevelVector;
	ColorLevelVector m_ColorLevels;

	typedef std::pair<float, float> OpacityLevel;
	typedef std::vector<OpacityLevel> OpacityLevelVector;
	OpacityLevelVector m_OpacityLevels;
};

#endif
