#ifdef OGL

///////////////////////////////////////////////////////////////////////////
//
//   GLCamera  - CSP - http://csp.homeip.net
//
//   coded by Wolverine, Brandon and GKW
//
///////////////////////////////////////////////////////////////////////////
//   This code represents the OpenGL Implementation of StandardCamera.
//   The Camera is the viewpoint in all renderings.
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//
///////////////////////////////////////////////////////////////////////////

#include "Framework.h"
#include "GLCamera.h"


/** Camera 
 * This class positions the 'camera' of the scene.
 */



/** 
 * \fn GLCamera::GLCamera()
 * \brief Camera constructor
 */
GLCamera::GLCamera()
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::GLCamera()");

  p_CameraMatrix = StandardMatrix4::IDENTITY;

  p_CameraInverseMatrix = StandardMatrix4::IDENTITY;
  p_CameraInverseMatrix.Inverse();


  p_Dirty = false;


}

/** 
 * \fn GLCamera::~GLCamera()
 * \brief Camera destructor 
 * 
 */
GLCamera::~GLCamera()
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::GLCamera()");
}

/** 
 * \fn GLCamera::SetCameraIdentity
 * \brief Sets the camera position back to the origin. and facing in the -z direction. 
 *
 */
void GLCamera::SetCameraIdentity()
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::SetCameraIdentity()");

  p_CameraMatrix = StandardMatrix4::IDENTITY;
  p_Dirty = true;

}



/** 
 * \fn void GLCamera::SetPosition(float loc_x, float loc_y, float loc_z,float target_x, float target_y, float target_z,float up_x, float up_y, float up_z)
 * \brief Sets the camera matrix to an absolute look_at position.
 * \param loc_x The camera X location.
 * \param loc_y The camera Y location.
 * \param loc_z The camera Z location.
 * \param target_x The camera targets X location.
 * \param target_y The camera targets Y location.
 * \param target_z The camera targets Z location.
 * \param up_x The camera's UP vector X component.
 * \param up_y The camera's UP vector Y component.
 * \param up_z The camera's UP vector Z component.
 */
void GLCamera::SetPosition(float loc_x, float loc_y, float loc_z,
		                     float target_x, float target_y, float target_z,
							 float up_x, float up_y, float up_z)
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::SetPosition() - Location: "
	  << loc_x << ", " << loc_y << ", " << loc_z << ", Target: "
	  << target_x << ", " << target_y << ", " << target_z << ", Up:"
	  << up_x << ", " << up_y << ", " << up_z );


  StandardVector3 loc(loc_x, loc_y,loc_z);
  StandardVector3 target(target_x, target_y, target_z);
  StandardVector3 upVector(up_x, up_y, up_z);
  p_CameraMatrix = LookAtMatrix4(loc,target, upVector);

  p_Dirty = true;


}

/** 
 * \fn GLCamera::SetPosition(StandardVector3 *pLoc, StandardVector3 * pTarget, StandardVector3 * pUp)
 * \brief Vector version of SetPosition
 * \param pLoc The Cameras location vector
 * \param pTarget The Cameras target vector
 * \param pUp The Cameras up vector
 *
 * Sets the camera matrix to a absolute position define
 * the vectors: location, target and up.
 */
void GLCamera::SetPosition(StandardVector3 *pLoc, StandardVector3 * pTarget, StandardVector3 * pUp)
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::SetPosition() - Location: "
	  << pLoc << ", Target: "
	  << pTarget << ", Up:"
	  << pUp );


  p_CameraMatrix = LookAtMatrix4(*pLoc, *pTarget, *pUp);

  p_Dirty = true;

	
}

/** SetPosition
 * roll,pitch,heading form of SetPosition.
 * Set the camera matrix to an absolute position.
 */
void GLCamera::SetPosition(float roll, float pitch, float heading,
		                     float loc_x, float loc_y, float loc_z)
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::SetPosition() - roll: "
	  << roll << ", pitch: "
	  << pitch << ", heading: "
	  << heading << ", loc_x: " << loc_x  
	  << ", loc_y: " << loc_y << ", loc_z: " << loc_z );

  p_Dirty = true;

}
	


/** GLCamera::SetPosition(float loc_x, float loc_y, float loc_z)
 * \brief Moves the camera to a new point 
 * \param loc_x The x position of the camera.
 * \param loc_y The y position of the camera.
 * \param loc_z The z position of the camera.
 */
void GLCamera::SetPosition(float loc_x, float loc_y, float loc_z)
{

	CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::SetPosition() - " 
		<< loc_x << ", " << loc_y << ", " << loc_z );

  p_CameraMatrix = TranslationMatrix4(-loc_x,-loc_y,-loc_z);

  p_Dirty = true;

}


