#ifndef __GLCAMERA_H__
#define __GLCAMERA_H__

#include "Framework.h"

class GLCamera : public StandardCamera
{
public:
  GLCamera();
  virtual ~GLCamera();

  // Camera Matrix Methods
  void SetCameraIdentity();

  // Absolute position methods.
  void SetPosition(float loc_x, float loc_y, float loc_z,
		                     float target_x, float target_y, float target_z,
							 float up_x, float up_y, float up_z);

  void SetPosition(StandardVector3 * pLoc,
							StandardVector3 * pTarget,
							StandardVector3 * pUp);

  void SetPosition(float roll, float pitch, float heading,
		                     float loc_x, float loc_y, float loc_z);
	
  // relative position methods.
//	void Translate(float X, float Y, float Z);
//	void Rotate(float angle, float x, float y, float z);
  
  void Rotate(char axis, float amount);
  void Move(char axis, float amount);

  void SetPosition(float loc_x, float loc_y, float loc_z);


  // projection matrix methods.
  void SetFOVPerspectiveView(float angle, unsigned short width, unsigned short height, float nearplane, float farplane);
  void SetPerspectiveView(float Angle, unsigned short Width, unsigned short Height, float NearZ, float FarZ);
  void SetOrthoView(unsigned short width, unsigned short height,float NearZ, float FarZ);

  // query methods  

  StandardMatrix4 * GetProjectionMatrix();
  StandardMatrix4 * GetCameraMatrix();
  StandardMatrix4 * GetCameraInverseMatrix();

  void Apply();


private:

  StandardMatrix4 p_CameraMatrix;
  StandardMatrix4 p_ProjectionMatrix;
  StandardMatrix4 p_CameraInverseMatrix;
  int p_Dirty;

  
};

#endif
