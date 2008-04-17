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

	void setDepth(float depth);
	float getDepth();

	void setHeight(float height);
	float getHeight();

	void setWidth(float width);
	float getWidth();

	void UpdateModel();

private:
	float m_Depth;
	float m_Height;
	float m_Width;

	struct ColorLevel {
		ColorLevel() {}
		ColorLevel(const float& t, const osg::Vec3& c) : threshold(t), color(c) {}
		float threshold;
		osg::Vec3 color;
	};

	typedef std::vector<ColorLevel> ColorLevelVector;
	ColorLevelVector m_ColorLevels;

	struct OpacityLevel {
		OpacityLevel() {}
		OpacityLevel(const float& t, const float& o) : threshold(t), opacity(o) {}
		float threshold;
		float opacity;
	};

	typedef std::vector<OpacityLevel> OpacityLevelVector;
	OpacityLevelVector m_OpacityLevels;
};

#endif
