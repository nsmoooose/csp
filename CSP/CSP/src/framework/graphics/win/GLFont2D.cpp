#ifdef OGL
#ifdef WIN32
#include <windows.h>
#endif

#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "GLFont2D.h"
#include "Texture.h"
#include "graphics/gl_globals.h"

extern GLStateMachine	statesGL;

GLFont2D::GLFont2D(StandardGraphics * pDisplay)
: Font2D(pDisplay)
{
	m_immediateMode = TRUE;
	m_pDisplay = pDisplay;
}

void GLFont2D::Create( const char * filename, int type )
{
	float x, y;
	float delta = 1.0 / 16.0f;
	float ll[2] = { 0.0f, 0.0f };
	float ul[2] = { 0.0f, 9.0f };
	float ur[2] = { 8.0f, 9.0f };
	float lr[2] = { 8.0f, 0.0f };

//	fontTex = pDisplay->CreateTexture2D;
	fontTex = new GLTexture2D(m_pDisplay);
	fontTex->setWrapS( GL_CLAMP );
	fontTex->setWrapT( GL_CLAMP );
	fontTex->setMagFilter( GL_LINEAR );
	fontTex->setMinFilter( GL_LINEAR );
	fontTex->genTexObject( filename, type );

	dlbase = glGenLists( 256 );
	if( fontTex->getTexID() != statesGL.boundTex[0] )
		glBindTexture( GL_TEXTURE_2D, fontTex->getTexID() );

	for( int i = 0; i < 256; i++ )
	{
		x = (float)(i%16)/16.0f;
		y = (float)(i/16)/16.0f;

		glNewList( dlbase + i, GL_COMPILE );
		glBegin( GL_TRIANGLE_STRIP );
			glTexCoord2f( x, 1 - y - delta );
			glVertex2fv( ll );
			glTexCoord2f( x + delta, 1 - y - delta );
			glVertex2fv( lr );
			glTexCoord2f( x, 1 - y );
			glVertex2fv( ul );
			glTexCoord2f( x + delta, 1 - y );
			glVertex2fv( ur );
		glEnd();
		glTranslatef( 8.0f, 0.0f, 0.0f );
		glEndList();
	}
}

GLFont2D::~GLFont2D()
{
	glDeleteLists( dlbase, 256 );
	delete( fontTex );
}

void GLFont2D::Begin()
{

}

void GLFont2D::End()
{

}

void GLFont2D::DrawTextF(char *string, short numChars, _Rect rect, short center, _Color *textColor)
//void Font::displayText( const char *string, int numChars, _Rect rect, int center, _Color *textColor )
{
	strS = strE = lastSpace = 0;
	locy = rect.y2 - 10.0f;
	width = rect.x2 - rect.x1;

	glBindTexture( GL_TEXTURE_2D, fontTex->getTexID() );
	glEnable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );
	glColor3fv( (float *)textColor );
//	glMatrixMode( GL_PROJECTION );
//	glPushMatrix();
//	glLoadIdentity();
//	glOrtho( rect.x1, rect.x2, rect.y1, rect.y2, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glListBase( dlbase );

	for( int index = 0; index < numChars; index++ )
	{
		if( string[index] == '\n' )
		{
			if( strS == index )
			{
				locy -= 10.0f;
				strS++;
				strE = strS;
				lastSpace = strS;
			} else {
				positionChar( center );
				drawChar( string + strS, strE - strS );
				strS = index + 1;
				strE = lastSpace = strS;
				locy -= 10.0f;
			}
		} else if( string[index] == ' ' )
		{
			strE++;
			lastSpace = index;
		} else {
			strE++;
			if( (strE - strS)*8 > width )
			{
				strE = lastSpace;
				positionChar( center );
				drawChar( string + strS, strE - strS );
				index = strS = strE = ++lastSpace;
			}
		}
	}
	if( strS != strE )
	{
		positionChar( center );
		drawChar( string + strS, strE - strS );
	}
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

void GLFont2D::drawChar( const char *string, int length )
{
	glCallLists( length, GL_BYTE, string );
}

void GLFont2D::positionChar( int center )
{
	switch( center )
	{
		case( 0 ):
			glLoadIdentity();
			glTranslatef( 0, locy, 0 );
			break;
		case( 1 ):
			glLoadIdentity();
			glTranslatef( (float)((width - ((strE-strS)*8)) / 2), locy, 0 );
			break;
		case( 2 ):
			glLoadIdentity();
			glTranslatef( (float)(width - (strE-strS)*8) - 3, locy, 0 );
			break;
	}
}

unsigned long GLFont2D::getTexID()
{
	return( fontTex->getTexID() );
}

#endif
