// decimate_texture.cpp	-- Thatcher Ulrich <tu@tulrich.com> 2002

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Program to take a .jpg image and *crudely* make a much smaller
// version of it.  Handy for getting a rough preview of gigantic
// generated images, before doing further processing.


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern "C" {
#include <jpeglib.h>
}

#include <engine/utility.h>
#include <engine/container.h>
#include <engine/jpeg.h>


void	decimate(SDL_RWops* out, SDL_RWops* in, int factor);


void	print_usage()
// Print usage info for this program.
{
	// no args, or -h or -?.  print usage.
	printf("decimate_texture: crude subsampler for .jpg images\n"
	       "\n"
	       "This program has been donated to the Public Domain by Thatcher Ulrich http://tulrich.com\n"
	       "Incorporates software from the Independent JPEG Group\n\n"
	       "usage: decimate_texture <input.jpg> <output.jpg>\n"
	       "\t[-x <reduction_factor>]\t-- defaults to 16\n"
		);
}


int	wrapped_main(int argc, char* argv[])
{
	// Process command-line options.
	char*	infile = NULL;
	char*	outfile = NULL;
	int	factor = 16;

	for ( int arg = 1; arg < argc; arg++ ) {
		if ( argv[arg][0] == '-' ) {
			// command-line switch.
			
			switch ( argv[arg][1] ) {
			case 'h':
			case '?':
				print_usage();
				exit( 1 );
				break;

			case 'x':
				// Get decimation factor.
				if (arg + 1 >= argc) {
					printf("error: -x option requires a reduction factor\n");
					print_usage();
					exit(1);
				}
				arg++;
				factor = atoi(argv[arg]);
				if (factor <= 0 || factor > 256) {
					printf("error: bad reduction factor\n");
					print_usage();
					exit(1);
				}
				break;
				
			default:
				printf("error: unknown command-line switch -%c\n", argv[arg][1]);
				exit(1);
				break;
			}

		} else {
			// File argument.
			if (infile == NULL) {
				infile = argv[arg];
			} else if (outfile == NULL) {
				outfile = argv[arg];
			} else {
				// This looks like extra noise on the command line; complain and exit.
				printf( "argument '%s' looks like extra noise; exiting.\n", argv[arg]);
				print_usage();
				exit( 1 );
			}
		}
	}

	// Make sure we have input and output filenames.
	if (infile == NULL || outfile == NULL) {
		// No input or output -- can't run.
		printf( "error: you must specify input and output filenames.\n" );
		print_usage();
		exit( 1 );
	}
	
	SDL_RWops*	in = SDL_RWFromFile(infile, "rb");
	if (in == 0) {
		printf("error: can't open %s for input.\n", outfile);
		exit(1);
	}

	SDL_RWops*	out = SDL_RWFromFile(outfile, "wb");
	if (out == 0) {
		printf("error: can't open %s for output.\n", outfile);
		exit(1);
	}

	decimate(out, in, factor);

	SDL_RWclose(in);
	SDL_RWclose(out);

	return 0;
}


#undef main	// @@ some crazy SDL/WIN32 thing that I don't understand.
int	main(int argc, char* argv[])
{
	try {
		return wrapped_main(argc, argv);
	}
	catch (const char* message) {
		printf("exception: %s\n", message);
	}
	catch (...) {
		printf("unknown exception\n");
	}
	return -1;
}


void	decimate(SDL_RWops* out, SDL_RWops* in, int factor)
// Generate texture for heightfield.
{
	// Open the input texture.
	jpeg::input*	input_jpeg = jpeg::input::create(in);
	if (input_jpeg == NULL)
	{
		printf("input texture can't be read as jpeg.\n");
		exit(1);
	}

	int	in_width = input_jpeg->get_width();
	int	in_height = input_jpeg->get_height();

	int	width = imax(1, in_width / factor);
	int	height = imax(1, in_height / factor);

	printf("Processing...      ");

	const char*	spinner = "-\\|/";

	// Create scanline buffers.
	Uint8*	input_pixels = new Uint8[in_width * 3];
	Uint8*	output_pixels = new Uint8[width * 3];

	// Create our JPEG compression object, and initialize compression settings.
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg::setup_rw_dest(&cinfo, out);

	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 80 /* 0..100 */, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	{for (int j = 0; j < height; j++) {

		input_jpeg->read_scanline(input_pixels);

		Uint8*	p = output_pixels;

		{for (int i = 0; i < width; i++) {
			Uint8	r, g, b, a;
			r = input_pixels[i * factor * 3 + 0];
			g = input_pixels[i * factor * 3 + 1];
			b = input_pixels[i * factor * 3 + 2];
			a = 255;

			*p++ = r;
			*p++ = g;
			*p++ = b;
		}}

		// Write out the scanline.
		JSAMPROW	row_pointer[1];
		row_pointer[0] = output_pixels;
		jpeg_write_scanlines(&cinfo, row_pointer, 1);

		// Read extra unused scanlines...
		{for (int i = 0; i < factor - 1; i++) {
			input_jpeg->read_scanline(input_pixels);	// @@ need to use a "skip scanline" API... ijg lib probably has it.
		}}

		int	percent_done = int(100.0f * float(j) / (height - 1));

		printf("\b\b\b\b\b\b%3d%% %c", percent_done, spinner[j&3]);
	}}
	
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	delete input_pixels;
	delete output_pixels;
	
	printf("done\n");
}


// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
