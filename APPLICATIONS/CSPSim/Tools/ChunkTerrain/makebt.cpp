// makebt.cpp	-- Thatcher Ulrich <tu@tulrich.com> 2001

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Program to take a .png image and make a .bt format terrain file out
// of it.


#include <stdlib.h>
#include <png.h>
#include <SDL/SDL.h>
#include "engine/utility.h"


#ifndef WIN32
//FIX: sounds like some WIN32 specifics...
#define png_voidp_NULL NULL
#define png_infopp_NULL NULL
#define int_p_NULL NULL
#endif

bool	check_if_png(char *file_name);


#undef main	// SDL wackiness.
int	main(int argc, char* argv[])
{
	printf("usage: makebt <inputfile.png> <outputfile.bt> [-s horizontal_scale] [-v vertical_scale]\n\t[-t min_threshold]\n");

	char*	input_file = NULL;
	char*	output_file = NULL;

	float	h_scale = 1.0f;
	float	v_scale = 1.0f;
	int	min_threshold = 0;
	float	thresh_scale = 1.0f;

	// parse arguments.
	for (int arg = 1; arg < argc; arg++) {
		if (argv[arg][0] == '-') {
			switch (argv[arg][1]) {
			default:
			case 'h':
				printf("Unknown switch -%c\n", argv[arg][1]);
				exit(1);
				break;
			case 's':	// horizontal scale
				arg++;
				if (arg >= argc) {
					printf("error: must follow -s with a number to scale the horizontal units with\n");
					exit(1);
				}
				h_scale = (float) atof(argv[arg]);
				break;
			case 'v':	// vertical scale
				arg++;
				if (arg >= argc) {
					printf("error: must follow -v with a number to scale the vertical units with\n");
					exit(1);
				}
				v_scale = (float) atof(argv[arg]);
				break;
			case 't':	// min threshold
				arg++;
				if (arg >= argc) {
					printf("error: must follow -t with a number for the minimum height threshold\n"
					       "('sea level') -- everything below that gets mapped to 0 height.\n");
					exit(1);
				}
				min_threshold = atoi(argv[arg]);
				break;
			}
		}
		else {
			if (input_file == NULL) {
				input_file = argv[arg];
			}
			else if (output_file == NULL) {
				output_file = argv[arg];
			}
			else {
				printf("too many args!\n");
				exit(1);
			}
		}
	}

	if (input_file == NULL || output_file == NULL) {
		printf("You must specify input and output file names.\n");
		exit(1);
	}

	printf("in = '%s', out = '%s', h_scale = %g, v_scale = %g, thresh = %d\n", input_file, output_file, h_scale, v_scale, min_threshold);

	FILE*	in;
	SDL_RWops*	out;

	bool	is_png = check_if_png(input_file);
	if (is_png == false) {
		printf("Input file is not a .png file!\n");
		exit(1);
	}
	in = fopen(input_file, "rb");
	if (in == NULL) {
		printf("Can't open input file!\n");
		exit(1);
	}

	out = SDL_RWFromFile(output_file, "wb");
	if (out == NULL) {
		printf("Can't open output file!\n");
		fclose(in);
		exit(1);
	}


	//
	// Read png input file.
	//

	png_structp png_ptr;
	png_infop info_ptr;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		fclose(in);
		return 1;
	}

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(in);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return 1;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		/* Free all of the memory associated with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		fclose(in);
		/* If we get here, we had a problem reading the file */
		return 1;
	}

	/* Set up the input control if you are using standard C streams */
	png_init_io(png_ptr, in);

	// Read the data.
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, png_voidp_NULL);

	/* At this point you have read the entire image */

	// Get image info.
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
				 &interlace_type, int_p_NULL, int_p_NULL);

	// Input file must be grayscale.
	if (color_type != PNG_COLOR_TYPE_GRAY) {
		printf("input file is not grayscale!\n");
		fclose(in);
		SDL_RWclose(out);
		return 1;
	}

	// Color depth must be 8 or 16 bits.
	if (bit_depth != 8 && bit_depth != 16) {
		printf("input file's bit depth is not 8 or 16!\n");
		fclose(in);
		SDL_RWclose(out);
		return 1;
	}

	// Compute threshold scale.
	if (bit_depth == 8) {
		thresh_scale = 255.0f / (255 - imin(min_threshold, 254));
	}
	else {
		thresh_scale = 65535.0f / (65535 - imin(min_threshold, 65534));
	}

	//
	// Write the .bt
	//

	printf("PNG width = %ld, height = %ld\n", width, height);

	SDL_RWwrite(out, "binterr1.1", 1, 10);

	SDL_WriteLE32(out, width);
	SDL_WriteLE32(out, height);
	SDL_WriteLE16(out, 2);	// sample size --> 2 bytes
	SDL_WriteLE16(out, 0);	// float flag --> false (we're writing short int's)
	SDL_WriteLE16(out, 0);	// utm flag --> false
	SDL_WriteLE16(out, 0);	// utm zone
	SDL_WriteLE16(out, 0);	// datum

	WriteDouble64(out, width * h_scale);	// left
	WriteDouble64(out, 0);	// right
	WriteDouble64(out, 0);	// bottom
	WriteDouble64(out, height * h_scale);	// top

	// Pad out with 0's to make a 256-byte header.
	for (int i = 60; i < 256; i++) {
		SDL_RWwrite(out, "\0", 1, 1);
	}

	int	min = 65535;
	int	max = 0;

	// BT data goes in columns, bottom-to-top, left-to-right.
	png_bytep*	row_pointers = png_get_rows(png_ptr, info_ptr);
	{for (unsigned int i = 0; i < width; i++) {
		for (unsigned int j = 0; j < height; j++) {
			Uint16	data = 0;
			if (bit_depth == 8) {
				data = ((Uint8*) (row_pointers[height - 1 - j]))[i];
			}
			else {
				data = SDL_SwapBE16(((Uint16*) (row_pointers[height - 1 - j]))[i]);
			}
			data = iclamp((int) ((data - min_threshold) * thresh_scale * v_scale), 0, 0x0FFFF);

			if (data > max) { max = data; }
			if (data < min) { min = data; }

			SDL_WriteLE16(out, data);
		}
	}}

	SDL_RWclose(out);
	
	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	
	/* close the file */
	fclose(in);

	printf("min height = %d, max height = %d\n", min, max);

	return 0;
}





#define PNG_BYTES_TO_CHECK 4
bool	check_if_png(char *file_name)
{
	FILE*	fp;
	
	char buf[PNG_BYTES_TO_CHECK];

	/* Open the prospective PNG file. */
	if ((fp = fopen(file_name, "rb")) == NULL)
		return 0;

	/* Read in some of the signature bytes */
	if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK)
		return 0;

	/* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
	   Return nonzero (true) if they match */
	
	return !png_sig_cmp((unsigned char*) &buf[0], (png_size_t)0, PNG_BYTES_TO_CHECK);
}

// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
