#ifdef OGL

#ifdef WIN32
#include  <windows.h>
#endif
#include <math.h>
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library
#include <graphics\typesgraphics.h>
#include "openglgraphics.h"
#include "glcamera.h"
#include <standard/TypesMath.h>



/** Camera 
 * This class positions the 'camera' of the scene.
 */



/** Camera constructor
 * 
 * 
 * 
 */
GLCamera::GLCamera(StandardGraphics * pDisplay)
	: Camera(pDisplay)
{
	m_pDisplay = pDisplay;
}

/** Camera destructor 
 * Clean up any paths etc.
 */
GLCamera::~GLCamera()
{
	// delete any paths if present.
}

/** SetPosition
 * Places the camera at the location
 * facing in the correct direction and
 * with the proper orientation.
 * This routine moves the camera to a 
 * absolute position.
 */
void GLCamera::SetPosition(float loc_x, float loc_y, float loc_z,
		                     float target_x, float target_y, float target_z,
							 float up_x, float up_y, float up_z)
{
	gluLookAt(loc_x, loc_y, loc_z, target_x, target_y, target_z, up_x, up_y, up_z);
}

/** SetPosition
 * Vector version of SetPosition
 */
void GLCamera::SetPosition(_Vector * loc, _Vector * target, _Vector * up)
{
	gluLookAt(loc->x, loc->y, loc->z, 
		target->x, target->y, target->z,
		up->x, up->y, up->z);
}

void GLCamera::SetPosition(vector3 & loc, vector3 & target, vector3 & up)
{
	gluLookAt(loc.x, loc.y, loc.z,
		      target.x, target.y, target.z,
			  up.x, up.y, up.z);
}

/** SetPosition
 * Euler(roll,pitch,heading) version
 */
void GLCamera::SetPosition(float roll, float pitch, float heading,
		                     float loc_x, float loc_y, float loc_z)
{
	glRotatef(roll, 1.0, 0.0, 0.0);
	glRotatef(pitch, 0.0, 1.0, 0.0);
	glRotatef(heading, 0.0, 0.0, 1.0);
	glTranslatef(-loc_x, -loc_y, -loc_z);
}
	


/** translate 
 * Moves the camera to a new point a relative position.
 */
void GLCamera::Translate(float X, float Y, float Z)
{
	glTranslatef(-X, -Y, -Z);
}

void GLCamera::Rotate(float angle, float x, float y, float z)
{
	glRotatef(angle, x, y, z);
}



void GLCamera::SetMatrix(_Matrix * mat)
{
	glLoadMatrixf( (float *)mat );
}

void GLCamera::MultiMatrix(_Matrix * mat)
{
	glMultMatrixf((float*)mat);
}


/** reset
 * Sets the camera position back to the origin.
 * and facing in the -z direction. 
 */
void GLCamera::Reset()
{
	glLoadIdentity();
}

void GLCamera::SetPerspectiveView(float angle, float nearplane, float farplane)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	_Rect rect = m_pDisplay->GetCoordsRect();
	float aspect = (rect.x2-rect.x1)/(rect.y2-rect.y1);
	gluPerspective(angle, aspect, nearplane, farplane);
	glMatrixMode(GL_MODELVIEW);
}

void GLCamera::SetOrthoView(float nearplane, float farplane)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	_Rect rect = m_pDisplay->GetCoordsRect();
	
	glOrtho(rect.x1, rect.x2, rect.y1, rect.y2, nearplane, farplane);
	glMatrixMode(GL_MODELVIEW);
}

#endif