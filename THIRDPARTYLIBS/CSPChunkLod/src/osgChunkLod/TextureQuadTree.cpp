/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: nil; -*- */

/*
 * TextureQuadTree.cpp
 *
 * Author(s): Vladimir Vukicevic <vladimir@pobox.com>
 * 
 * The code follows closely after Thatcher Ulrich's sample ChunkLOD implementation.
 * See http://tulrich.com/geekstuff/chunklod.html for more information.
 * Original algorithm by Thatcher Ulrich, released into the public domain.
 *
 * The osgChunkLod library is open source and may be redistributed and/or modified
 * under the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
 * 
 */


#include <osg/Image>

#include <osgChunkLod/TextureQuadTree>

extern "C" {
#include <jpeglib.h>
}

#include <assert.h>
#include <iostream>

namespace osgChunkLod {

int tqt_allocated = 0;

///////////////////////////////////
///// jpeg decompression junk
///////////////////////////////////


class jpegMemoryDecompress {
public:
	jpegMemoryDecompress (unsigned char *in, unsigned long sz) {
		indata = in;
		insize = sz;

		dinfo.client_data = this;
		outdata = NULL;
		jpeg_create_decompress (&dinfo);

		srcmgr = new struct jpeg_source_mgr;
		srcmgr->next_input_byte = NULL;
		srcmgr->bytes_in_buffer = 0;

		srcmgr->init_source = jpegMemoryDecompress::init_source;
		srcmgr->fill_input_buffer = jpegMemoryDecompress::fill_input_buffer;
		srcmgr->skip_input_data = jpegMemoryDecompress::skip_input_data;
		srcmgr->resync_to_restart = jpeg_resync_to_restart;
		srcmgr->term_source = jpegMemoryDecompress::term_source;
		dinfo.src = srcmgr;

		errmgr = new struct jpeg_error_mgr;
		dinfo.err = jpeg_std_error (errmgr);

		jpeg_read_header (&dinfo, TRUE);
	}

	~jpegMemoryDecompress () {
		jpeg_destroy_decompress (&dinfo);
		tqt_allocated -= width*height*depth;
#ifdef DUMP_ALLOC
		std::cerr << "FREE TEXTURE " << tqt_allocated << "\n";
#endif
		delete [] outdata;
		delete srcmgr;
		delete errmgr;
	}

	void decompress () {
		if (outdata != NULL) return;

		jpeg_start_decompress (&dinfo);
		width = dinfo.output_width;
		height = dinfo.output_height;
		depth = dinfo.output_components;

		outdata = new unsigned char[width * height * depth];
		tqt_allocated += width*height*depth;
#ifdef DUMP_ALLOC
		std::cerr << "ALLOCATE TEXTURE " << tqt_allocated << "\n";
#endif
		unsigned char *outptr = outdata;
		int linesread = 0;

		// I hate jpeglib
		JSAMPARRAY jsp = new JSAMPROW[10];
		for (int i = 0; i < 10; i++) {
			jsp[i] = new JSAMPLE[width * depth];
		}

		int numlines;
		while (linesread < height) {
			numlines = jpeg_read_scanlines (&dinfo, jsp, 10);
			outptr = outdata + (linesread * width * depth);
			for (int i = 0; i < numlines; i++) {
				memcpy (outptr, jsp[i], width * depth);
				outptr += width * depth;
			}	
			linesread += numlines;
		}
		jpeg_finish_decompress (&dinfo);
		for (int i = 0; i < 10; i++) {
			delete [] jsp[i];
		}
		delete [] jsp;
	}

	struct jpeg_error_mgr *errmgr;
	struct jpeg_source_mgr *srcmgr;
	struct jpeg_decompress_struct dinfo;

	unsigned char *indata;
	unsigned long insize;
	unsigned char *outdata;
	int width, height;
	unsigned char depth;        // assume RGB if 3

	// jpeg decompressor struct functions
	static void init_source (j_decompress_ptr cinfo) {
		jpegMemoryDecompress *jmd = (jpegMemoryDecompress *) (cinfo->client_data);
		cinfo->src->next_input_byte = jmd->indata;
		cinfo->src->bytes_in_buffer = jmd->insize;
	}

	static boolean fill_input_buffer (j_decompress_ptr cinfo) {
		// shouldn't happen...
		return TRUE;
	}

	static void skip_input_data (j_decompress_ptr cinfo, long num_bytes) {
		cinfo->src->next_input_byte += num_bytes;
		cinfo->src->bytes_in_buffer -= num_bytes;
	}

	static void term_source (j_decompress_ptr cinfo) {
		// nothing
	}
};

/////////////////////
/// end jpeg junk
/////////////////////

static const int TQT_VERSION = 1;
static const int TQT_HEADER = 0x00747174; /* 'tqt\0' */

struct tqt_header_info {
	int m_header;
	int m_version;
	int m_tree_depth;
	int m_tile_size;
};

TextureQuadTree::TextureQuadTree (const char* filename)
{
	_source = new MmapFile (filename);

	tqt_header_info *header = (tqt_header_info *) _source->mapbase();
	if (header->m_header != TQT_HEADER) {
		throw "Invalid tqt file";
	}

	if (header->m_version != TQT_VERSION) {
		throw "Unknown tqt version";
	}

	_depth = header->m_tree_depth;
	_tileSize = header->m_tile_size;

	_source->seek (sizeof (tqt_header_info));
	// read TOC; each entry contains an offset to the chunk's
	// JPEG data, relative to the start of the data
	_toc.resize (nodeCount (_depth));
	for (int i = 0; i < nodeCount (_depth); i++) {
		_toc[i] = _source->readUI32();
	}
}

TextureQuadTree::~TextureQuadTree ()
{
	delete _source;
}

osg::Image*
TextureQuadTree::loadImage (int level, int col, int row) const
{
	int index = nodeIndex (level, col, row);

	assert (index < int(_toc.size()));
	unsigned int offset = _toc[index];

	unsigned char *dataptr = ((unsigned char *) _source->mapbase()) + offset;
	unsigned long left = _source->mapsize() - offset;

	jpegMemoryDecompress jmd (dataptr, left);
	jmd.decompress();

	osg::Image* img = new osg::Image();
	unsigned int pf;
	if (jmd.depth == 1) pf = GL_LUMINANCE;
	else if (jmd.depth == 2) pf = GL_LUMINANCE_ALPHA;
	else if (jmd.depth == 3) pf = GL_RGB;
	else if (jmd.depth == 4) pf = GL_RGBA;
	// FIXME else ?

	img->setImage (jmd.width, jmd.height, jmd.depth, jmd.depth, /* internalFormat */
		   pf, GL_UNSIGNED_BYTE,
		   jmd.outdata, osg::Image::USE_NEW_DELETE);
	// take the pointer
	jmd.outdata = NULL;

	return img;
}

// the number of nodes in a fully populated quadtree of the given depth
int
TextureQuadTree::nodeCount (int depth)
{
	return 0x55555555 & ((1 << depth * 2) - 1);
}

// return a node index given the level and position of
// a node
int
TextureQuadTree::nodeIndex (int level, int col, int row)
{
	assert (col >= 0 && col < (1 << level));
	assert (row >= 0 && row < (1 << level));

	return nodeCount(level) + (row << level) + col;
}


}
