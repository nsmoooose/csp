#ifndef __TYPESCAMERA_H__
#define __TYPESCAMERA_H__

#include "Framework.h"

/** StandardCamera
 * The camera class controls the graphics world/model/view
 * and projection matrices. The camera matrix will be defined
 * to be the world/model/view matrix. The camera class provides
 * both absolute and relative methods to set the camera matrix.
 * Methods to Set prospective and ortho matries.
 * And methods to query the camera and projection matrices.
 */

class StandardCamera
{

private:

public:
	 //StandardCamera() {};
    virtual ~StandardCamera() {};

	// Camera Matrix methods
	virtual void SetCameraIdentity() = 0;

	// absolute SetPosition methods (LookAt)
	virtual void SetPosition(float loc_x, float loc_y, float loc_z,
		        float target_x, float target_y, float target_z,
			    float up_x, float up_y, float up_z) = 0;

	virtual void SetPosition(StandardVector3 * pLocation, 
		        StandardVector3 * pTarget, 
				StandardVector3 * pUp) = 0;

	virtual void SetPosition(float roll, float pitch, float heading,
	            float loc_x, float loc_y, float loc_z) = 0;

	virtual void SetPosition(float loc_x, float loc_y, float loc_z) = 0;

	// relative position methods
	virtual void Rotate(char axis, float amount) = 0;
  virtual void Move(char axis, float amount) = 0;

	// projection matrix methods
	virtual void SetFOVPerspectiveView(float Angle, 
                          unsigned short Width, unsigned short Height,
                          float NearZ, float FarZ) = 0;
	virtual void SetPerspectiveView(float Angle, 
                          unsigned short Width, unsigned short Height,
                          float NearZ, float FarZ) = 0;
	virtual void SetOrthoView(unsigned short width, unsigned short height,float NearZ, float FarZ) = 0;

	// query methods

  virtual void Apply() = 0;

  virtual StandardMatrix4* GetProjectionMatrix() = 0;
  virtual StandardMatrix4* GetCameraMatrix() = 0;
  virtual StandardMatrix4* GetCameraInverseMatrix() = 0;

};

#endif
