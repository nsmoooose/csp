// heightfield_shader.cpp	-- Thatcher Ulrich <tu@tulrich.com> 2001

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Program to take heightfield input and generate texture for it.
//
// This is very hacky; I just need something quick that makes vaguely
// interesting textures for demoing the chunk renderer.


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

#include "bt_array.h"


//void	initialize_tileset(array<texture_tile>* tileset);
//const texture_tile*	choose_tile(const array<texture_tile>& tileset, int r, int g, int b);
void	heightfield_shader(const char* infile,
			   SDL_RWops* out,
			   SDL_RWops* diffuse_input,
			   SDL_Surface* altitude_gradient,
			   int dimension,
			   float input_vertical_scale
			   );


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
	printf("heightfield_shader: program for generating a .jpg texture for a heightfield\n"
	       "terrain.\n\n"
	       "This program has been donated to the Public Domain by Thatcher Ulrich http://tulrich.com\n"
	       "Incorporates software from the Independent JPEG Group\n\n"
	       "usage: heightfield_shader <input.bt> <output.jpg>\n"
	       "\t[-t <diffuse_texture_filename>]\t-- should match the output size\n"
	       "\t[-g <altitude_gradient_bitmap>]\t-- assigns diffuse color using altitude\n"
	       "\t[-r <dimension of output texture>]\n"
	       "\t[-v <bt_input_vertical_scale>]\n"
	       "\n"
	       "\tThe input file should be a .BT format terrain file with (2^N+1) x (2^N+1) datapoints.\n"
		);
}


