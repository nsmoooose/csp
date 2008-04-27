#ifndef __CSPSIM_CLOUDS_CLOUDBOX_H__
#define __CSPSIM_CLOUDS_CLOUDBOX_H__

#include <osg/Billboard>
#include <osg/Geometry>
#include <osg/LOD>

namespace csp {
namespace clouds {

/* This represents a box of cloud sprites. This box contains several sprites that are 
billboarded to always face the camera. 

Dimensions are the radius from the center of the cloud to its edges. A x radius of 5.0
will generate a cloud that is 10.0 in total width.

The density controls how many sprites that will be created within this box. In order
to make the cloud more realistic, sprites that are outside of the ellipsoid this box
is generating are removed.

Color levels controls the color for each sprite depending on where it is located.
At the bottom or at the top. The threshold for each color level is a value between
0 and 1. 0.5 means 50% from the bottom.

Opacity levels controls alpha value depending on distance from center of cloud. The
threshold is a value between 0 and 1.0. */
class CloudBox : public osg::LOD
{
public:
	typedef std::pair<float, osg::Vec3> ColorLevel;
	typedef std::vector<ColorLevel> ColorLevelVector;

	typedef std::pair<float, float> OpacityLevel;
	typedef std::vector<OpacityLevel> OpacityLevelVector;

	CloudBox(void);
	virtual ~CloudBox(void);

	void setDimensions(osg::Vec3 dimensions);
	osg::Vec3 getDimensions();

	void setColorLevels(const ColorLevelVector& levels);
	ColorLevelVector getColorLevels();

	void setOpacityLevels(const OpacityLevelVector& levels);
	OpacityLevelVector getOpacityLevels();

	void setDensity(int count);
	int getDensity();

	// Generate necessary geometry to be able to display this cloud.
	void UpdateModel();

private:
	osg::Vec3 m_Dimensions;
	ColorLevelVector m_ColorLevels;
	OpacityLevelVector m_OpacityLevels;
	int m_Density;

	static const osg::Billboard::Mode BillboardMode;
	
	osg::ref_ptr<osg::Geometry> CreateCloudSprite(const float& alpha, const osg::Vec3& color, const osg::Vec2& size);
	osg::ref_ptr<osg::Node> CreateHighDetailGeometry();
	osg::ref_ptr<osg::Node> CreateMediumDetailGeometry();
	osg::ref_ptr<osg::Node> CreateLowDetailGeometry();
};

}	// end namespace clouds
}	// end namespace csp

#endif
