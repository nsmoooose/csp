// Texture.h: Schnittstelle für die Klasse CTexture.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURE_H__9A59B5F2_ABD1_4FC5_9F03_CDF48678B1FB__INCLUDED_)
#define AFX_TEXTURE_H__9A59B5F2_ABD1_4FC5_9F03_CDF48678B1FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <gl\gl.h>												// Header File For The OpenGL32 Library

class CTexture  
{
public:
	CTexture();
	virtual ~CTexture();

	GLuint	iGLTexHandle;
	bool	bIsAvailable;
	float  *pfCoverageMap;

protected:

};

#endif // !defined(AFX_TEXTURE_H__9A59B5F2_ABD1_4FC5_9F03_CDF48678B1FB__INCLUDED_)
