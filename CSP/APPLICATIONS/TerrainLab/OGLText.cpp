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

#include <windows.h>
#include <assert.h>
#include <gl/gl.h>
#include <stdio.h>



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
	glTranslatef(-1, 1, 0);
	glScalef(0.023f, 0.05f, 1);
	//glScalef(2, 2, 2);
	glTranslatef(fX, -1-fY, 0);
	RenderText(text);
	glPopMatrix();
}

void COGLText::RenderText(char *cText)
{
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
	assert(iResult!=iPixelCount*3 && "fontfile not found/unable to read");
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

	// create textures
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(256, m_puiTextureList);
	for (int i=0; i<256; i++)
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

