#ifdef OGL


#ifdef WIN32
#include <windows.h>
#endif

#include <gl/gl.h>
#include <gl/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>
#include <string.h>
#include "Texture.h"
#include "bitmap.h"
#include "graphics/win/tgainfo.h"

int const Texture::DIB	= 1;
int const Texture::TGA	= 2;
short Texture::mipmaplevels = 0;

Texture::Texture()
{
	texID		= 0;
	width		= 0;
	height		= 0;
	intFormat	= 0;
	extFormat	= 0;
	minFilter	= 0;
	magFilter	= 0;
	wrapS		= 0;
	wrapT		= 0;
	wrapR		= 0;

	texture		= NULL;
}

Texture::Texture( const char *filename, int filetype )
{
// These two lines should be moved
#ifdef	LINUX
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmaplevels );
#endif
	switch( filetype )
	{
	case DIB:
		{
			DIBINFO *bitmap = new DIBINFO;
			LoadDIBitmap( filename, bitmap );
			extFormat = GL_RGBA;
			width = bitmap->biHeader->biWidth;
			height = bitmap->biHeader->biHeight;
			intFormat = 0;
			minFilter = 0;
			magFilter = 0;
			wrapS = 0;
			wrapT = 0;
			wrapR = 0;
			texture = bitmap->rgba;
			bitmap->rgba = NULL;
			delete( bitmap );
		}
		break;
	case TGA:
		{
			TGAINFO *tga = new TGAINFO;
			LoadTGA( filename, tga );
			extFormat = GL_RGB;
			width = tga->usWidth;
			height = tga->usHeight;
			intFormat = 0;
			minFilter = 0;
			magFilter = 0;
			wrapS = 0;
			wrapT = 0;
			wrapR = 0;
			texture = tga->pixels;
			tga->pixels = NULL;
			delete( tga );
		}
		break;
	}
}

Texture::~Texture()
{
	glDeleteTextures( 1, &texID );
	delete[]( texture );
}

bool Texture::genTexObject()
{
	if( intFormat == 0 )
		intFormat = GL_RGB5;
	if( minFilter == 0 )
		minFilter = GL_NEAREST;
	if( magFilter == 0 )
		magFilter = GL_NEAREST;
	if( wrapS == 0 )
		wrapS = GL_REPEAT;
	if( wrapT == 0 )
		wrapT = GL_REPEAT;
	if( wrapR == 0 )
		wrapR = GL_REPEAT;

	// Hack until OpenGL 1.2 / GLU 1.3 is available for windows
#ifdef	LINUX
	glGenTextures( 1, &texID );
	glBindTexture( GL_TEXTURE_2D, texID );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
	if( (minFilter == GL_NEAREST) || (minFilter == GL_LINEAR) )
	{
		glTexImage2D( GL_TEXTURE_2D, 0, intFormat, width, height, 0,
			extFormat, GL_UNSIGNED_BYTE, texture );
	} else {
		gluBuild2DMipmapLevels( GL_TEXTURE_2D, intFormat, width, height,
			extFormat, GL_UNSIGNED_BYTE, 0, 0, mipmaplevels );
	}
#endif

#ifdef WIN32
	glGenTextures( 1, &texID );
	glBindTexture( GL_TEXTURE_2D, texID );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
	if( (minFilter == GL_NEAREST) || (minFilter == GL_LINEAR ) )
	{
		glTexImage2D( GL_TEXTURE_2D, 0, intFormat, width, height, 0,
			extFormat, GL_UNSIGNED_BYTE, texture );
	} else {
		gluBuild2DMipmaps( GL_TEXTURE_2D, intFormat, width, height,
			extFormat, GL_UNSIGNED_BYTE, texture );
	}
#endif

	delete[]( texture );
	texture = NULL;

	return( true );
}

bool Texture::genTexObject( const char *filename, int filetype )
{
#ifdef	LINUX
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmaplevels );
#endif
	switch( filetype )
	{
		case(DIB):
			{
				DIBINFO *bitmap = new DIBINFO;
				LoadDIBitmap( filename, bitmap );
				extFormat = GL_RGBA;
				width = bitmap->biHeader->biWidth;
				height = bitmap->biHeader->biHeight;
				texture = bitmap->rgba;
				bitmap->rgba = NULL;
				delete( bitmap );
			}
			return( genTexObject() );
		case(TGA):
			{
				TGAINFO *tga = new TGAINFO;
				LoadTGA( filename, tga );
				extFormat = tga->uiextFormat;
				width = tga->usWidth;
				height = tga->usHeight;
				texture = tga->pixels;
				tga->pixels = NULL;
				delete( tga );
			}
			return( genTexObject() );
	}

	return( false );
}

unsigned int Texture::getTexID()
{
	return( texID );
}

void Texture::setIntFormat( unsigned long format )
{
	intFormat = format;
}

bool Texture::setMinFilter( unsigned long filter )
{
	if( minFilter != filter )
	{
		minFilter = filter;
		return( false );
	}
	return( true );
}

bool Texture::setMagFilter( unsigned long filter )
{
	if( magFilter != filter )
	{
		magFilter = filter;
		return( false );
	}
	return( true );
}

bool Texture::setWrapS( unsigned long wrap )
{
	if( wrapS != wrap )
	{
		wrapS = wrap;
		return( false );
	}
	return( true );
}

