// ogl.cpp	-- by Thatcher Ulrich <tu@tulrich.com>

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Some OpenGL helpers; mainly to generically deal with extensions.


#include <SDL/SDL.h>
#include <engine/ogl.h>
#include <engine/utility.h>
#include <stdlib.h>
#include <string.h>


#ifdef WIN32
#include <windows.h>	// for wglGetProcAddress
#define PROC_NAME_PREFIX "wgl"
#else // !WIN32
#define PROC_NAME_PREFIX "glX"
#endif // !WIN32

#include <GL/gl.h>


#define GL_ALL_COMPLETED_NV               0x84F2


namespace ogl {

	bool	is_open = false;


	// Pointers to extension functions.
	typedef void * (APIENTRY * PFNWGLALLOCATEMEMORYNVPROC) (int size, float readfreq, float writefreq, float priority);
	typedef void (APIENTRY * PFNWGLFREEMEMORYNVPROC) (void *pointer);
	typedef void (APIENTRY * PFNGLVERTEXARRAYRANGENVPROC) (int size, void* buffer);
	typedef void (APIENTRY * PFNGLGENFENCESNVPROC) (GLsizei n, GLuint *fence_array);
	typedef void (APIENTRY * PFNGLSETFENCENVPROC) (GLuint fence_id, GLenum condition);
	typedef void (APIENTRY * PFNGLFINISHFENCENVPROC) (GLuint fence_id);

	PFNWGLALLOCATEMEMORYNVPROC	wglAllocateMemoryNV = 0;
	PFNWGLFREEMEMORYNVPROC	wglFreeMemoryNV = 0;
	PFNGLVERTEXARRAYRANGENVPROC	glVertexArrayRangeNV = 0;
	PFNGLGENFENCESNVPROC glGenFencesNV = 0;
	PFNGLSETFENCENVPROC glSetFenceNV = 0;
	PFNGLFINISHFENCENVPROC glFinishFenceNV = 0;


	// Big, fast vertex-memory buffer.
	const int	VERTEX_BUFFER_SIZE = 4 << 20;
	void*	vertex_memory_buffer = 0;
	int	vertex_memory_top = 0;
	bool	vertex_memory_from_malloc = false;	// tells us whether to wglFreeMemoryNV() or free() the buffer when we're done.


	const int	STREAM_SUB_BUFFER_COUNT = 2;

	class vertex_stream {
	// Class to facilitate streaming verts to the video card.  Takes
	// care of fencing, and buffer bookkeeping.
	public:
		vertex_stream(int buffer_size);
		~vertex_stream();
	
		void*	reserve_memory(int size);
		void	flush_combiners();
	
	private:
		int	m_sub_buffer_size;
		int	m_buffer_top;
		void*	m_buffer;
		int	m_extra_bytes;	// extra bytes after last block; used to pad up to write-combiner alignment
	
		unsigned int	m_fence[4];
	};


	vertex_stream*	s_stream = NULL;


	void	open()
	// Scan for extensions.
	{
		wglAllocateMemoryNV = (PFNWGLALLOCATEMEMORYNVPROC) SDL_GL_GetProcAddress( PROC_NAME_PREFIX "AllocateMemoryNV" );
		wglFreeMemoryNV = (PFNWGLFREEMEMORYNVPROC) SDL_GL_GetProcAddress( PROC_NAME_PREFIX "FreeMemoryNV" );
		glVertexArrayRangeNV = (PFNGLVERTEXARRAYRANGENVPROC) SDL_GL_GetProcAddress( "glVertexArrayRangeNV" );

		glGenFencesNV = (PFNGLGENFENCESNVPROC) SDL_GL_GetProcAddress( "glGenFencesNV" );
		glSetFenceNV = (PFNGLSETFENCENVPROC) SDL_GL_GetProcAddress( "glSetFenceNV" );
		glFinishFenceNV = (PFNGLFINISHFENCENVPROC) SDL_GL_GetProcAddress( "glFinishFenceNV" );
	}


	void	close()
	// Release anything we need to.
	{
		// @@ free that mongo vertex buffer.
	}


	void*	allocate_vertex_memory( int size )
	// Allocate a block of memory for storing vertex data.  Using this
	// allocator will hopefully give you faster glDrawElements(), if
	// you do vertex_array_range on it before rendering.
	{
		// For best results, we must allocate one big ol' chunk of
		// vertex memory on the first call to this function, via
		// wglAllocateMemoryNV, and then allocate sub-chunks out of
		// it.

		if ( vertex_memory_buffer == 0 ) {
			// Need to allocate the big chunk.
			
			// If we have NV's allocator, then use it.
			if ( wglAllocateMemoryNV ) {
				vertex_memory_buffer = wglAllocateMemoryNV( VERTEX_BUFFER_SIZE, 0.f, 0.f, 0.5f );	// @@ this gets us AGP memory.
//				wglAllocateMemoryNV( size, 0.f, 0.f, 1.0f );	// @@ this gets us video memory.
				vertex_memory_from_malloc = false;
				vertex_memory_top = 0;

				if ( vertex_memory_buffer && glVertexArrayRangeNV ) {
					glVertexArrayRangeNV( VERTEX_BUFFER_SIZE, vertex_memory_buffer );
				}

				glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);	// GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV
			}
			// else we'll fall back on malloc() for vb allocations.
		}

		// Carve a chunk out of our big buffer, or out of malloc if
		// the buffer is dry.

