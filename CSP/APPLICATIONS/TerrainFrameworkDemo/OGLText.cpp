///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   <file-description>
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////

#include "OGLText.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <assert.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>

COGLText OGLText;

COGLText::COGLText()
{
}

COGLText::~COGLText()
{
	glDeleteTextures(256, m_puiTextureList);
	delete []m_puiTextureList;
	glDeleteLists(m_uiCharLists, 256);
}

GLvoid COGLText::Print(const char *fmt, ...)				
{
	char		text[256];								
	va_list		ap;										

	if (fmt == NULL) return;											

	va_start(ap, fmt);									
	    vsprintf(text, fmt, ap);						
	va_end(ap);												

	RenderText(text);
}

void COGLText::Print(float fX, float fY, const char *fmt, ...)
{
	char		text[256];								
	va_list		ap;										

	if (fmt == NULL) return;											

	va_start(ap, fmt);									
	    vsprintf(text, fmt, ap);						
	va_end(ap);												

	glPushMatrix();
	glLoadIdentity();
	//gluLookAt(0, 0, 0, 0, 1, 0, 0, 0, 1);
	glTranslatef(-1, 1, 0);
	glScalef(0.023f, 0.05f, 1);
	//glScalef(2, 2, 2);
	glTranslatef(fX, -1-fY, 0);
	RenderText(text);
	glPopMatrix();
}

void COGLText::RenderText(char *cText)
{
	// disable projection matrix - we absolutely need an orthonogal view
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushAttrib(GL_LIST_BIT | GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);		
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glListBase(m_uiCharLists);								
	glCallLists(strlen(cText), GL_UNSIGNED_BYTE, cText);	
	glPopAttrib();		
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

char* COGLText::LoadCharacters(char *cFilename)
{
	char	*pCharBuffer;
	char	*pCharArray;
	FILE	*pFile;
	int		iResult;
	int		iPixelCount		= 256*16*16;

	// load fontfile
	pFile = fopen(cFilename, "r");
	pCharBuffer = new char[iPixelCount*3];
	iResult = fread(pCharBuffer, iPixelCount*3, 1, pFile);
	assert(iResult==1 && "fontfile not found/unable to read");
	fclose(pFile);

	// reorganize chars and add alpha values
	pCharArray = new char[iPixelCount*4];
	int iIndex = 0;
	for (int y=0; y<16; y++)
		for (int x=0; x<16; x++)
		{
			char *pSrc = pCharBuffer+y*256*3*16+x*16*3;
			char *pDst = pCharArray+iIndex*256*4;
			for (int y2=0; y2<16; y2++)
				for (int x2=0; x2<16; x2++)
				{
					pDst[y2*16*4+x2*4+0] = pSrc[y2*256*3+x2*3+0];
					pDst[y2*16*4+x2*4+1] = pSrc[y2*256*3+x2*3+1];
					pDst[y2*16*4+x2*4+2] = pSrc[y2*256*3+x2*3+2];
					pDst[y2*16*4+x2*4+3] = pSrc[y2*256*3+x2*3+2]; // alpha-value!
				}
			iIndex++;
		}
	delete pCharBuffer;
	return pCharArray;
}

void COGLText::CreateDisplayLists(char *pCharArray)
{
	m_puiTextureList	= new unsigned int[256];
    int i;

	// create textures
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(256, m_puiTextureList);
	for (i=0; i<256; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_puiTextureList[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, pCharArray+i*256*4);
	}
	delete []pCharArray;

	m_uiCharLists = glGenLists(256);
	for (i=0; i<256; i++)
	{
		glNewList(m_uiCharLists+i, GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D, m_puiTextureList[i]);
			glColor3f(1, 1, 1);
			glBegin(GL_QUADS);
				glTexCoord2f(0.1f, 1);
				glVertex2f(0, 0);
				glTexCoord2f(0.1f, 0);
				glVertex2f(0, 1);
				glTexCoord2f(0.9f, 0);
				glVertex2f(1, 1);
				glTexCoord2f(0.9f, 1);
				glVertex2f(1, 0);
			glEnd();
			glTranslatef(1, 0, 0);
		glEndList();
	}
}

void COGLText::Init()
{
	char *pCharArray;
	pCharArray = LoadCharacters("Font.raw");
	CreateDisplayLists(pCharArray);
}

/*GLvoid BuildFont(GLvoid)								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID

	base = glGenLists(96);								// Storage For 96 Characters

	font = CreateFont(	-24,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Courier New");					// Font Name

	SelectObject(hDC, font);							// Selects The Font We Want

	wglUseFontBitmaps(hDC, 32, 96, base);				// Builds 96 Characters Starting At Character 32
}

GLvoid KillFont(GLvoid)									// Delete The Font
{
	glDeleteLists(base, 96);							// Delete All 96 Characters
}

GLvoid glPrint(const char *fmt, ...)					// Custom GL "Print" Routine
{
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base - 32);								// Sets The Base Character to 32
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
}*/