bool Texture::setWrapT( unsigned long wrap )
{
	if( wrapT != wrap )
	{
		wrapT = wrap;
		return( false );
	}
	return( true );
}

bool Texture::setWrapR( unsigned long wrap )
{
	if( wrapR != wrap )
	{
		wrapR = wrap;
		return( false );
	}
	return( true );
}

void Texture::Apply()
{
	glBindTexture(GL_TEXTURE_2D, texID);
}

void Texture::LoadDIBitmap( const char *filename, DIBINFO *info )
{
	int i, j, imageSize, qIndex, pIndex;
	unsigned char *pixbuf;
	unsigned char *quadbuf;
	unsigned char *buf = new unsigned char[54];
	DIBFILEHEADER	*fheader = new DIBFILEHEADER;
	DIBINFOHEADER	*iheader = new DIBINFOHEADER;
	ifstream inDIB;
	
	inDIB.open( filename, ios::in | ios::binary | ios::nocreate );

	if( !inDIB.is_open() )
	{
		return;
	}

	inDIB.read( buf, 54 );

	fheader = (DIBFILEHEADER *)memcpy( fheader, buf, 14 );
	iheader = (DIBINFOHEADER *)memcpy( iheader, (buf + 14), 40 );

	quadbuf = new unsigned char[ iheader->biHeight * iheader->biWidth * 4 ];

	if( iheader->biBitCount == 24 )
	{
		imageSize = iheader->biHeight * iheader->biWidth * 3;

		pixbuf = new unsigned char[ imageSize ];

		inDIB.read( pixbuf, iheader->biWidth * iheader->biHeight * 3 );
		/*	DIB's are stored left to right, bottom to top.
		 *	quadbuf is stored left to right, top to bottom,
		 *	rgba.
		 */
		qIndex = 0;
		for( i = 0; i < iheader->biWidth; i++ )
		{
			for( j = 0; j < iheader->biHeight; j++, qIndex += 4 )
			{
				pIndex = (i*iheader->biWidth*3)+(j*3);
				quadbuf[qIndex+3] = 255;
				quadbuf[qIndex+2] = pixbuf[pIndex];
				quadbuf[qIndex+1] = pixbuf[pIndex+1];
				quadbuf[qIndex  ] = pixbuf[pIndex+2];
			}
		}
		delete( pixbuf );
	} else if( iheader->biBitCount == 8 )
	{
		qIndex = 0;
		int ctablesize = fheader->dataOffset - 54;
		unsigned char color = 0;
		unsigned char *ctable;
		ctable = new unsigned char[ctablesize];
		
		inDIB.read( ctable, ctablesize );

		for( i = 0; i < iheader->biHeight; i++ )
		{
			for( j = 0; j < iheader->biWidth; j++, qIndex += 4 )
			{
				inDIB.read( &color, 1 );
				quadbuf[qIndex+3] = 255;
				quadbuf[qIndex+2] = ctable[(color*4)];
				quadbuf[qIndex+1] = ctable[(color*4)+1];
				quadbuf[qIndex] = ctable[(color*4)+2];
			}
		}
		delete[]( ctable );
	}

	info->biHeader = iheader;
	info->rgba = quadbuf;
	
	inDIB.close();
	delete[]( buf );
	delete( fheader );
}

void Texture::WriteDIBitmap( DIBINFO *image )
{

}

void Texture::LoadTGA( const char *filename, TGAINFO *info )
{
	ifstream inTGA;
	TGAHEADER *tgaheader = new TGAHEADER;
	unsigned char *footer = new unsigned char[26];
	unsigned char *tga = NULL;


	inTGA.open( filename, ios::in | ios::binary | ios::nocreate );
	if( !inTGA.is_open() )
	{
		return;
	}

	inTGA.seekg( -26, ios::end );
	inTGA.read( footer, 26 );
	inTGA.seekg( 0, ios::beg );
	
	inTGA.read( (unsigned char *)tgaheader, sizeof( TGAHEADER ) );

	if( tgaheader->IDLength > 0 )
		inTGA.seekg( tgaheader->IDLength, ios::cur );

	if( tgaheader->CMapType == 0 )
	{
		long tgalength = (tgaheader->PixelDepth/8)*tgaheader->Height*tgaheader->Width;
		switch( tgaheader->PixelDepth/8 )
		{
		case 1:
			info->uiextFormat = GL_LUMINANCE;
			tga = new unsigned char[tgalength];
			inTGA.read( tga, tgalength );
			break;
		case 2:
			// info->uiextFormat = GL_UNSIGNED_SHORT_5_6_5;
			break;
		case 3:
			info->uiextFormat = GL_RGB;
			tga = new unsigned char[tgalength];
			inTGA.read( tga, tgalength );
			break;
		case 4:
			info->uiextFormat = GL_RGBA;
			tga = new unsigned char[tgalength];
			inTGA.read( tga, tgalength );
			break;
		}
	}

	info->usHeight = tgaheader->Height;
	info->usWidth = tgaheader->Width;
	info->pixels = tga;

	inTGA.close();
	delete( tgaheader );
	delete[]( footer );
}

void Texture::WriteTGA( TGAINFO *info )
{

}

#endif