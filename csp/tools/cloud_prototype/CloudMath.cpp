#include <stdlib.h>
#include "CloudMath.h"

bool CloudMath::InsideEllipsoid(osg::Vec3 point, osg::Vec3 ellipsoid) {
	float value1 = 
		(point.x() * point.x()) / (ellipsoid.x() * ellipsoid.x()) +
		(point.y() * point.y()) / (ellipsoid.y() * ellipsoid.y());
	float value2 = 
		(point.x() * point.x()) / (ellipsoid.x() * ellipsoid.x()) +
		(point.z() * point.z()) / (ellipsoid.z() * ellipsoid.z());
	float value3 = 
		(point.y() * point.y()) / (ellipsoid.y() * ellipsoid.y()) +
		(point.z() * point.z()) / (ellipsoid.z() * ellipsoid.z());

	return value1 < 1 && value2 < 1 && value3 < 1;
}

float CloudMath::GenerateRandomNumber(float minimum, float maximum) {
	return (rand() % static_cast<int>(maximum - minimum)) + minimum;
}