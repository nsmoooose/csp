#ifndef __GLCAMERA_H__
#define __GLCAMERA_H__

#include <standard/TypesMath.h>
#include "graphics/typesgraphics.h"

class GLCamera : public Camera
{
public:
	GLCamera(StandardGraphics * pDisplay);
	~GLCamera();

	virtual void SetPosition(float loc_x, float loc_y, float loc_z,
		                     float target_x, float target_y, float target_z,
							 float up_x, float up_y, float up_z);
	virtual void SetPosition(_Vector * loc, _Vector * target, _Vector * up);
	virtual void SetPosition(vector3 &, vector3 &, vector3 &);
	virtual void SetPosition(float roll, float pitch, float heading,
		                     float loc_x, float loc_y, float loc_z);
	virtual void Translate(float X, float Y, float Z);
	virtual void Rotate(float angle, float x, float y, float z);

	virtual void SetMatrix(_Matrix *  mat);
	virtual void MultiMatrix(_Matrix * mat);
	virtual void SetPerspectiveView(float angle, float nearplane, float farplane);
	virtual void SetOrthoView(float nearplane, float farplane);

	virtual void Reset();


private:

	StandardGraphics * m_pDisplay;

};

#endif