/** \fn void GLCamera::Move(char axis, float amount)
 * \brief Moves the camera to a new point a relative position.
 * \param axis The axis to move the camera along.
 * \param amount The amount to move the camera.
 *
 * The parameters are axis which specifices the
 * axis to move in the direction. Can be ('x','y','z',
 * 'X','Y', or 'Z'). The other parameter which amount.
 */
void GLCamera::Move(char axis, float amount)
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::Move() - " << axis << ", " << amount);

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
}

/** 
 * \fn void GLCamera::Rotate(char axis, float amount)
 * \brief Rotate the current camera matrix by an Angle around anaxis defined by x,y,z.
 * \param axis The axis to move the camera around.
 * \param amount The amount in radians to move the camera.
 */
void GLCamera::Rotate(char axis, float amount)
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::Rotate() - " << axis << ", " << amount);

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

}

// View/Projection routines

/**
 * \fn void GLCamera::SetFOVPerspectiveView(float angle, unsigned short width, unsigned short height, float NearZ, float FarZ)
 * \brief Sets the projection matrix using a perspective view.
 * \param angle The field of View angle.
 * \param width The width in pixels of the view.
 * \param height The height in pixels of the view.
 * \param NearZ The near plane of the perspective view.
 * \param FarZ The far plane of the perspective view.
 */
void GLCamera::SetFOVPerspectiveView(float angle, 
									 unsigned short width, unsigned short height,
									 float NearZ, float FarZ)
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::SetFOVPerspectviewView() - " <<
		angle << ", " << width << ", " << height << ", " << NearZ << ", " << FarZ );

     p_ProjectionMatrix = PerspectiveFOVMatrix4(angle, (float)width / (float)height, NearZ, FarZ);

}

/** 
 * \fn void GLCamera::SetPerspectiveView(float angle, unsigned short width, unsigned short height, float NearZ, float FarZ)
 * \brief Sets the projection matrix using a perspective view.
 * \param angle The field of View angle.
 * \param width The width in pixels of the view. 
 * \param height The height in pixels of the view.
 * \param NearZ The near plane of the perspective view.
 * \param FarZ The far plane of the perspective view.
 *
 */
void GLCamera::SetPerspectiveView(float angle, 
									 unsigned short width, unsigned short height,
									 float NearZ, float FarZ)
{

	CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::SetPerspectiveView() - " << angle <<
		", " << width << ", " << height << ", " << NearZ << ", " << FarZ );

     p_ProjectionMatrix = PerspectiveMatrix4(angle, (float)width / (float)height, NearZ, FarZ);

}


/** 
 * \fn void GLCamera::SetOrthoView(unsigned short width, unsigned short height, float NearZ, float FarZ)
 * \brief Sets the projection matrix using a ortho view.
 * \param width The width of the view in pixels.
 * \param height The height of the view in pixels.
 * \param NearZ The near plane of the view.
 * \param FarZ The far plane of the view.
 */
void GLCamera::SetOrthoView(unsigned short width, unsigned short height,
							float NearZ, float FarZ)
{

    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::SetOrthoView() - "
		<< width << ", " << height << ", " << NearZ << ", " << FarZ );

	p_ProjectionMatrix = OrthoMatrix4(width, height, 
                         NearZ, FarZ) ;

}

/**
 * \fn StandardMatrix4 * GLCamera::GetProjectionMatrix()
 * \brief returns a pointer to the projection matrix.
 */
StandardMatrix4 * GLCamera::GetProjectionMatrix()
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::GetProjectionMatrix()" );

  return &p_ProjectionMatrix;

}

/** 
 * \fn StandardMatrix4 * GLCamera::GetCameraMatrix()
 * \param returns a pointer to the camera's world/view matrix
 */
StandardMatrix4 * GLCamera::GetCameraMatrix()
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::GetCameraMatrix()" );

  return &p_CameraMatrix;

}

/**
 * \fn StandardMatrix4* GLCamera::GetCameraInverseMatrix()
 * \brief Returns the Inverse of the Camera's World/View matrix. 
 *
 * Returns the Inverse of the Camera's World/View matrix. p_Dirty is set to false to 
 * avoid recalculating the inverse if the camera has not moved.
 */
StandardMatrix4* GLCamera::GetCameraInverseMatrix()
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::GetCameraInverseMatrix()" );
	
  if(p_Dirty == true)
  {
    p_CameraInverseMatrix = p_CameraMatrix;
    p_CameraInverseMatrix.Inverse();
    p_Dirty = false;
  }

  return &p_CameraInverseMatrix;
}

/** 
 * \fn void GLCamera::Apply()
 * \brief Sets the Camera's world/view matrix to the OpenGL MODELVIEW matrix.
 */
void GLCamera::Apply()
{


  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLCamera::Apply()");

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glLoadMatrixf((float*)p_CameraMatrix);

}

#endif
