#ifndef		__TEXTURE_H__
#define		__TEXTURE_H__

#include "bitmap.h"
#include "tgainfo.h"

class Texture
{
public:

	// File Types
	static int const	DIB;
	static int const	TGA;

	Texture();
	Texture( const char *, int );
	~Texture();

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

	virtual void Apply();

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