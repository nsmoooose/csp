#ifdef OGL

#ifdef	WIN32
#include <windows.h>
#endif

#include <gl/gl.h>
#include "TextureFormat.h"

TextureFormat::TextureFormat()
{
	tex				= 0;
	texMode			= GL_REPLACE;
	multiTexture	= false;
}

TextureFormat::TextureFormat( Texture *texture, unsigned long mode, bool multi )
{
	tex = texture;
	texMode = mode;
	multiTexture = multi;
}

TextureFormat::~TextureFormat()
{
	delete( rotation );
}

void TextureFormat::Apply()
{

}

void TextureFormat::setTexture( Texture *texture )
{
	tex = texture;
}

Texture * TextureFormat::getTexture()
{
	return( tex );
}

void TextureFormat::setTexMode( unsigned long mode )
{
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode );
}

void TextureFormat::setWrapS( unsigned long mode )
{
	if( tex->setWrapS( mode ) == false )
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode );
}

void TextureFormat::setWrapT( unsigned long mode )
{
	if( tex->setWrapT( mode ) == false )
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode );
}

void TextureFormat::setWrapR( unsigned long mode )
{
#ifdef	LINUX
	if( tex->setWrapR( mode ) == false )
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, mode );
#endif
}

void TextureFormat::setMagFilter( unsigned long mode )
{
	if( tex->setMagFilter( mode ) == false )
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode );
}

void TextureFormat::setMinFilter( unsigned long mode )
{
	if( tex->setMinFilter( mode ) == false )
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode );
}

void TextureFormat::setTexRotation( float *rot )
{
	rotation = rot;
}

void TextureFormat::setMultiTexture( bool mode )
{
	multiTexture = mode;
}

void TextureFormat::setTexUnit( int unit )
{
	texUnit = unit;
}

#endif