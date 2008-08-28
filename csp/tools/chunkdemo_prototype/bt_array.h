// bt_array.h	-- Thatcher Ulrich <tu@tulrich.com> 2002

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Wrapper for accessing a .bt format disk file.  Uses memory-mapped
// file access to a (potentially giant) data file directly without
// loading it all into RAM.


#ifndef BT_ARRAY_H
#define BT_ARRAY_H


#include <engine/container.h>


class bt_array
// Class for wrapping access to a .bt terrain file.
{
	bt_array();
public:
	static bt_array*	create(const char* filename);

	~bt_array();

	int	get_width() const { return m_width; }
	int	get_height() const { return m_height; }
	bool	get_utm_flag() const { return m_utm_flag; }
	int	get_utm_zone() const { return m_utm_zone; }
	int	get_datum() const { return m_datum; }

	double	get_left() const { return m_left; }
	double	get_right() const { return m_right; }
	double	get_bottom() const { return m_bottom; }
	double	get_top() const { return m_top; }

	// out-of-bounds access is clamped.
	float	get_sample(int x, int z) const;

private:
	// We're going to cache short vertical strips of data, because
	// our processing tools often want to scan horizontally, but
	// the data is stored vertically.  For giant .bt files,
	// scanning horizontally is brutal on the mmap subsystem.
	struct cache_line {
		void*	m_data;
		int	m_v0;

		cache_line() : m_data(NULL), m_v0(0) {}
		~cache_line() {
			if (m_data) delete [] (Uint8*) m_data;
			m_data = 0;
		}
	};
	mutable array<cache_line>	m_cache;
	int	m_cache_height;
	int	m_sizeof_element;

	bool	m_float_data;	// true if the array data is in floating-point format.
	int	m_width;
	int	m_height;
	bool	m_utm_flag;
	int	m_utm_zone;
	int	m_datum;
	double	m_left, m_right, m_bottom, m_top;

	void*	m_data;
	int	m_data_size;

};


#endif // BT_ARRAY_H


// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
