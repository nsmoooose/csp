
#ifdef	WIN32
#include <windows.h>
#endif

#include "MaterialFormat.h"
#include <gl/gl.h>

MaterialFormat::MaterialFormat()
{
	mat = NULL;
}

MaterialFormat::MaterialFormat( _Material *material )
{
	mat = material;
}

MaterialFormat::~MaterialFormat()
{
	delete( mat );
}

void MaterialFormat::Apply()
{
	_Material *currMat = GetCurrentMaterial();

	if( *(currMat->ambient) != *(mat->ambient) )
	{
		glMaterialfv( GL_FRONT, GL_AMBIENT, (float *)mat->ambient );
		*(currMat->ambient) = *(mat->ambient);
	}
	if( *(currMat->diffuse) != *(mat->diffuse) )
	{
		glMaterialfv( GL_FRONT, GL_DIFFUSE, (float *)mat->diffuse );
		*(currMat->diffuse) = *(mat->diffuse);
	}
	if( *(currMat->emission) != *(mat->emission) )
	{
		glMaterialfv( GL_FRONT, GL_EMISSION, (float *)mat->emission );
		*(currMat->emission) = *(mat->emission);
	}
	if( *(currMat->specular) != *(mat->specular) )
	{
		glMaterialfv( GL_FRONT, GL_SPECULAR, (float *)mat->specular );
		*(currMat->specular) = *(mat->specular);
	}
	if( currMat->shininess != mat->shininess )
	{
		glMaterialfv( GL_FRONT, GL_SHININESS, mat->shininess );
		currMat->shininess = mat->shininess;
	}
}

void MaterialFormat::SetMaterial( _Material *material )
{
	if( mat == NULL )
	{
		mat = material;
	} else {
		delete( mat );
		mat = material;
	}
}

_Material * MaterialFormat::GetCurrentMaterial()
{
	return( NULL );
}