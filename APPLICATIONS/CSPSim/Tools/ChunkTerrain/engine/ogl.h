// ogl.h	-- by Thatcher Ulrich <tu@tulrich.com>

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Some OpenGL helpers; mainly to generically deal with extensions.


#ifndef OGL_H
#define OGL_H


#ifdef WIN32
//
// WIN32 includes.  We don't want to have to include windows.h because
// it's such a pig, so #define a couple things that are required to
// make the gl.h stuff work.
//

#	ifndef _INC_WINDOWS

#		define WINAPI	__stdcall
#		define APIENTRY WINAPI
#		define CALLBACK __stdcall
#		define DECLSPEC_IMPORT __declspec(dllimport)

#		if !defined(_GDI32_)
#			define WINGDIAPI DECLSPEC_IMPORT
#		else
#			define WINGDIAPI
#		endif

#	endif

#	ifndef _WCHAR_T_DEFINED
		typedef unsigned short wchar_t;
#		define _WCHAR_T_DEFINED
#	endif // _WCHAR_T_DEFINED

#	include <GL/gl.h>
#	include <GL/glu.h>


#else // not WIN32

	//
	// not WIN32
	//

#	include <GL/gl.h>
#	include <GL/glu.h>

#	define APIENTRY


#endif // not LINUX


namespace ogl {
	void	open();
	void	close();

	// For allocating DMA or video memory, for holding vertex arrays.
	void*	allocate_vertex_memory( int size );	// @@ add a flag for selecting AGP vs. video mem?
	void	free_vertex_memory( void* buffer );

	// Fences; for synchronizing with the GPU.
	void	gen_fences(int count, unsigned int* fence_array);
	void	set_fence(unsigned int fence_id);
	void	finish_fence(unsigned int fence_id);

	// Stream operations; for pushing dynamic vertex data.
	void*	stream_get_vertex_memory(int size);
	void	stream_flush_combiners();	// do this after filling your buffer, and before calling glDrawElements()
};


// GL extension constants...
#define GL_VERTEX_ARRAY_RANGE_NV          0x851D
#define GL_VERTEX_ARRAY_RANGE_LENGTH_NV   0x851E
#define GL_VERTEX_ARRAY_RANGE_VALID_NV    0x851F
#define GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV 0x8520
#define GL_VERTEX_ARRAY_RANGE_POINTER_NV  0x8521

#define GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV	0x8533


#endif // OGL_H

// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
