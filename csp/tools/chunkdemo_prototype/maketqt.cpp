// maketqt.cpp	-- Thatcher Ulrich <tu@tulrich.com> 2002

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Program to take a large image and turn it into a quadtree of image
// tiles, a "texture quadtree".  Texture quadtree (".tqt") files can
// be fed into the chunkdemo for hi-res textured terrain.


#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
extern "C" {
#include <jpeglib.h>
}

#include "engine/utility.h"
#include "engine/container.h"
#include "engine/geometry.h"
#include "engine/jpeg.h"
#include "engine/image.h"
#include "engine/tqt.h"


static const char*	spinner = "-\\|/";
static int spin_count = 0;


void	print_usage()
// Print usage info.
{
	printf("maketqt: program for making a texture quadtree file from a .jpg input texture.\n\n"
	       "This program has been donated to the Public Domain by Thatcher Ulrich http://tulrich.com\n"
	       "Incorporates software from the Independent JPEG Group\n\n"
		   "usage: maketqt <input_jpeg> <output_tqt> [-d tree_depth] [-t tile_size]\n"
		   "\n"
		   "tree_depth determines the depth of the fully-populated quadtree.  The default is 6.\n"
		   "tile_size should be a power of two.  The default is 256.\n"
		);
}


struct tqt_info {
	SDL_RWops* out;
	array<Uint32>&	toc;
	int	tree_depth;
	int	tile_size;

	tqt_info(SDL_RWops* o, array<Uint32>* tocptr, int d, int ts)
		: out(o),
		  toc(*tocptr),
		  tree_depth(d),
		  tile_size(ts)
	{
	}
};


image::rgb*	generate_tiles(tqt_info* p, int level, int col, int row);


#undef main	// @@ Under Win32, SDL wants to put in its own main(), to process args.  We don't need that.
int	main(int argc, char* argv[])
{
	char*	infile = NULL;
	char*	outfile = NULL;
	int	tree_depth = 6;
	int	tile_size = 256;

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
				// Set the tilesize.
				if (arg + 1 >= argc) {
					printf("error: -t option requires an integer for tile_size\n");
					print_usage();
					exit(1);
				}
				arg++;
				tile_size = atoi(argv[arg]);
				break;
			case 'd':
				// Tree depth.
				if (arg + 1 >= argc) {
					printf("error: -d option requires an integer for tree_depth\n");
					print_usage();
					exit(1);
				}
				arg++;
				tree_depth = atoi(argv[arg]);
				break;

			default:
				printf("error: unknown command-line switch -%c\n", argv[arg][1]);
				exit(1);
				break;
			}

		} else {
			// File argument.
			if ( infile == NULL ) {
				infile = argv[arg];
			} else if ( outfile == NULL ) {
				outfile = argv[arg];
			} else {
				// This looks like extra noise on the command line; complain and exit.
				printf( "argument '%s' looks like extra noise; exiting.\n", argv[arg]);
				print_usage();
				exit( 1 );
			}
		}
	}

	// Must specify input filename.
	if (infile == NULL) {
		printf("error: you must supply an input filename which points to a .jpg image\n");
		print_usage();
		exit(1);
	}

	// Must specify an output filename.
	if (outfile == NULL) {
		printf("error: you must specify an output filename, for the texture quadtree output\n");
		print_usage();
		exit(1);
	}

	// Validate the tile_size.  Must be a power of two.
	int	logged_tile_size = 1 << frnd(log2((float) tile_size));
	if (tile_size <= 0 || logged_tile_size != tile_size) {
		printf("error: tile_size must be a power of two.\n");
		print_usage();
		exit(1);
	}

	// Validate tree depth.  Keep it within reason.
	if (tree_depth <= 0 || tree_depth > 12)
	{
		printf("error: tree_depth out of range.  Keep it between 1 and 12.\n");
		print_usage();
		exit(1);
	}
	
	// Open input file.
	SDL_RWops*	in = SDL_RWFromFile(infile, "rb");
	if (in == NULL) {
		printf("Can't open input file '%s'!\n", infile);
		exit(1);
	}

	// Open output file.
	SDL_RWops*	out = SDL_RWFromFile(outfile, "w+b");
	if (out == NULL) {
		printf("Can't open output file '%s'!\n", outfile);
		SDL_RWclose(in);
		exit(1);
	}

	// Start reading the input.
	jpeg::input*	j_in = jpeg::input::create(in);
	if (j_in == NULL) {
		printf("Failure reading JPEG header of input file '%s'!\n", infile);
		SDL_RWclose(in);
		SDL_RWclose(out);
		exit(1);
	}

	// Size the tiles.
	int	tile_dim = 1 << (tree_depth - 1);

	// Write .tqt header.
	SDL_RWwrite(out, "tqt\0", 1, 4);	// filetype tag
	SDL_WriteLE32(out, 1);			// version number.
	SDL_WriteLE32(out, tree_depth);
	SDL_WriteLE32(out, tile_size);

	// Make a null table of contents, and write it to the file.
	array<Uint32>	toc;
	toc.resize(tqt::node_count(tree_depth));

	int	toc_start = SDL_RWtell(out);
	for (int i = 0; i < toc.size(); i++) {
		toc[i] = 0;
		SDL_WriteLE32(out, toc[i]);
	}

	int	tile_max_source_height = int(j_in->get_height() / float(tile_dim) + 1);

	// Create a horizontal strip, as wide as the image, and tall
	// enough to cover a whole tile.
	image::rgb*	strip = image::create_rgb(j_in->get_width(), tile_max_source_height);

	// Initialize the strip by reading the first set of scanlines.
	int	next_scanline = 0;
	int	strip_top = 0;
	while (next_scanline < tile_max_source_height) {
		j_in->read_scanline(image::scanline(strip, next_scanline));
		next_scanline++;
	}

	image::rgb*	tile = image::create_rgb(tile_size, tile_size);

	printf("making leaf tiles....     ");

	// generate base level tiles.
	for (int row = 0; row < tile_dim; row++) {
		float	y0 = float(row) / tile_dim * j_in->get_height();
		float	y1 = float(row + 1) / tile_dim * j_in->get_height();

		int	lines_to_read = imin(int(y1), j_in->get_height()) - (strip_top + strip->m_height);
		if (lines_to_read > 0)
		{
			// Copy existing lines up...
			int	lines_to_keep = strip->m_height - lines_to_read;
			{for (int i = 0; i < lines_to_keep; i++) {
				memcpy(image::scanline(strip, i), image::scanline(strip, i + lines_to_read /*keep*/), strip->m_width * 3);
			}}

			// Read new lines
			{for (int i = lines_to_keep; i < strip->m_height; i++) {
				j_in->read_scanline(image::scanline(strip, i));
			}}

			strip_top += lines_to_read;
		}

		for (int col = 0; col < tile_dim; col++) {
			float	x0 = float(col) / tile_dim * j_in->get_width();
			float	x1 = float(col + 1) / tile_dim * j_in->get_width();

			// Resample from the input strip to the output tile.
			image::resample(tile, 0, 0, tile_size - 1, tile_size - 1,
					strip, x0, y0 - strip_top, x1, y1 - strip_top);

			// Update the table of contents with an offset
			// to the data we're about to write.
			int	offset = SDL_RWtell(out);
			int	quadtree_index = tqt::node_index(tree_depth - 1, col, row);
			toc[quadtree_index] = offset;

			// Write the jpeg data.
			image::write_jpeg(out, tile, 90);

			int	percent_done = int(100.f * float(col + row * tile_dim) / (tile_dim * tile_dim));
			printf("\b\b\b\b\b\b%3d%% %c", percent_done, spinner[(spin_count++)&3]);
		}
	}


	// Done reading the input file.
	delete j_in;
	SDL_RWclose(in);

	delete strip;
	delete tile;	// done with the working tile surface.

	printf("\n");

	printf("making interior tiles....");

	// Now generate the upper levels of the tree by resampling the
	// lower levels.
	// 
	// The output file is both input and output at this point.
	tqt_info	inf(out, &toc, tree_depth, tile_size);
	image::rgb*	root_tile = generate_tiles(&inf, 0, 0, 0);

	delete root_tile;	// dispose of root tile.

	// Write the TOC back into the head of the file.
	SDL_RWseek(out, toc_start, SEEK_SET);
	{for (int i = 0; i < toc.size(); i++) {
		SDL_WriteLE32(out, toc[i]);
	}}

	SDL_RWclose(out);

	return 0;
}


