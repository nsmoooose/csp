// jpeg.h	-- Thatcher Ulrich <tu@tulrich.com> 2002

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Wrapper for jpeg file operations.  The actual work is done by the
// IJG jpeg lib.

#ifndef JPEG_H
#define JPEG_H


struct jpeg_decompress_struct;
struct jpeg_compress_struct;
struct SDL_RWops;


namespace jpeg
{
	// jpeglib data source constructors, for using SDL_RWops instead
	// of stdio for jpeg IO.
	void	setup_rw_source(jpeg_decompress_struct* cinfo, SDL_RWops* instream);
	void	setup_rw_dest(jpeg_compress_struct* cinfo, SDL_RWops* outstream);


	// Helper object for reading jpeg image data.  Basically a thin
	// wrapper around jpeg_decompress_struct.
	struct input {
		// Read header and create a jpeg input object.
		static input*	create(SDL_RWops* in);

		virtual ~input();

		virtual int	get_height() const = 0;
		virtual int	get_width() const = 0;
		virtual void	read_scanline(unsigned char* rgb_data) = 0;
	};


	// Helper object for writing jpeg image data.
	struct output {
		// Create an output object.   Quality goes from 1-100.
		static output*	create(SDL_RWops* out, int width, int height, int quality);

		virtual ~output();

		// ...
		virtual void	write_scanline(unsigned char* rgb_data) = 0;
	};
};


#endif // JPEG_H

// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
