#ifndef __GLTEXTURE_H_
#define __GLTEXTURE_H_

#include "Framework.h"
#include "TgaInfo.h"
#include "Bitmap.h"

class GLTexture :  public StandardTexture
{

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

public:



	GLTexture();
	~GLTexture();

	short Initialize(const char * filename, int filetype);
        short Initialize(const char * filename);
        short Initialize(const char * filename, _MIPMAP Levels, StandardColor * colorKey);
	void Uninitialize();

	void Apply();


	// File Types
	static int const	DIB;
	static int const	TGA;

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



};

#endif
