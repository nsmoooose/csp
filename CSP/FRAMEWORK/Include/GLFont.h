#ifndef __GLFONT_H_
#define __GLFONT_H_

#ifdef OGL

#include "Framework.h"
#include "GLTexture.h"

//class StandardFont;


/** \class GLFont
 * OpenGL Font class
 */
class GLFont  /* : public StandardFont */
{
public:
	GLFont(StandardGraphics* Graphics);	
	virtual ~GLFont();

	virtual void Create(const char * filename, int type);
	virtual void DrawTextF(char *buffer, _Rect rect, short LeftCenterRight, short TopCenterBottom,  StandardColor* color);
	virtual void Begin();
	virtual void End();

private:
	void drawChar( const char *, int );
	void positionChar( int, int );
	unsigned long getTexID();


	GLuint		dlbase;
	GLTexture		*fontTex;
	int			m_box_width;
	int                     m_box_height;
	int                     m_box_left;
        int                     m_box_top;
	int                     m_char_width;
        int                     m_char_height;
	int			lastSpace;
	int			strS;
	int			strE;
	float		locy;

	int         m_immediateMode;

      StandardGraphics *p_Graphics;
 
};


#endif
#endif
