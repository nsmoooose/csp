#ifdef OGL
#ifndef __GLFONT2D_H_
#define __GLFONT2D_H_

#include "graphics/typesgraphics.h"

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#include "GLTexture2D.h"

/** \class GLFont2D
 * OpenGL Font class
 */
class GLFont2D : public Font2D
{
public:
	GLFont2D(StandardGraphics * pDisplay);	
	virtual ~GLFont2D();
	virtual void Create(const char * filename, int type);
	virtual void DrawTextF(char *buffer, short bufferbytecount, _Rect rect, short LeftCenterRight, _Color*color);
	virtual void Begin();
	virtual void End();

private:
	void drawChar( const char *, int );
	void positionChar( int );
	unsigned long getTexID();


	GLuint		dlbase;
	GLTexture2D		*fontTex;
	int			width;
	int			lastSpace;
	int			strS;
	int			strE;
	float		locy;

	int         m_immediateMode;

	StandardGraphics * m_pDisplay;
};








#endif
#endif