#ifdef OGL

//#include <streamb>
#include "Framework.h"


#include "GLTexture.h"
#include "Bitmap.h"
#include "TgaInfo.h"

using namespace std;

int const GLTexture::DIB	= 1;
int const GLTexture::TGA	= 2;
short GLTexture::mipmaplevels = 0;

GLTexture::GLTexture()
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLTexture::GLTexture()");

  texID		= 0;
  width		= 0;
  height	= 0;
  intFormat	= 0;
  extFormat	= 0;
  minFilter	= 0;
  magFilter	= 0;
  wrapS		= 0;
  wrapT		= 0;
  wrapR		= 0;
  texture		= NULL;
}


short GLTexture::Initialize(const char *filename )
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLTexture::Initialize - filename: "
      << filename);

  Initialize(filename, 0, 0);

  return 0;
}

short GLTexture::Initialize(const char *filename, int filetype )
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLTexture::Initialize - filename: "
      << filename << ", filetype: " << filetype);

  Initialize(filename, 0, 0);
  return _A_OK;

}


short GLTexture::Initialize(const char *filename, _MIPMAP Levels, StandardColor * colorkey )
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLTexture::Initialize - filename: "
      << filename << ", mipmap levels: " << Levels << ", colorkey: " << colorkey);

  int filetype = 0;

  // get the file extension to determine the filetype.
  char * extstr = strrchr(filename, '.');

  char * extstrcpy = (char*)malloc(strlen(extstr)+1);

  strcpy (extstrcpy, extstr);

  for (int i=0; i<strlen(extstrcpy); i++)
    {
      extstrcpy[i] = tolower(extstrcpy[i]);
    }
  if (strcmp(extstrcpy, ".bmp") == 0)
    filetype = GLTexture::DIB;
  else if (strcmp(extstrcpy, ".tga") == 0)
    filetype = GLTexture::TGA;
  else 
    return 1;          // could not determine the file type or its unsupported.


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

	// hardcoded values for now.
    minFilter = GL_LINEAR;
	magFilter = GL_LINEAR;

	genTexObject();

  return 0;
}


GLTexture::~GLTexture()
{
	glDeleteTextures( 1, &texID );
	delete[]( texture );
}

void GLTexture::Apply()
{
	glBindTexture(GL_TEXTURE_2D, getTexID());
}

void GLTexture::Uninitialize()
{

}

///////////////////////////////////////
// protected member classes.

bool GLTexture::genTexObject()
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
#ifdef	__LINUX__


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
	  // until new glu is available.
	  //		gluBuild2DMipmapLevels( GL_TEXTURE_2D, intFormat, width, height,
	  //	extFormat, GL_UNSIGNED_BYTE, 0, 0, mipmaplevels );
		gluBuild2DMipmaps( GL_TEXTURE_2D, intFormat, width, height,
			extFormat, GL_UNSIGNED_BYTE, texture );
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

bool GLTexture::genTexObject( const char *filename, int filetype )
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

unsigned int GLTexture::getTexID()
{
	return( texID );
}

void GLTexture::setIntFormat( unsigned long format )
{
	intFormat = format;
}

bool GLTexture::setMinFilter( unsigned long filter )
{
	if( minFilter != filter )
	{
		minFilter = filter;
		return( false );
	}
	return( true );
}

bool GLTexture::setMagFilter( unsigned long filter )
{
	if( magFilter != filter )
	{
		magFilter = filter;
		return( false );
	}
	return( true );
}

bool GLTexture::setWrapS( unsigned long wrap )
{
	if( wrapS != wrap )
	{
		wrapS = wrap;
		return( false );
	}
	return( true );
}

bool GLTexture::setWrapT( unsigned long wrap )
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLTexture::SET0()");

  if( wrapT != wrap )
	{
		wrapT = wrap;
		return( false );
	}
	return( true );
}

bool GLTexture::setWrapR( unsigned long wrap )
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLTexture::setWrapR() - wrap: " << wrap);

  if( wrapR != wrap )
	{
		wrapR = wrap;
		return( false );
	}
	return( true );
}

