///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   declaration of CTexture
//   class loads and represents a texture (loads TGA-files)
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//	7/16/2001	implemented basic functionality and adapted structure for
//              new resource-manager - zzed
//  8/5/2001	moved OpenGL-handling to CGLTexture - zzed
//	8/7/2001	added DevIL (former OpenIL) support - Stormbringer
//
//
///////////////////////////////////////////////////////////////////////////


#if !defined(TEXTURE_H)
#define TEXTURE_H

#include "BaseResource.h"

#include <stdio.h>

#include <il/il.h>
#include <il/ilu.h>
#include <il/ilut.h>

class CTexture : public CBaseResource
{
public:
	// our errors ...
	class LoadingFailed {};

	uchar	*m_pData;		// TODO: make it protected again! (testing)
public:
	CTexture();
	CTexture(char* cFilename);
	CTexture(ushort usWidth, ushort usHeight, uchar usPixeldepth, uchar *pData);

	virtual bool Create();
	virtual void Destroy();
	virtual bool Recreate();
	virtual void Dispose();
	virtual size_t GetSize();
	virtual bool IsDisposed();
	

protected:
	bool	m_bDisposable;
	char	*m_cFilename;
	ushort	m_usWidth;
	ushort	m_usHeight;
	
	uchar	m_ucPixeldepth;			// either 24 bit (RGB) or 32 bit (RGBA)
	uint	m_uiILTexName;			// DevIL's texture name format
	
	void LoadImage(char *cFilename);
	void Init();
};

#endif
