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

#if !defined(GLEXTENSIONS_H)
#define GLEXTENSIONS_H

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#ifdef WIN32
#include <glext.h>
#else
#include <GL/glext.h>
#endif

#include <string>

using namespace std;


namespace glext 
{

  // Don't try and do these if this is OpenGL1.2 or greater because
  //  these prototypes are already defined.

// #ifndef GL_VERSION_1_2
#ifdef WIN32

	extern PFNGLMULTITEXCOORD1FARBPROC		glMultiTexCoord1fARB;
	extern PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB;
	extern PFNGLMULTITEXCOORD3FARBPROC		glMultiTexCoord3fARB;
	extern PFNGLMULTITEXCOORD4FARBPROC		glMultiTexCoord4fARB;
	extern PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;
	extern PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB;

#endif

	bool EnableMultiTexturing();
}



#endif