		if ( vertex_memory_buffer && vertex_memory_top + size <= VERTEX_BUFFER_SIZE ) {
			// Just allocate from the end of the big buffer and increment the top.
			unsigned char*	buffer = (unsigned char*) vertex_memory_buffer + vertex_memory_top;
			vertex_memory_top += size;

			return (void*) buffer;

		} else {
			// Fall back to malloc.
			printf( "avm: warning, falling back to malloc!\n" );
			return malloc( size );
		}
	}


	void	free_vertex_memory(void* buffer)
	// Frees a buffer previously allocated via allocate_vertex_memory().
	{
		// this function is not ready for prime-time.
		assert( 0 );
	}


	void	gen_fences(int count, unsigned int* fence_array)
	// Wrapper for glGenFencesNV.
	{
		if (glGenFencesNV) {
			glGenFencesNV(count, fence_array);
		}
		else
		{
			// set all fences to 0.
			for (int i = 0; i < count; i++) {
				fence_array[i] = 0;
			}
		}
	}

	
	void	set_fence(unsigned int fence_id)
	// Use this to declare all previous glDrawElements() calls as
	// belonging to the specified fence.  A subsequent
	// finish_fence(id) will block until those drawing calls have
	// completed.
	{
		if (glSetFenceNV)
		{
			glSetFenceNV(fence_id, GL_ALL_COMPLETED_NV);
		}
		// else no-op.
	}


	void	finish_fence(unsigned int fence_id)
	// Block until all gl drawing calls, associated with the specified
	// fence, have completed.
	{
		if (glFinishFenceNV)
		{
			glFinishFenceNV(fence_id);
		}
		// else no-op.
	}


	void*	stream_get_vertex_memory(int size)
	// Return a buffer to contain size bytes of vertex memory.
	// Put your vertex data in it, then call
	// stream_flush_combiners(), then call glDrawElements() to
	// draw the primitives.
	{
		if (s_stream == NULL) {
			s_stream = new vertex_stream(VERTEX_BUFFER_SIZE);
		}

		return s_stream->reserve_memory(size);
	}


	void	stream_flush_combiners()
	// Make sure to flush all data written to the most recently
	// requested vertex buffer (requested by the last call to
	// stream_get_vertex_memory()).  Ensure that the data doesn't
	// get hung up in a processor write-combiner.
	{
		assert(s_stream);
		if (s_stream == NULL) return;

		s_stream->flush_combiners();
	}


	static const int	WRITE_COMBINER_ALIGNMENT = 64;	// line-size (in bytes) of the write-combiners, must be power of 2


	int	wc_align_up(int size)
	// Return given size, rounded up to the next nearest write combiner
	// alignment boundary.
	{
		assert((WRITE_COMBINER_ALIGNMENT & (WRITE_COMBINER_ALIGNMENT - 1)) == 0);	// make sure it's a power of 2

		return (size + WRITE_COMBINER_ALIGNMENT - 1) & ~(WRITE_COMBINER_ALIGNMENT - 1);
	}


	//
	// vertex_stream
	//

	vertex_stream::vertex_stream(int buffer_size)
	// Construct a streaming buffer, with vertex RAM of the specified size.
	{
		assert(buffer_size >= STREAM_SUB_BUFFER_COUNT);
	
		m_sub_buffer_size = buffer_size / STREAM_SUB_BUFFER_COUNT;
		m_buffer = ogl::allocate_vertex_memory(buffer_size);
		m_buffer_top = 0;
		m_extra_bytes = 0;
	
		// set up fences.
		ogl::gen_fences(4, &m_fence[0]);

		// Set (dummy) fences which will be finished as we reach them.
		ogl::set_fence(m_fence[1]);
		ogl::set_fence(m_fence[2]);
		ogl::set_fence(m_fence[3]);
	}

	vertex_stream::~vertex_stream()
	{
//		ogl::free_vertex_memory(m_buffer);
	}

	void*	vertex_stream::reserve_memory(int size)
	// Clients should call this to get a temporary chunk of fast
	// vertex memory.  Fill it with vertex info and call
	// glVertexPointer()/glDrawElements().  The memory won't get
	// stomped until the drawing is finished, provided you use the
	// returned buffer in a glDrawElements call before you call
	// reserve_memory() to get the next chunk.
	{
		assert(size <= m_sub_buffer_size);

		int	aligned_size = wc_align_up(size);
		m_extra_bytes = aligned_size - size;
	
		for (int sub_buffer = 1; sub_buffer <= STREAM_SUB_BUFFER_COUNT; sub_buffer++)
		{
			int	border = m_sub_buffer_size * sub_buffer;

			if (m_buffer_top <= border
			    && m_buffer_top + aligned_size > border)
			{
				// Crossing into the next sub-buffer.

				int	prev_buffer = sub_buffer - 1;
				int	next_buffer = sub_buffer % STREAM_SUB_BUFFER_COUNT;

				// Protect the previous sub-buffer.
				ogl::set_fence(m_fence[prev_buffer]);

				// Don't overwrite the next sub-buffer while it's still active.
				ogl::finish_fence(m_fence[next_buffer]);
	
				// Start the next quarter-buffer.
				m_buffer_top = m_sub_buffer_size * next_buffer;
			}
		}
	
		void*	buf = ((char*) m_buffer) + m_buffer_top;
		m_buffer_top += aligned_size;
	
		return buf;
	}
	
	
	void	vertex_stream::flush_combiners()
	// Make sure the tail of the block returned by the last call
	// to reserve_memory() gets written to the system RAM.  If the
	// block doesn't end on a write-combiner line boundary, the
	// last bit of data may still be waiting to be flushed.
	//
	// That's my theory anyway -- farfetched but I'm not sure how
	// else to explain certain bug reports of spurious triangles
	// being rendered.
	{
		if (m_extra_bytes) {
			// Fill up the rest of the last write-combiner line.
			memset(((char*) m_buffer) + m_buffer_top - m_extra_bytes, 0, m_extra_bytes);
		}
	}
};


// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
