#ifdef OGL

#include <stdlib.h>
#include <memory.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#include <graphics\typesgraphics.h>
#include "GLMaterialObject.h"

GLMaterialObject * GLMaterialObject::m_currMat=NULL;

GLMaterialObject::GLMaterialObject(StandardGraphics * pDisplay) : 
	MaterialObject(pDisplay)
{
	if (GLMaterialObject::m_currMat == NULL)
	{
		// this is a hack but set the current material
		// equal to this objects, then create a new one.
		// this stops infinite recursion.
		GLMaterialObject::m_currMat = this;
		GLMaterialObject::m_currMat = new GLMaterialObject(pDisplay);
	}
}

GLMaterialObject::~GLMaterialObject()
{

}

void GLMaterialObject::Create(_Color * pEmission,
						_Color * pSpecular,
						_Color * pDiffuse,
						_Color * pAmbient,
						float _shininess)
{
	emission = *pEmission;
	specular = *pSpecular;
	diffuse = *pDiffuse;
	ambient = *pAmbient;
	shininess = _shininess;

}

void GLMaterialObject::Destroy()
{

}

void GLMaterialObject::Apply()
{
	GLMaterialObject *currMat = (GLMaterialObject*)GetCurrentMaterial();

	if( currMat->ambient != ambient )
	{
		glMaterialfv( GL_FRONT, GL_AMBIENT, (float *)&ambient );
		currMat->ambient = ambient;
	}
	if( currMat->diffuse != diffuse)
	{
		glMaterialfv( GL_FRONT, GL_DIFFUSE, (float *)&diffuse );
		currMat->diffuse = diffuse;
	}
	if( currMat->emission != emission )
	{
		glMaterialfv( GL_FRONT, GL_EMISSION, (float *)&emission );
		currMat->emission = emission;
	}
	if( currMat->specular != specular )
	{
		glMaterialfv( GL_FRONT, GL_SPECULAR, (float *)&specular );
		currMat->specular = specular;
	}
	if( currMat->shininess != shininess )
	{
		glMaterialfv( GL_FRONT, GL_SHININESS, &shininess );
		currMat->shininess = shininess;
	}

}


MaterialObject * GLMaterialObject::GetCurrentMaterial()
{
	return m_currMat;
}

#endif