image::rgb*	generate_tiles(tqt_info* p, int level, int col, int row)
// (Recursively) generate quadtree tiles by resampling child tiles.
// Returns the tile for the specified node.  As tiles are generated,
// write offsets into the table of contents.
{
	int	qindex = tqt::node_index(level, col, row);
	int	offset = p->toc[qindex];
	if (offset) {
		// Tile already built.  Read it from the file.
		SDL_RWseek(p->out, offset, SEEK_SET);
		image::rgb*	tile = image::read_jpeg(p->out);
		return tile;
	}

	if (level >= p->tree_depth - 1) {
		// We're at the bottom of the tree.  Don't recurse.
		assert(0);	// should not happen, if we generated tiles for all leaf nodes.
		throw "bug";
	}

	// Make a workspace to temporarily hold child tile data in one big image.
	image::rgb*	workspace = image::create_rgb(p->tile_size * 2 - 1, p->tile_size * 2 - 1);

	// Resample the four child tiles to make this tile.
	image::rgb*	tile = image::create_rgb(p->tile_size, p->tile_size);

	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 2; i++) {
			int	ccol = col * 2 + i;
			int	crow = row * 2 + j;
			image::rgb*	child_tile = generate_tiles(p, level + 1, ccol, crow);

#if 0
			int	half_tile = p->tile_size >> 1;
			int	ox = i ? half_tile - 1 : 0;
			int	oy = j ? half_tile - 1 : 0;
			float	shave = 0.5f * float(p->tile_size - 1) / float(p->tile_size - 2);
			float	ix = i ? shave : 0;
			float	iy = j ? shave : 0;
			image::resample(tile, ox, oy, ox + half_tile - 1, oy + half_tile - 1,
					child_tile,
					ix, iy,
					ix + float(p->tile_size) - shave, iy + float(p->tile_size) - shave);
#else
			// Copy image data into workspace.
			{
				int	ox = i ? p->tile_size - 1 : 0;
				int	oy = j ? p->tile_size - 1 : 0;
				for (int row = 0; row < p->tile_size; row++, oy++) {
					memcpy(image::scanline(workspace, oy) + ox * 3, image::scanline(child_tile, row), p->tile_size * 3);
				}
			}
#endif // 0

			delete child_tile;
		}
	}

	// Resample from the workspace into the output tile.
	image::resample(tile, 0, 0, p->tile_size - 1, p->tile_size - 1,
			workspace, 0.f, 0.f, float(workspace->m_width - 1), float(workspace->m_height - 1));

	delete workspace;

	// Write out the generated tile.
	{
		SDL_RWseek(p->out, 0, SEEK_END);
		int	offset = SDL_RWtell(p->out);
		
		// Update table of contents.
		p->toc[qindex] = offset;
		
		image::write_jpeg(p->out, tile, 80);
	}

	printf("\b%c", spinner[(spin_count++)&3]);

	return tile;	// return to caller, so it can use it.
}


// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
