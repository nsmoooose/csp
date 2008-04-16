#ifndef __CSPSIM_CLOUDS_CLOUDSPRITE_H__
#define __CSPSIM_CLOUDS_CLOUDSPRITE_H__

#include <osg/Billboard>

class CloudSprite : public osg::Billboard
{
public:
	CloudSprite(void);
	virtual ~CloudSprite(void);

	void UpdateModel(const osg::Vec3& position, const float& alpha, const osg::Vec3& color);
};

#endif