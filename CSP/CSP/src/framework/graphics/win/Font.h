/*
		Font class
*/

#ifndef __GLFONT_H__
#define __GLFONT_H__

#ifdef	WIN32
#include <windows.h>
#endif

#include <gl/gl.h>
#include "Texture.h"
#include "graphics/TypesGraphics.h"

class Font
{
public:
	Font( const char *, int );
	~Font();
	
	void displayText( const char *, int, _Rect, int, _Color * );

	unsigned long getTexID();

private:
	void drawChar( const char *, int );
	void positionChar( int );

	GLuint		dlbase;
	Texture		*fontTex;
	int			width;
	int			lastSpace;
	int			strS;
	int			strE;
	float		locy;
};

#endif