// shader2
// copyright 2003 Mark Rose <mkrose@users.sourceforge.net>


// Program to merge multiple ppm strips into a large jpeg image


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
extern "C" {
#include <jpeglib.h>
}

#include <engine/utility.h>
#include <engine/container.h>
#include <engine/geometry.h>
#include <engine/jpeg.h>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif 

//void	initialize_tileset(array<texture_tile>* tileset);
//const texture_tile*	choose_tile(const array<texture_tile>& tileset, int r, int g, int b);
void	merge(const char* infile, const char *outfile, int parts);


void	error(const char* fmt)
// Generic bail function.
//
// TODO: This should go in the engine library, with an
// API to register a callback instead of exit()ing.
{
	printf(fmt);
	exit(1);
}


void	print_usage()
// Print usage info for this program.
{
	// no args, or -h or -?.  print usage.
	printf("merge-ppm: program for generating a .jpg texture from multiple ppm strips\n"
	       "terrain.\n\n"
	       "Incorporates software from the Independent JPEG Group\n\n"
	       "usage: merge-ppm <input-base> <output.jpg>\n"
	       "\t[-n <strips>]\t-- the number of input strips\n"
		);
}


int	wrapped_main(int argc, char* argv[])
// Reads the given .BT terrain file or grayscale bitmap, and generates
// a texture map to use when rendering the heightfield.
{
	// Process command-line options.
	char*	infile = NULL;
	char*	outfile = NULL;
	int	strips = 1;

	for ( int arg = 1; arg < argc; arg++ ) {
		if ( argv[arg][0] == '-' ) {
			// command-line switch.
			
			switch ( argv[arg][1] ) {
			case 'h':
			case '?':
				print_usage();
				exit( 1 );
				break;


			case 'n':
				if (arg + 1 >= argc) {
					printf("error: -s option requires a strip count\n");
					print_usage();
					exit(1);
				}
				arg++;
				strips = atoi(argv[arg]);
				if (strips < 1) {
					printf("error: -s value must be >= 1\n");
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

	// Print the parameters.
	printf("infile: %s\n", infile);
	printf("outfile: %s\n", outfile);

	// Process the data.
	merge(infile, outfile, strips);

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


void	ReadPixel(SDL_Surface *s, int x, int y, Uint8* R, Uint8* G, Uint8* B, Uint8* A)
// Utility function to read a pixel from an SDL surface.
// TODO: Should go in the engine utilities somewhere.
{
	assert(x >= 0 && x < s->w);
	assert(y >= 0 && y < s->h);

	// Get the raw color data for this pixel out of the surface.
	// Location and size depends on surface format.
	Uint32	color = 0;

	switch (s->format->BytesPerPixel) {
	case 1: { /* Assuming 8-bpp */
		Uint8 *bufp;
		bufp = (Uint8*) s->pixels + y * s->pitch + x;
		color = *bufp;
	}
	break;
	case 2: { /* Probably 15-bpp or 16-bpp */
		Uint16 *bufp;
		bufp = (Uint16 *)s->pixels + y*s->pitch/2 + x;
		color = *bufp;
	}
	break;
	case 3: { /* Slow 24-bpp mode, usually not used */
		Uint8 *bufp;
		bufp = (Uint8 *)s->pixels + y*s->pitch + x * 3;
		if (SDL_BYTEORDER == SDL_LIL_ENDIAN) {
			color = bufp[0];
			color |= bufp[1] <<  8;
			color |= bufp[2] << 16;
		} else {
			color = bufp[2];
			color |= bufp[1] <<  8;
			color |= bufp[0] << 16;
		}
	}
	break;
	case 4: { /* Probably 32-bpp */
		Uint32 *bufp;
		bufp = (Uint32 *)s->pixels + y*s->pitch/4 + x;
		color = *bufp;
	}
	break;
	}

	// Extract the components.
	SDL_GetRGBA(color, s->format, R, G, B, A);
}



void	merge(const char* infile,
              const char* outfile,
	      int strips)
{
	SDL_RWops* out = 0;


	printf("Merging...      ");

	const char*	spinner = "-\\|/";
	Uint8*	texture_pixels = 0;
	Uint8*	diffuse_pixels = 0;
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int width = 0;
	int height = 0;
	int row = 0;

	for (int strip=0; strip < strips; strip++) {

		// strip
		SDL_Surface *strip_map = NULL;
		char fn[1024];
		snprintf(fn, 1023, "%s-%d.ppm", infile, strip); 
		strip_map = IMG_Load(fn);

		if (strip == 0) {
			width = strip_map->w;
			height = strip_map->h * strips;
	
			// create a buffer to hold a 1-pixel high RGB buffer.  We're
			// going to create our texture one scanline at a time.
			texture_pixels = new Uint8[width * 3];
			diffuse_pixels = new Uint8[width * 3];

			// Create our JPEG compression object, and initialize compression settings.
			out = SDL_RWFromFile(outfile, "wb");
			if (out == 0) {
				printf("error: can't open %s for output.\n", outfile);
				exit(1);
			}
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
		}


		{for (int j = 0; j < strip_map->h; j++) {

			Uint8*	p = texture_pixels;

			{for (int i = 0; i < width; i++) {

				Uint8	r, g, b, a;
				ReadPixel(strip_map, i, j, &r, &g, &b, &a);
				*p++ = r;
				*p++ = g;
				*p++ = b;
			}}

			// Write out the scanline.
			JSAMPROW	row_pointer[1];
			row_pointer[0] = texture_pixels;
			jpeg_write_scanlines(&cinfo, row_pointer, 1);

			int	percent_done = int(100.0f * float(row++) / (height - 1));

			printf("\b\b\b\b\b\b%3d%% %c", percent_done, spinner[j&3]);
		}}

		SDL_FreeSurface(strip_map);
	}
	
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	SDL_RWclose(out);

	delete diffuse_pixels;
	delete texture_pixels;
	
	printf("done\n");
}


// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
