///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   definition of CTexture
//   class loads and represents a texture (loads TGA-files)
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//	7/18/2001	implemented basic functionality and adapted structure for
//              new resource-manager - zzed
//  8/5/2001	moved OpenGL-handling to CGLTexture
//	8/7/2001	added DevIL (former OpenIL) support - Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////

#include "Texture.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

CTexture::CTexture() : CBaseResource()
{
	Init();
	m_bDisposable = true;
}

CTexture::CTexture(char* cFilename) 
{
	Init();
	m_cFilename = cFilename;
	m_bDisposable = true;
}

CTexture::CTexture(ushort usWidth, ushort usHeight, uchar usPixeldepth, uchar *pData)
{
	Init();
	m_usWidth = usWidth;
	m_usHeight = usHeight;
	m_ucPixeldepth = usPixeldepth;
	m_pData = pData;
	m_bDisposable = false;
}

void CTexture::Init()
{
	m_cFilename = 0;
	m_pData = 0;
	m_ucPixeldepth = 0;
	m_usHeight = 0;
	m_usWidth = 0;	
}

bool CTexture::Create()
{
	if (!m_bDisposable) return true;
	try 
	{
		LoadImage(m_cFilename);
	}
	catch (LoadingFailed) 
	{
		return false;
	}
	return true;
}

void CTexture::Destroy()
{
	if (m_pData) delete[] m_pData;
	m_pData = 0;
}

bool CTexture::Recreate()
{
	return Create();
}

void CTexture::Dispose()
{
	if (m_bDisposable) Destroy();
}

size_t CTexture::GetSize()
{
	if (m_pData) return m_usHeight*m_usWidth*(m_ucPixeldepth/8);
	else return 0;
}

bool CTexture::IsDisposed()
{
	return m_pData ? false : true; 
}

void CTexture::LoadImage(char *cFilename)
{
	ilGenImages(1, &m_uiILTexName);					// Grab a new image name.
	ilBindImage(m_uiILTexName);						// Bind the image, just like a texture in GL

	ilLoadImage(cFilename);							// now load it!
	if (!(ilGetError() == IL_NO_ERROR))				// shit, error occured
	{
		printf("\n\n\nerror while opening file '%s'! Exiting program ....\n\n", cFilename);
		exit(-1);
	}

	ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);		// convert it to RGB format (important for TGA, as TGA is stored in BGR format)

	m_usWidth		= ilGetInteger(IL_IMAGE_WIDTH);
	m_usHeight		= ilGetInteger(IL_IMAGE_HEIGHT);
	m_ucPixeldepth	= ilGetBoolean(IL_IMAGE_BITS_PER_PIXEL);

	uint uiTotal = m_usWidth*m_usHeight*m_ucPixeldepth/8;	// number of bytes to read
	m_pData = new uchar[uiTotal];
	memcpy(m_pData, ilGetData(), uiTotal);			// copy image data into location used by texture

	ilDeleteImages(1, &m_uiILTexName);				// image data is now stored in m_pData, so we don't need the IL Image any longer
}