void GLTexture::LoadDIBitmap( const char *filename, DIBINFO *info )
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLTexture::LoadDIBitmap() - filename: "
      << filename );

	int i, j, imageSize, qIndex, pIndex;
	char *pixbuf;
	unsigned char *quadbuf;
	char *buf = new char[54];
	DIBFILEHEADER	*fheader = new DIBFILEHEADER;
	DIBINFOHEADER	*iheader = new DIBINFOHEADER;
	ifstream inDIB;
	
	inDIB.open( filename, ios::in | ios::binary /* | ios::nocreate */);

	if( !inDIB.is_open() )
	{
		return;
	}

	inDIB.read( buf, 54 );

	fheader = (DIBFILEHEADER *)memcpy( fheader, buf, 14 );
	iheader = (DIBINFOHEADER *)memcpy( iheader, (buf + 14), 40 );

	quadbuf = new unsigned char[ iheader->biHeight * iheader->biWidth * 4 ];

        fprintf(stdout, "GLTexture::LoadDIBitmap() : Height = %d, Width = %d\n",
		iheader->biHeight, iheader->biWidth);


	if( iheader->biBitCount == 24 )
	{
	  fprintf(stdout, "GLTexture::LoadDIBitmap() : loading 24 bit bitmap\n");
		imageSize = iheader->biHeight * iheader->biWidth * 3;

		pixbuf = new char[ imageSize ];

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
				pIndex = (i*iheader->biHeight*3)+(j*3);
				quadbuf[qIndex+3] = 255;
				quadbuf[qIndex+2] = pixbuf[pIndex];
				quadbuf[qIndex+1] = pixbuf[pIndex+1];
				quadbuf[qIndex  ] = pixbuf[pIndex+2];
			}
		}
		delete [] pixbuf;
	} else if( iheader->biBitCount == 8 )
	{
	  fprintf(stdout, "GLTexture::LoadDIBitmap() : loading 8 bit bitmap\n");
		qIndex = 0;
		int ctablesize = fheader->dataOffset - 54;
		char color = 0;
		char *ctable;
		ctable = new char[ctablesize];
		
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
        fprintf(stdout, "GLTexture::LoadDIBitmap() : finished loading BMP bitmap\n");

	delete[]( buf );
	delete( fheader );
}

void GLTexture::WriteDIBitmap( DIBINFO *image )
{

}

void GLTexture::LoadTGA( const char *filename, TGAINFO *info )
{

  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLTexture::LoadTga() - filename: "
      << filename);

	ifstream inTGA;
	TGAHEADER *tgaheader = new TGAHEADER;
	char *footer = new char[26];
	char *tga = NULL;


	inTGA.open( filename, ios::in | ios::binary /*| ios::nocreate */ );
	if( !inTGA.is_open() )
	{
	  fprintf(stdout, "GLTexture::LoadTGA() : Failed to open file\n");
		return;
	}

	inTGA.seekg( -26, ios::end );
	inTGA.read( footer, 26 );
	inTGA.seekg( 0, ios::beg );
	
	inTGA.read( (char *)tgaheader, sizeof( TGAHEADER ) );

	if( tgaheader->IDLength > 0 )
		inTGA.seekg( tgaheader->IDLength, ios::cur );

	if( tgaheader->CMapType == 0 )
	{
		unsigned char tc;
		int i;
		long tgalength = (tgaheader->PixelDepth/8)*tgaheader->Height*tgaheader->Width;
		switch( tgaheader->PixelDepth/8 )
		{
		case 1:
		  fprintf(stdout, "GLTexture::LoadTGA() : format is LUMINANCE\n");
			info->uiextFormat = GL_LUMINANCE;
			tga = new char[tgalength];
			inTGA.read( tga, tgalength );
			break;
		case 2:
			// info->uiextFormat = GL_UNSIGNED_SHORT_5_6_5;
			break;
		case 3:
		  fprintf(stdout, "GLTexture::LoadTGA() : format is RGB\n");
			info->uiextFormat = GL_RGB;
			tga = new char[tgalength];
			inTGA.read( tga, tgalength );
			for( i = 0; i < tgalength; i += 3 )
			{
				tc = tga[i];
				tga[i] = tga[i + 2];
				tga[i + 2] = tc;
			}
			break;
		case 4:

		  fprintf(stdout, "GLTexture::LoadTGA() : format is RGBA\n");
			info->uiextFormat = GL_RGBA;
			tga = new char[tgalength];
			inTGA.read( tga, tgalength );
			break;
		}
	}

        fprintf(stdout, "GLTexture::LoadTGA() : Height = %d, Width = %d, PixelDepth = %d\n",
		tgaheader->Height, tgaheader->Width, tgaheader->PixelDepth);

	info->usHeight = tgaheader->Height;
	info->usWidth = tgaheader->Width;
	info->pixels = (unsigned char *)tga;


	inTGA.close();
	delete( tgaheader );
	delete[]( footer );
}

void GLTexture::WriteTGA( TGAINFO *info )
{

}

#endif
