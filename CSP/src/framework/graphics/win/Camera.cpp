


#include "Camera.h"
#ifdef WIN32
#include  <windows.h>
#endif
#include <math.h>
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library
#include <standard/TypesMath.h>



/** Camera 
 * This class positions the 'camera' of the scene.
 */



/** Camera constructor
 * Set initial position to be at the 0,0,0 orgin.
 * facing in the -z direction with up pointing in the
 * y direction.
 */
Camera::Camera()
{
	x = 0; y = 0; z = 0;
	dir_x = 0; dir_y = 0; dir_z = -1;
	up_x = 0; up_y = 1; up_z = 0;
}

/** Camera destructor 
 * Clean up any paths etc.
 */
Camera::~Camera()
{
	// delete any paths if present.
}

/** position
 * Places the camera at the location
 * facing in the correct direction and
 * with the proper orientation.
 * This routine moves the camera to a 
 * absolute position.
 */
void Camera::Position()
{
	double target_x = x + dir_x;
	double target_y = y + dir_y;
	double target_z = z + dir_z;
	gluLookAt(x, y, z, target_x, target_y, target_z, up_x, up_y, up_z);
}

/** translate 
 * Moves the camera to a new point a relative position.
 */
void Camera::Translate(double X, double Y, double Z)
{
	x = X; 
	y = Y;
	z = Z;
	glTranslatef(-x, -y, -z);
}

void Camera::Rotate(double angle, double x, double y, double z)
{
	glRotatef(angle, x, y, z);
}


void Camera::SetPosition(double _x, double _y, double _z)
{
	x = _x;
	y = _y;
	z = _z;
}

void Camera::SetDirection(double x, double y, double z)
{
	dir_x = x;
	dir_y = y;
	dir_z = z;
}

void Camera::SetUp(double x, double y, double z)
{
	up_x = x;
	up_y = y;
	up_z = z;
}


void Camera::SetMatrix(_MatrixStruct  mat)
{

}

/** resetPosition
 * Sets the camera position back to the origin.
 * and facing in the -z direction. 
 */
void Camera::ResetPosition()
{
	glLoadIdentity();
}


void Camera::FollowPath()
{
	
}
