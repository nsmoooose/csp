#ifdef D3D

#include "WinDXCamera.h"

/** Camera 
 * This class positions the 'camera' of the scene.
 */

/** Camera constructor
 */
WinDXCamera::WinDXCamera(StandardGraphics* Graphics)
{
  p_Graphics = Graphics;
  p_Device = (IDirect3DDevice8 *)p_Graphics->GetDevice();

	p_CameraMatrix = StandardMatrix4::IDENTITY;

  p_CameraInverseMatrix = StandardMatrix4::IDENTITY;
  p_CameraInverseMatrix.Inverse();

  p_Dirty = false;

}

/** Camera destructor 
 * 
 */
WinDXCamera::~WinDXCamera()
{

}

/** SetCameraIdentity
 * Sets the camera position back to the origin.
 */
inline void WinDXCamera::SetCameraIdentity()
{
	p_CameraMatrix = StandardMatrix4::IDENTITY;

  p_Dirty = true;

}

/** SetPosition
 * Float version of SetPosition
 * Sets the camera matrix to an absolute look_at
 * position.
 */
inline void WinDXCamera::SetPosition(float loc_x, float loc_y, float loc_z,
		                     float target_x, float target_y, float target_z,
							 float up_x, float up_y, float up_z)
{
	p_CameraMatrix = LookAtMatrix4(StandardVector3(loc_x,loc_y,loc_z),
		                             StandardVector3(target_x,target_y,target_z),
								                 StandardVector3(up_x,up_y,up_z));

  p_Dirty = true;

}

/** SetPosition
 * Vector version of SetPosition
 * Sets the camera matrix to a absolute position define
 * the vectors: location, target and up.
 */
inline void WinDXCamera::SetPosition(StandardVector3 * pLoc, StandardVector3 * pTarget, StandardVector3 * pUp)
{
	p_CameraMatrix = LookAtMatrix4(*pLoc, *pTarget, *pUp);

  p_Dirty = true;


}

/** SetPosition
 * roll,pitch,heading form of SetPosition.
 * Set the camera matrix to an absolute position.
 */
inline void WinDXCamera::SetPosition(float roll, float pitch, float heading,
		                     float loc_x, float loc_y, float loc_z)
{

  p_Dirty = true;


}

/** translate 
 * Moves the camera to a new point a relative position.
 */
inline void WinDXCamera::SetPosition(float loc_x, float loc_y, float loc_z)
{
	p_CameraMatrix = TranslationMatrix4(-loc_x,-loc_y,-loc_z);

  p_Dirty = true;

}

/** translate 
 * Moves the camera to a new point a relative position.
 */
void WinDXCamera::Move(char axis, float amount)
{
  switch(axis)
  {
    case 'x':
    case 'X':
	    p_CameraMatrix *= TranslationMatrix4(-amount,0,0);
      break;

    case 'y':
    case 'Y':
	    p_CameraMatrix *= TranslationMatrix4(0,-amount,0);
      break;

    case 'z':
    case 'Z':
	    p_CameraMatrix *= TranslationMatrix4(0,0,-amount);
      break;
  }

  p_Dirty = true;

	// set world matrix
}

/** Rotate
 * Rotate the current camera matrix by an Angle around an
 * axis defined by x,y,z.
 */
void WinDXCamera::Rotate(char axis, float amount)
{
  switch(axis)
  {
    case 'x':
    case 'X':
	    p_CameraMatrix *= RotationXMatrix4(-amount);
      break;

    case 'y':
    case 'Y':
	    p_CameraMatrix *= RotationYMatrix4(-amount);
      break;

    case 'z':
    case 'Z':
	    p_CameraMatrix *= RotationZMatrix4(-amount);
      break;
  }

  p_Dirty = true;

	// set world matrix
}



/** SetPerspectiveView
 * Sets the projection matrix using a perspective view.
 * this uses a left handed perspective matrix.
 */
inline void WinDXCamera::SetFOVPerspectiveView(float Angle, 
                         unsigned short Width, unsigned short Height,
                         float NearZ, float FarZ)
{

  p_ProjectionMatrix = PerspectiveFOVMatrix4(Angle, (float)Width / (float)Height, NearZ, FarZ);
}



/** SetPerspectiveView
 * Sets the projection matrix using a perspective view.
 * this uses a right handed perspective matrix
 */
inline void WinDXCamera::SetPerspectiveView(float Angle, 
                         unsigned short Width, unsigned short Height,
                         float NearZ, float FarZ)
{

  p_ProjectionMatrix = PerspectiveMatrix4(Angle, (float)Width / (float)Height, NearZ, FarZ);

}

/** SetOrthoView
 * Sets the projection matrix using a ortho view.
 */
inline void WinDXCamera::SetOrthoView(unsigned short width, unsigned short height,float NearZ, float FarZ)
{
	p_ProjectionMatrix = OrthoMatrix4(width, height, 
                         NearZ, FarZ) ;

}


inline StandardMatrix4* WinDXCamera::GetProjectionMatrix()
{
  return &p_ProjectionMatrix;
}

inline StandardMatrix4* WinDXCamera::GetCameraMatrix()
{
  return &p_CameraMatrix;
}

inline StandardMatrix4* WinDXCamera::GetCameraInverseMatrix()
{

  if(p_Dirty == true)
  {
    p_CameraInverseMatrix = p_CameraMatrix;
    p_CameraInverseMatrix.Inverse();
    p_Dirty = false;
  }

  return &p_CameraInverseMatrix;
}

inline void WinDXCamera::Apply()
{
  p_Device->SetTransform( D3DTS_VIEW, (D3DMATRIX *)&p_CameraMatrix);
}

#endif
