///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of CGLTexture
//   class represents an OpenGL texture
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//	7/16/2001	implemented basic functionality - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined GLTEXTURE_H
#define GLTEXTURE_H

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "Texture.h"
#include "global.h"


class CGLTexture : public CTexture
{
public:
	CGLTexture();
	CGLTexture(char *cFilename, uint uiGLTexFormat = GL_RGBA, uint uiGLMagFilter = GL_LINEAR, uint uiGLMinFilter = GL_LINEAR);

	virtual bool Create();
	virtual void Destroy();
	virtual void Bind();

	inline uint GetGLHandle() { if (m_hGLTexture==0) GenerateGLTex(); return m_hGLTexture; }

protected:
	uint	m_hGLTexture;			// OpenGL-texture-handle

	void DeleteGLTex();
private:
	uint	m_uiGLTexFormat;		// GL's internal texture format
	uint	m_uiGLMagFilter;		// GL-filter used for texture magnification
	uint	m_uiGLMinFilter;		// GL-filter used for texture minification

	void Init();
	void GenerateGLTex();
};


#endif // GLTEXTURE_H
