///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   definition of CGLTexture
//   class represents an OpenGL texture
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//	8/5/2001	implemented basic functionality - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#include "GLTexture.h"



CGLTexture::CGLTexture()
{
	Init();
}

CGLTexture::CGLTexture(char *cFilename, uint uiGLTexFormat, uint uiGLMagFilter, uint uiGLMinFilter)
	: CTexture(cFilename)
{
	Init();
	m_uiGLTexFormat = uiGLTexFormat;
	m_uiGLMagFilter = uiGLMagFilter;
	m_uiGLMinFilter = uiGLMinFilter;
}

void CGLTexture::Init()
{
	m_hGLTexture = 0;
	m_uiGLTexFormat = 0;
	m_uiGLTexFormat = GL_RGB;
	m_uiGLMagFilter = GL_LINEAR;
	m_uiGLMinFilter = GL_LINEAR;
}

bool CGLTexture::Create()
{
	if (!CTexture::Create()) return false;
	GenerateGLTex();
	return true;
}

void CGLTexture::Destroy()
{
	DeleteGLTex();
	CTexture::Destroy();
}

void CGLTexture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, GetGLHandle());
}

void CGLTexture::GenerateGLTex()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &m_hGLTexture);
	
	glBindTexture(GL_TEXTURE_2D, m_hGLTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_uiGLMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_uiGLMinFilter); 
	glTexImage2D(GL_TEXTURE_2D, 0, m_uiGLTexFormat, m_usWidth, m_usHeight, 0, (m_ucPixeldepth==24 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, m_pData);
	/*char text[20];
	sprintf(text, "t%d.raw", g++);
	FILE *f = fopen(text, "wb");
	fwrite(m_pData, m_usWidth*m_usHeight*3, 1, f);
	fclose(f);*/
}

void CGLTexture::DeleteGLTex()
{
	glDeleteTextures(1, &m_hGLTexture);
	m_hGLTexture = 0;
}
