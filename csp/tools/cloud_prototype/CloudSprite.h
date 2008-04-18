#ifndef __CSPSIM_CLOUDS_CLOUDSPRITE_H__
#define __CSPSIM_CLOUDS_CLOUDSPRITE_H__

#include <osg/Billboard>

namespace csp {
namespace clouds {

/* This object represents a single billboarded image within a cloud box.
A cloud box typically contains ~100 sprites at random positions. 

Each cloud sprite can have an alpha value and a color. The alpha value
is used to make sprites at the edges of the cloudbox more transparent.

The color is used to make lower parts of the clouds darker and top parts
brighter. */
class CloudSprite : public osg::Billboard
{
public:
	CloudSprite(void);
	virtual ~CloudSprite(void);

	// Creates neccessary geometry with alpha and color value.
	void UpdateModel(const osg::Vec3& position, const float& alpha, const osg::Vec3& color);
};

}	// end namespace clouds
}	// end namespace csp

#endif
