///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//  type definitions for OpenGL extensions (as multitexturing)
//  and functions to enable them
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//	7/21/2001	created file  - zzed
//
//
//
///////////////////////////////////////////////////////////////////////////

#include "glextensions.h"


namespace glext
{

// Do not need to use extensions for multitexturing if this is openGL 1.2.
//#ifndef GL_VERSION_1_2
#ifdef WIN32
	PFNGLMULTITEXCOORD1FARBPROC		glMultiTexCoord1fARB	= 0;
	PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB	= 0;
	PFNGLMULTITEXCOORD3FARBPROC		glMultiTexCoord3fARB	= 0;
	PFNGLMULTITEXCOORD4FARBPROC		glMultiTexCoord4fARB	= 0;
	PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		= 0;
	PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB= 0;

	bool glext::EnableMultiTexturing()
	{
		string	sExtensions;
		int		iMaxTexelUnits;
		
		sExtensions = (char*)glGetString(GL_EXTENSIONS);
		if (sExtensions.find("GL_ARB_multitexture")==string.npos) 
			return false;	// haven't found multitexturing!

		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &iMaxTexelUnits);
		if (iMaxTexelUnits<2) 
			return false;	// not enough texture units!

		// implement functions
		glMultiTexCoord1fARB	= (PFNGLMULTITEXCOORD1FARBPROC)		wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord3fARB	= (PFNGLMULTITEXCOORD3FARBPROC)		wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord4fARB	= (PFNGLMULTITEXCOORD4FARBPROC)		wglGetProcAddress("glMultiTexCoord4fARB");
		glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC)	wglGetProcAddress("glClientActiveTextureARB");		

		return true;
	}
#else
  // If we are on a OpenGL1.2 or higher system define a dummy glextEnableMultiTexturing symbol
  bool glext::EnableMultiTexturing()
  {
    return true;
  }


#endif

}

