// jpeg.cpp	-- Thatcher Ulrich <tu@tulrich.com> 2002

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Wrapper for jpeg file operations.  The actual work is done by the
// IJG jpeg lib.


#include "engine/jpeg.h"
#include "engine/utility.h"
#include <stdio.h>
extern "C" {
#include <jpeglib.h>
}
#include <SDL/SDL.h>


namespace jpeg
{
	static const int	IO_BUF_SIZE = 4096;

	// A jpeglib source manager that reads from a SDL_RWops.  Paraphrased
	// from IJG jpeglib jdatasrc.c.
	struct rw_source
	{
		struct jpeg_source_mgr	m_pub;		/* public fields */

		SDL_RWops*	m_in_stream;		/* source stream */
		bool	m_start_of_file;		/* have we gotten any data yet? */
		JOCTET	m_buffer[IO_BUF_SIZE];	/* start of buffer */

		rw_source(SDL_RWops* in)
			: m_in_stream(in),
			  m_start_of_file(true)
		// Constructor.  The caller is responsible for closing the input stream
		// after it's done using us.
		{
			// fill in function pointers...
			m_pub.init_source = init_source;
			m_pub.fill_input_buffer = fill_input_buffer;
			m_pub.skip_input_data = skip_input_data;
			m_pub.resync_to_restart = jpeg_resync_to_restart;	// use default method
			m_pub.term_source = term_source;
			m_pub.bytes_in_buffer = 0;
			m_pub.next_input_byte = NULL;
		}

		static void init_source(j_decompress_ptr cinfo)
		{
			rw_source*	src = (rw_source*) cinfo->src;
			src->m_start_of_file = true;
		}

		static boolean	fill_input_buffer(j_decompress_ptr cinfo)
		// Read data into our input buffer.  Client calls this
		// when it needs more data from the file.
		{
			rw_source*	src = (rw_source*) cinfo->src;

			size_t	bytes_read = SDL_RWread(src->m_in_stream, src->m_buffer, 1, IO_BUF_SIZE);

			if (bytes_read <= 0) {
				// Is the file completely empty?
				if (src->m_start_of_file) {
					// Treat this as a fatal error.
					throw "empty jpeg source stream.";
				}
				// warn("jpeg end-of-stream");

				// Insert a fake EOI marker.
				src->m_buffer[0] = (JOCTET) 0xFF;
				src->m_buffer[1] = (JOCTET) JPEG_EOI;
				bytes_read = 2;
			}

			// Expose buffer state to clients.
			src->m_pub.next_input_byte = src->m_buffer;
			src->m_pub.bytes_in_buffer = bytes_read;
			src->m_start_of_file = false;

			return TRUE;
		}

		static void	skip_input_data(j_decompress_ptr cinfo, long num_bytes)
		// Called by client when it wants to advance past some
		// uninteresting data.
		{
			rw_source*	src = (rw_source*) cinfo->src;

			// According to jpeg docs, large skips are
			// infrequent.  So let's just do it the simple
			// way.
			if (num_bytes > 0) {
				while (num_bytes > (long) src->m_pub.bytes_in_buffer) {
					num_bytes -= (long) src->m_pub.bytes_in_buffer;
					fill_input_buffer(cinfo);
				}
				// Handle remainder.
				src->m_pub.next_input_byte += (size_t) num_bytes;
				src->m_pub.bytes_in_buffer -= (size_t) num_bytes;
			}
		}

		static void term_source(j_decompress_ptr cinfo)
		// Terminate the source.  Make sure we get deleted.
		{
			rw_source*	src = (rw_source*) cinfo->src;
			assert(src);

			// @@ it's kind of bogus to be deleting here
			// -- term_source happens at the end of
			// reading an image, but we're probably going
			// to want to init a source and use it to read
			// many images, without reallocating our
			// buffer.
			delete src;
			cinfo->src = NULL;
		}
	};

	
	void	setup_rw_source(jpeg_decompress_struct* cinfo, SDL_RWops* instream)
	// Set up the given decompress object to read from the given
	// stream.
	{
		// assert(cinfo->src == NULL);
		cinfo->src = (jpeg_source_mgr*) new rw_source(instream);
	}


	// A jpeglib destination manager that writes to a SDL_RWops.
	// Paraphrased from IJG jpeglib jdatadst.c.
	struct rw_dest
	{
		struct jpeg_destination_mgr	m_pub;	/* public fields */

		SDL_RWops*	m_out_stream;		/* source stream */
		JOCTET	m_buffer[IO_BUF_SIZE];	/* start of buffer */

		rw_dest(SDL_RWops* out)
			: m_out_stream(out)
		// Constructor.  The caller is responsible for closing
		// the output stream after it's done using us.
		{
			// fill in function pointers...
			m_pub.init_destination = init_destination;
			m_pub.empty_output_buffer = empty_output_buffer;
			m_pub.term_destination = term_destination;

			m_pub.next_output_byte = m_buffer;
			m_pub.free_in_buffer = IO_BUF_SIZE;
		}

		static void init_destination(j_compress_ptr cinfo)
		{
			rw_dest*	dest = (rw_dest*) cinfo->dest;
			assert(dest);

			dest->m_pub.next_output_byte = dest->m_buffer;
			dest->m_pub.free_in_buffer = IO_BUF_SIZE;
		}

