#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <standard/TypesMath.h>

class Camera
{
public:
	Camera();
	~Camera();

	void Position();
	void SetPosition(double x, double y, double z);
	void SetDirection(double x, double y, double z);
	void SetUp(double x, double y, double z);
	void Translate(double x, double y, double z);
	void Rotate(double angle, double x, double y, double z);
	void ResetPosition();
	void SetMatrix(_MatrixStruct  mat);

	void FollowPath();



private:
	double x;
	double y;
	double z;

	double dir_x;
	double dir_y;
	double dir_z;

	double up_x;
	double up_y;
	double up_z;

};

#endif