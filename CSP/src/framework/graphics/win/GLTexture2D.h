#ifndef __GLTEXTURE2D_H_
#define __GLTEXTURE2D_H_

#include "graphics/typesgraphics.h"

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#include "tgainfo.h"
#include "bitmap.h"

class GLTexture2D : public Texture2D
{
public:

	// File Types
	static int const	DIB;
	static int const	TGA;

	GLTexture2D(StandardGraphics * pDisplay);
//	Texture( const char *, int );
	virtual ~GLTexture2D();
	virtual void Create(const char * filename, int filetype);
	virtual void Apply();
	virtual void Destroy();

public:

	bool genTexObject();
	bool genTexObject( const char *, int );

	unsigned int getTexID();

	void setIntFormat( unsigned long );
	bool setMinFilter( unsigned long );
	bool setMagFilter( unsigned long );
	bool setWrapS( unsigned long );
	bool setWrapT( unsigned long );
	bool setWrapR( unsigned long );

	static void LoadDIBitmap( const char *, DIBINFO * );
	static void WriteDIBitmap( DIBINFO * );
	static void LoadTGA( const char *, TGAINFO * );
	static void WriteTGA( TGAINFO * );


private:
	unsigned int		texID;
	unsigned int		width;
	unsigned int		height;
	unsigned int		intFormat;
	unsigned int		extFormat;
	unsigned int		minFilter;
	unsigned int		magFilter;
	unsigned int		wrapS;
	unsigned int		wrapT;
	unsigned int		wrapR;

	static short		mipmaplevels;

	void *texture;

};

#endif