int	wrapped_main(int argc, char* argv[])
// Reads the given .BT terrain file or grayscale bitmap, and generates
// a texture map to use when rendering the heightfield.
{
	// Process command-line options.
	char*	infile = NULL;
	char*	outfile = NULL;
	char*	gradient_file = NULL;
	char*	diffuse_texture_filename = NULL;
	int	dimension = 1024;
	float	input_vertical_scale = 1.0f;

	for ( int arg = 1; arg < argc; arg++ ) {
		if ( argv[arg][0] == '-' ) {
			// command-line switch.
			
			switch ( argv[arg][1] ) {
			case 'h':
			case '?':
				print_usage();
				exit( 1 );
				break;

			case 't':
				// Get diffuse texture filename.
				if (arg + 1 >= argc) {
					printf("error: -t option requires a filename\n");
					print_usage();
					exit(1);
				}
				arg++;
				diffuse_texture_filename = argv[arg];
				break;

			case 'g':
				// Get gradient filename.
				if (arg + 1 >= argc) {
					printf("error: -g option requires a filename\n");
					print_usage();
					exit(1);
				}
				arg++;
				gradient_file = argv[arg];
				break;
			case 'r':
				// Get output dimension.
				if (arg + 1 >= argc) {
					printf("error: -r option requires a bitmap dimension (usually a power of 2)\n");
					print_usage();
					exit(1);
				}
				arg++;
				dimension = atoi(argv[arg]);
				if (dimension <= 16 || dimension >= 66636) {
					printf("error: bad output dimension\n");
					print_usage();
					exit(1);
				}
				break;
				
			case 'v':
				// Set the input vertical scale.
				arg++;
				if (arg < argc) {
					input_vertical_scale = (float) atof(argv[arg]);
				}
				else {
					printf("error: -v option must be followed by a value for the input vertical scale\n");
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
	
	SDL_RWops*	out = SDL_RWFromFile(outfile, "wb");
	if (out == 0) {
		printf("error: can't open %s for output.\n", outfile);
		exit(1);
	}

	// Try to open diffuse texture file.
	SDL_RWops*	diffuse_input = NULL;
	if (diffuse_texture_filename)
	{
		diffuse_input = SDL_RWFromFile(diffuse_texture_filename, "rb");
		if (diffuse_input == NULL)
		{
			printf("error: can't open %s for input.\n", diffuse_texture_filename);
			exit(1);
		}
	}

	// Altitude gradient bitmap.
	SDL_Surface*	altitude_gradient = NULL;
	if (gradient_file && gradient_file[0]) {
		altitude_gradient = IMG_Load(gradient_file);
	}

	// Print the parameters.
	printf("infile: %s\n", infile);
	printf("outfile: %s\n", outfile);

	// Process the data.
	heightfield_shader(infile, out, diffuse_input, altitude_gradient,
			   dimension, input_vertical_scale);

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


struct heightfield {
	int	m_size;
	int	m_log_size;
	float	m_sample_spacing;
	float	m_input_vertical_scale;

	bt_array*	m_bt;

	heightfield() {
		m_size = 0;
		m_log_size = 0;
		m_sample_spacing = 1.0f;
		m_input_vertical_scale = 1.0f;
		m_bt = NULL;
	}

	~heightfield() {
		clear();
	}

	void	clear()
	// Frees any allocated data and resets our members.
	{
		if (m_bt) {
			delete m_bt;
			m_bt = NULL;
		}
		m_size = 0;
		m_log_size = 0;
	}


	float	height(int x, int z) const
	// Return the height at (x, z).
	{
		assert(m_bt);
		return m_bt->get_sample(x, z) * m_input_vertical_scale;
	}


	float	height_interp(float x, float z) const
	// Return the height at any point within the grid.  Bilinearly
	// interpolates between samples.
	{
		x = fclamp(x, 0.f, float(m_size - 3));
		z = fclamp(z, 0.f, float(m_size - 3));

		int	i = (int) floor(x);
		int	j = (int) floor(z);

		float	wx = x - i;
		float	wz = z - j;

		// Compute lighting at nearest grid points, and bilinear blend
		// the results.
		float	y00 = height(i, j);
		float	y10 = height(i+1, j);
		float	y01 = height(i, j+1);
		float	y11 = height(i+1, j+1);
		
		return (y00 * (1 - wx) * (1 - wz)
			+ y10 * (wx) * (1 - wz)
			+ y01 * (1 - wx) * (wz)
			+ y11 * (wx) * (wz));
	}


	Uint8	compute_light(float x, float z) const
	// Compute a lighting value at a location in heightfield-space.
	// Result is 0-255.  Input is in heightfield index coordinates,
	// but is interpolated between the nearest samples.  Uses an
	// arbitrary hard-coded sun direction & lighting parameters.
	{
		x = fclamp(x, 0.f, float(m_size - 3));
		z = fclamp(z, 0.f, float(m_size - 3));

		int	i = (int) floor(x);
		int	j = (int) floor(z);

		float	wx = x - i;
		float	wz = z - j;

		// Compute lighting at nearest grid points, and bilinear blend
		// the results.
		Uint8	s00 = compute_light_int(i, j);
		Uint8	s10 = compute_light_int(i+1, j);
		Uint8	s01 = compute_light_int(i, j+1);
		Uint8	s11 = compute_light_int(i+1, j+1);
		
		return (Uint8) (s00 * (1 - wx) * (1 - wz)
				+ s10 * (wx) * (1 - wz)
				+ s01 * (1 - wx) * (wz)
				+ s11 * (wx) * (wz));
	}


	Uint8	compute_light_int(int i, int j) const
	// Aux function for compute_light().  Computes lighting at a
	// discrete heightfield spot.
	{
		float	y00 = height(i, j);
		float	y10 = height(i+1, j);
		float	y01 = height(i, j+1);
		float	y11 = height(i+1, j+1);
				
		float	slopex = (((y00 - y10) + (y01 - y11)) * 0.5f) / m_sample_spacing;
		float	slopez = (((y00 - y01) + (y10 - y11)) * 0.5f) / m_sample_spacing;

		return iclamp((int) ((slopex * 0.25 + slopez * 0.15 + 0.5) * 255), 0, 255);	// xxx arbitrary params
	}


	bool	init_bt(const char* bt_filename)
	// Use the specified .BT format heightfield data file as our height input.
	//
	// Return true on success, false on failure.
	{
		clear();

		m_bt = bt_array::create(bt_filename);
		if (m_bt == NULL) {
			// failure.
			return false;
		}

		m_size = imax(m_bt->get_width(), m_bt->get_height());

		// Compute the log_size and make sure the size is 2^N + 1
		m_log_size = (int) (log2((float) m_size - 1) + 0.5);
		if (m_size != (1 << m_log_size) + 1) {
			if (m_size < 0 || m_size > (1 << 20)) {
				printf("invalid heightfield dimensions -- size from file = %d\n", m_size);
				return false;
			}

			printf("Warning: data is not (2^N + 1) x (2^N + 1); will extend to make it the correct size.\n");

			// Expand log_size until it contains size.
			while (m_size > (1 << m_log_size) + 1) {
				m_log_size++;
			}
			m_size = (1 << m_log_size) + 1;
		}

		m_sample_spacing = (float) (fabs(m_bt->get_right() - m_bt->get_left()) / (double) (m_size - 1));
		printf("sample_spacing = %f\n", m_sample_spacing);//xxxxxxx

		return true;
	}
};


void	compute_lightmap(SDL_Surface* out, const heightfield& hf);


void	heightfield_shader(const char* infile,
			   SDL_RWops* out,
			   SDL_RWops* diffuse_input,
			   SDL_Surface* altitude_gradient,
			   int dimension,
			   float input_vertical_scale
			   )
// Generate texture for heightfield.
{
	heightfield	hf;

	// Load the heightfield data.
	if (hf.init_bt(infile) == false) {
		printf("Can't load %s as a .BT file\n", infile);
		exit(1);
	}
	hf.m_input_vertical_scale = input_vertical_scale;

	// Open the diffuse texture, if any.
	jpeg::input*	diffuse_jpeg = NULL;
	if (diffuse_input)
	{
		diffuse_jpeg = jpeg::input::create(diffuse_input);
		if (diffuse_jpeg == NULL)
		{
			printf("Diffuse texture can't be read as jpeg.\n");
			exit(1);
		}
		if (diffuse_jpeg->get_height() != dimension
		    || diffuse_jpeg->get_width() != dimension)
		{
			printf("Diffuse texture (size %d x %d) must match output dimension (%d)\n",
			       diffuse_jpeg->get_width(), diffuse_jpeg->get_height(), dimension);
			exit(1);
		}
	}

	// Compute and write the lightmap, if any.
	printf("Shading...      ");

	const char*	spinner = "-\\|/";

	int	width = dimension;
	int	height = dimension;
	float	resolution = float(dimension) / (hf.m_size);
	
	// create a buffer to hold a 1-pixel high RGB buffer.  We're
	// going to create our texture one scanline at a time.
	Uint8*	texture_pixels = new Uint8[width * 3];
	Uint8*	diffuse_pixels = new Uint8[width * 3];

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

		Uint8*	p = texture_pixels;

		if (diffuse_jpeg)
		{
			diffuse_jpeg->read_scanline(diffuse_pixels);
		}

		{for (int i = 0; i < width; i++) {

			// Pick a color for this texel.
			// TODO: break this out into a more sophisticated function.
			Uint8	r, g, b, a;
			if (diffuse_jpeg) {
				r = diffuse_pixels[i * 3 + 0];
				g = diffuse_pixels[i * 3 + 1];
				b = diffuse_pixels[i * 3 + 2];
				a = 255;
			}
			else if (altitude_gradient) {
				// Shade the terrain using an altitude gradient.
				float	y = hf.height_interp(i / resolution, j / resolution);
				int	h = altitude_gradient->h;
				if (y < 0.1f)
				{
					// Sea level.
					ReadPixel(altitude_gradient, 0, h - 1, &r, &g, &b, &a);
				}
				else {
					// Above sea level.
					int	hpix = (int) floor(((y - 0.1f) / 4500.f) * h);
					if (hpix >= h) {
						hpix = h - 1;
					}
					if (hpix < 1) {
						hpix = 1;
					}
					ReadPixel(altitude_gradient, 0, (h - 1) - hpix, &r, &g, &b, &a);	// TODO: smooth interpolation, or jittered sampling, or something.
				}
			}
			else {
				r = g = b = a = 255;
			}
#if 0
			else {
				// Select a tile.
				const texture_tile*	t;
				if (tilemap) {
				// Get the corresponding pixel from the tilemap, and
				// get the tile that it corresponds to.
					Uint8	r, g, b, a;
					ReadPixel(tilemap, imin((int) (i / resolution), tilemap->w-1), imin((int) (j / resolution), tilemap->h-1), &r, &g, &b, &a);
					t = choose_tile(tileset, r, g, b);
				} else {
				// In the absence of a tilemap, use the first tile for
				// everything.
					t = &tileset[0];
				}
				assert(t);

				// get tile pixel
				int	tx, ty;
				tx = (i % t->image->w);
				ty = (j % t->image->h);
				Uint8	r, g, b, a;
				ReadPixel(t->image, tx, ty, &r, &g, &b, &a);
			}
#endif // 0

			// apply light to the color
			Uint8	light = hf.compute_light(i / resolution, j / resolution);

			r = (r * light) >> 8;
			g = (g * light) >> 8;
			b = (b * light) >> 8;

			*p++ = r;
			*p++ = g;
			*p++ = b;
		}}

		// Write out the scanline.
		JSAMPROW	row_pointer[1];
		row_pointer[0] = texture_pixels;
		jpeg_write_scanlines(&cinfo, row_pointer, 1);

		int	percent_done = int(100.0f * float(j) / (height - 1));

		printf("\b\b\b\b\b\b%3d%% %c", percent_done, spinner[j&3]);
	}}
	
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

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