		static boolean	empty_output_buffer(j_compress_ptr cinfo)
		// Write the output buffer into the stream.
		{
			rw_dest*	dest = (rw_dest*) cinfo->dest;
			assert(dest);

			if (SDL_RWwrite(dest->m_out_stream, dest->m_buffer, 1, IO_BUF_SIZE) != IO_BUF_SIZE)
			{
				// Error.
				throw "jpeg::rw_dest couldn't write data.";
			}

			dest->m_pub.next_output_byte = dest->m_buffer;
			dest->m_pub.free_in_buffer = IO_BUF_SIZE;

			return TRUE;
		}

		static void term_destination(j_compress_ptr cinfo)
		// Terminate the destination.  Flush any leftover
		// data, and make sure we get deleted.
		{
			rw_dest*	dest = (rw_dest*) cinfo->dest;
			assert(dest);

			// Write any remaining data.
			int	datacount = IO_BUF_SIZE - dest->m_pub.free_in_buffer;
			if (datacount > 0) {
				if (SDL_RWwrite(dest->m_out_stream, dest->m_buffer, 1, datacount) != datacount) {
					// Error.
					throw "jpeg::rw_dest::term_destination couldn't write data.";
				}
			}

			// Clean ourselves up.
			delete dest;
			cinfo->dest = NULL;
		}
	};


	void	setup_rw_dest(j_compress_ptr cinfo, SDL_RWops* outstream)
	// Set up the given compress object to write to the given
	// output stream.
	{
		cinfo->dest = (jpeg_destination_mgr*) new rw_dest(outstream);
	}


	//
	// wrappers
	//


	struct input_impl : public input
	// Bascially this is a thin wrapper around jpeg_decompress
	// object.
	{
		// State needed for input.
		struct jpeg_decompress_struct	m_cinfo;
		struct jpeg_error_mgr m_jerr;

		input_impl(SDL_RWops* in)
		// Constructor.  Read the header data from in, and
		// prepare to read data.
		{
			m_cinfo.err = jpeg_std_error(&m_jerr);

			// Initialize decompression object.
			jpeg_create_decompress(&m_cinfo);

			setup_rw_source(&m_cinfo, in);

			jpeg_read_header(&m_cinfo, TRUE);
			jpeg_start_decompress(&m_cinfo);
		}

		~input_impl()
		// Destructor.  Clean up our jpeg reader state.
		{
			jpeg_finish_decompress(&m_cinfo);

/*
			rw_source* src = (rw_source*) m_cinfo.src;
			delete src;
			m_cinfo.src = NULL;
*/

			jpeg_destroy_decompress(&m_cinfo);
		}

		int	get_height() const
		// Return the height of the image.  Take the data from our m_cinfo struct.
		{
			return m_cinfo.output_height;
		}

		int	get_width() const
		// Return the width of the image.  Take the data from our m_cinfo struct.
		{
			return m_cinfo.output_width;
		}

		int	get_components() const
		// Return number of components (i.e. == 3 for RGB
		// data).  The size of the data for a scanline is
		// get_width() * get_components().
		{
			return m_cinfo.output_components;
		}


		void	read_scanline(unsigned char* rgb_data)
		// Read a scanline's worth of image data into the
		// given buffer.  The amount of data read is
		// get_width() * get_components().
		{
			assert(m_cinfo.output_scanline < m_cinfo.output_height);
			int	lines_read = jpeg_read_scanlines(&m_cinfo, &rgb_data, 1);
			assert(lines_read == 1);
			lines_read = lines_read;	// avoid warning in NDEBUG
		}
	};


	/*static*/ input*	input::create(SDL_RWops* in)
	// Create and return a jpeg-input object that will read from the
	// given input stream.
	{
		return new input_impl(in);
	}


	// Default constructor.
	input::~input() {}


	struct output_impl : public output
	// Basically this is a thin wrapper around jpeg_compress
	// object.
	{
		// State needed for output.
		struct jpeg_compress_struct	m_cinfo;
		struct jpeg_error_mgr m_jerr;

		output_impl(SDL_RWops* out, int width, int height, int quality)
		// Constructor.  Read the header data from in, and
		// prepare to read data.
		{
			m_cinfo.err = jpeg_std_error(&m_jerr);

			// Initialize decompression object.
			jpeg_create_compress(&m_cinfo);

			setup_rw_dest(&m_cinfo, out);
			m_cinfo.image_width = width;
			m_cinfo.image_height = height;
			m_cinfo.input_components = 3;
			m_cinfo.in_color_space = JCS_RGB;
			jpeg_set_defaults(&m_cinfo);
			jpeg_set_quality(&m_cinfo, quality, TRUE);

			jpeg_start_compress(&m_cinfo, TRUE);
		}


		~output_impl()
		// Destructor.  Clean up our jpeg reader state.
		{
			jpeg_finish_compress(&m_cinfo);
/*
			rw_dest* src = (rw_source*) m_cinfo.dest;
			delete dest;
			m_cinfo.dest = NULL;
*/
			jpeg_destroy_compress(&m_cinfo);
		}


		void	write_scanline(unsigned char* rgb_data)
		// Write out a single scanline.
		{
			jpeg_write_scanlines(&m_cinfo, &rgb_data, 1);
		}
	};


	/*static*/ output*	output::create(SDL_RWops* in, int width, int height, int quality)
	// Create and return a jpeg-input object that will read from the
	// given input stream.
	{
		return new output_impl(in, width, height, quality);
	}


	// Default constructor.
	output::~output() {}
};


// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
