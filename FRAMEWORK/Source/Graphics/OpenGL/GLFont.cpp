///////////////////////////////////////////////////////////////////////////
//
//   GLFont  - CSP - http://csp.homeip.net
//
//   coded by GKW, Wolverine
//
///////////////////////////////////////////////////////////////////////////
//   
//   This code represents the OpenGL Implementation of a Font class.
//   A bitmap with the proper characters is loaded to initialize the
//   font. When text is displayed with this font the portion of the bitmap
//   is rendered to the display.
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//
///////////////////////////////////////////////////////////////////////////



#ifdef OGL

#include "GLFont.h"
#include "GLTexture.h"
#include "GLGlobals.h"

/**
 * \fn GLFont::GLFont(StandardGraphics* Graphics)
 * \brief GLFont constructor
 * \param Graphics The OpenGL StandardGraphics to display the font.
 */
GLFont::GLFont(StandardGraphics* Graphics)
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLFont::GLFont()");

  p_Graphics = Graphics;

  m_immediateMode = true;
}

/** 
 * \fn void GLFont::Create( const char * filename, int type )
 * \brief Create a font object
 * \param filename The name of the bitmap to use for the font.
 * \param type The type of bitmap file.
 */
void GLFont::Create( const char * filename, int type )
{
  CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLFont::Create()");

  m_char_width=8;
  m_char_height=16;

	float x, y;
	float delta = 1.0 / 16.0f;
	float ll[2] = { 0.0f, 0.0f };
	float ul[2] = { 0.0f, 9.0f };
	float ur[2] = { 8.0f, 9.0f };
	float lr[2] = { 8.0f, 0.0f };

//	fontTex = pDisplay->CreateTexture2D;
	fontTex = new GLTexture();
//	fontTex->Initialize(filename, GLTexture::TGA);
	fontTex->setWrapS( GL_CLAMP );
	fontTex->setWrapT( GL_CLAMP );
	fontTex->setMagFilter( GL_LINEAR );
	fontTex->setMinFilter( GL_LINEAR );
	fontTex->genTexObject( filename, type );
//	fontTex->genTexObject();

	dlbase = glGenLists( 256 );
	if( fontTex->getTexID() != statesGL.boundTex[0] )
		glBindTexture( GL_TEXTURE_2D, fontTex->getTexID() );

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

/** 
 * \fn GLFont::~GLFont()
 * \brief The Destructor for GLFont. The display lists are deleted and the font bitmap is deleted.
 */
GLFont::~GLFont()
{
	glDeleteLists( dlbase, 256 );
	delete( fontTex );
}

/**
 * \fn void GLFont::Begin()
 * \brief Not currently used. This should be used to avoid multiple state changes with several display lines.
 */
void GLFont::Begin()
{

}

/**
 * \fn void GLFont::End()
 * \brief Not currently used. This should be used to avoid multiple state changes with several display lines.
 */
void GLFont::End()
{

}

/** 
 * \fn void GLFont::DrawTextF(char *string, _Rect rect, short horiz_center, short vert_center, StandardColor *textColor)
 * \brief Prints a Text string to the rendering display.
 * \param string The string to print out.
 * \param rect The surrounding rectangle around the text.
 * \param horiz_center a flag specifying the horiz centering.
 * \param vert_center a flag specifying the vert centering.
 * \param textColor The color of the text.
 */
void GLFont::DrawTextF(char *string, _Rect rect, short horiz_center, short vert_center, StandardColor *textColor)
{

    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLFont::DrawTextF() - string " << string);


    int numChars = strlen(string);
	strS = strE = lastSpace = 0;
	locy = rect.y2 - 10.0f;
  // to get absolute width you need to times rect.x1 and rect.x2 * p_Init->Width
	_GRAPHICS_INIT_STRUCT* init = p_Graphics->GetInit();

	m_box_width = (int)rect.x2 - (int)rect.x1;
	m_box_height = (int)rect.y2 - (int)rect.y1;
	m_box_left = rect.x1;
	m_box_top  =  init->Height - rect.y1 - 30;

	// if we are in wireframe mode temp disable it.
	if(GLStateMachine::GetWireframe())
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindTexture( GL_TEXTURE_2D, fontTex->getTexID() );
	glEnable( GL_BLEND );

	// if we are in depthtest mode temp disable it.
	if (GLStateMachine::GetZBuffer())
		glDisable( GL_DEPTH_TEST );

	glColor4ub( textColor->r, textColor->g, textColor->b, textColor->a );

	// set the ortho transform to be the full screen.
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0 , init->Width, 0 , init->Height, -1, 1 );
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
				positionChar( horiz_center, vert_center );
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
			if( (strE - strS)*m_char_width > m_box_width )
			{
				strE = lastSpace;
				positionChar( horiz_center, vert_center );
				drawChar( string + strS, strE - strS );
				index = strS = strE = ++lastSpace;
			}
		}
	}
	if( strS != strE )
	{
		positionChar( horiz_center, vert_center );
		drawChar( string + strS, strE - strS );
	}
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	// if we are in wireframe mode reenable it.
	if (GLStateMachine::GetWireframe())
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDisable( GL_BLEND );

	// if we are in depthtest mode temp disable it.
	if (GLStateMachine::GetZBuffer())
		glEnable( GL_DEPTH_TEST );

}

/** 
 * \fn void GLFont::drawChar( const char *string, int length )
 * \brief prints out a group of characters. Usually a single line.
 * \param string the string to print out.
 * \param length the length of characters to print out.
 */
void GLFont::drawChar( const char *string, int length )
{
    CSP_LOG(CSP_OPENGL, CSP_TRACE, "GLFont::drawChar()");
	glCallLists( length, GL_BYTE, string );
}

/** 
 * \fn void GLFont::positionChar( int horiz_center, int vert_center )
 * \brief Positions before drawing text.
 * \param horiz_center Flag specifying the horiz centering.
 * \param vert_center Flag specifying the vert centering. 
 */
void GLFont::positionChar( int horiz_center, int vert_center )
{
	switch( horiz_center )
	{
		case( 0 ):
			glLoadIdentity();
			glTranslatef( m_box_left, m_box_top+locy, 0 );
			break;
		case( 1 ):
			glLoadIdentity();
			glTranslatef( m_box_left + (float)((m_box_width - ((strE-strS)*m_char_width)) / 2), m_box_top+locy, 0 );
			break;
		case( 2 ):
			glLoadIdentity();
			glTranslatef( m_box_left + (float)(m_box_width - (strE-strS)*m_char_width) - 3, m_box_top+locy, 0 );
			break;
	}
}

/** 
 * \fn unsigned long GLFont::getTexID()
 * \brief returns the fonts texture ID.
 */
unsigned long GLFont::getTexID()
{
	return( fontTex->getTexID() );
}

#endif
