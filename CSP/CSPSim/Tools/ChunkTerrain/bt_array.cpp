// bt_array.cpp	-- Thatcher Ulrich <tu@tulrich.com> 2002

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Wrapper for accessing a .bt format disk file.  Uses memory-mapped
// file access to a (potentially giant) data file directly without
// loading it all into RAM.


#include "bt_array.h"
#include "mmap_util.h"
#include <engine/utility.h>
#include <string.h>


const int	BT_HEADER_SIZE = 256;	// offset from the start of the .bt file, before the data starts.


const int TOTAL_CACHE_BYTES = 16 << 20; // a number of bytes that we're pretty sure will fit in physical RAM.


bt_array::bt_array()
// Constructor.  Invalidate everything.  Client use bt_array::create()
// to make an instance.
	: m_cache_height(0),
	  m_sizeof_element(2),
	  m_float_data(false),
	  m_width(0),
	  m_height(0),
	  m_utm_flag(false),
	  m_utm_zone(0),
	  m_datum(0),
	  m_left(0),
	  m_right(0),
	  m_bottom(0),
	  m_top(0),
	  m_data(0),
	  m_data_size(0)
{
}


bt_array::~bt_array()
// Destructor.  Free buffers, close files, etc.
{
	m_cache.resize(0);
	if (m_data) {
		assert(m_data_size);
		mmap_util::unmap(m_data, m_data_size);

		m_data = 0;
		m_data_size = 0;
	}
}


/* static */ bt_array*	bt_array::create(const char* filename)
// Makes a bt_array accessor for the specified .bt data file.  Returns
// NULL if the file is not a valid .bt file, or if there are other
// errors.
{
	assert(filename);

	SDL_RWops*	in = SDL_RWFromFile(filename, "rb");
	if (in == 0) {
		// Can't open the file.
		// warning ...
		printf("can't open file %s\n", filename);
		return NULL;
	}
	
	//
	// Read .BT header.
	//
	
	// File-type marker.
	char	buf[11];
	SDL_RWread(in, buf, 1, 10);
	buf[10] = 0;
	if (strcmp(buf, "binterr1.1") != 0) {
		// Bad input file format.  Must not be BT 1.1.
		printf("input file %s is not .BT version 1.1 format\n", filename);
		SDL_RWclose(in);
		return NULL;
	}

	// Create a bt_array instance, and load its members.
	bt_array*	bt = new bt_array();

	bt->m_width = SDL_ReadLE32(in);
	bt->m_height = SDL_ReadLE32(in);
	if (bt->m_width <= 0 || bt->m_height <= 0) {
		// invalid data size.
		printf("invalid data size: width = %d, height = %d\n", bt->m_width, bt->m_height);

		delete bt;
		SDL_RWclose(in);
		return NULL;
	}

	int	sample_size = SDL_ReadLE16(in);
	bt->m_float_data = SDL_ReadLE16(in) == 1 ? true : false;
	if (bt->m_float_data && sample_size != 4) {
		// can't deal with floats that aren't 4 bytes.
		printf("invalid data format: float, but size = %d\n", sample_size);
		delete bt;
		SDL_RWclose(in);
		return NULL;
	}
	if (bt->m_float_data == false && sample_size != 2) {
		// can't deal with ints that aren't 2 bytes.
		printf("invalid data format: int, but size = %d\n", sample_size);
		delete bt;
		SDL_RWclose(in);
		return NULL;
	}
	
	bt->m_sizeof_element = bt->m_float_data ? 4 : 2;
	bt->m_utm_flag = SDL_ReadLE16(in) ? true : false;
	bt->m_utm_zone = SDL_ReadLE16(in);
	bt->m_datum = SDL_ReadLE16(in);
	bt->m_left = ReadDouble64(in);
	bt->m_right = ReadDouble64(in);
	bt->m_bottom = ReadDouble64(in);
	bt->m_top = ReadDouble64(in);

	// Close the file.
	SDL_RWclose(in);

	// Reopen the data using memory-mapping.
	bt->m_data_size = sample_size * bt->m_width * bt->m_height;
	bt->m_data_size += BT_HEADER_SIZE;
	bt->m_data = mmap_util::map(bt->m_data_size, false, filename);
	if (bt->m_data == 0) {
		// Failed to open a memory-mapped view to the data.
		printf("mmap_util::map() failed on %s, size = %d\n", filename, bt->m_data_size);
		delete bt;
		return NULL;
	}

	// Initialize the (empty) cache.
	if (bt->m_width * 4096 < TOTAL_CACHE_BYTES) {
		// No point in caching -- the dataset isn't big enough
		// to stress mmap.
		bt->m_cache_height = 0;
	} else {
		// Figure out how big our cache lines should be.
		bt->m_cache_height = TOTAL_CACHE_BYTES / bt->m_width / bt->m_sizeof_element;
	}
	if (bt->m_cache_height) {
		bt->m_cache.resize(bt->m_width);
	}

	return bt;
}


float	bt_array::get_sample(int x, int z) const
// Return the altitude from this .bt dataset, at the specified
// coordinates.  x runs west-to-east, and z runs north-to-south,
// unlike UTM.  @@ Fix this to make it match UTM???
//
// Out-of-bounds coordinates are clamped.
{
	// clamp coordinates.
	x = iclamp(x, 0, m_width - 1);
	z = iclamp(z, 0, m_height - 1);

	int	index = 0;
	char*	data = NULL;
	if (m_cache_height) {
		// Cache is active.
		
		// 'v' coordinate is (m_height - 1 - z) -- the issue
		// is that BT scans south-to-north, which our z
		// coordinates run north-to-south.  It's easier to
		// compute cache info using the natural .bt data
		// order.
		int	v = (m_height - 1 - z);

		cache_line*	cl = &m_cache[x];
		if (cl->m_data == NULL || v < cl->m_v0 || v >= cl->m_v0 + m_cache_height) {
			// Cache line must be refreshed.
			cl->m_v0 = (v / m_cache_height) * m_cache_height;
			if (cl->m_data == NULL) {
				cl->m_data = new Uint8[m_cache_height * m_sizeof_element];
			}
			int	fillsize = imin(m_cache_height, m_height - cl->m_v0) * m_sizeof_element;
			memcpy(cl->m_data,
			       ((Uint8*)m_data) + BT_HEADER_SIZE + (cl->m_v0 + m_height * x) * m_sizeof_element,
			       fillsize);
		}

		index = v - cl->m_v0;
		data = (char*) cl->m_data;
	} else {
		// Cache is inactive.  Index straight into the raw data.
		data = (char*) m_data;
		data += BT_HEADER_SIZE;
		index = (m_height - 1 - z) + m_height * x;
	}

	if (m_float_data) {
		// raw data is floats.
		data += index * 4;
		union {
			float	f;
			Uint32	u;
		} raw;
		raw.u = SDL_SwapLE32(*(Uint32*) data);

		return raw.f;

	} else {
		// Raw data is 16-bit integer.
		data += index * 2;
		Uint16	y = SDL_SwapLE16(*(Uint16*) data);

		return y;
	}
}



// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
