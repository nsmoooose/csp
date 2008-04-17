#pragma once

#include <osg/vec3>

class CloudMath
{
public:
	static bool InsideEllipsoid(osg::Vec3 point, osg::Vec3 ellipsoid);
	static float GenerateRandomNumber(float minimum, float maximum);
};
