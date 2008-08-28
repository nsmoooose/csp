// image_filters.h	-- Thatcher Ulrich <tu@tulrich.com> 2002

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Image rescaling routines.


#ifndef IMAGE_FILTERS_H
#define IMAGE_FILTERS_H


#include <engine/image.h>


namespace image_filters
{
	void	resample(image::rgb* out,
			 image::rgb* in, float in_x0, float in_y0, float in_x1, float in_y1);
};


#endif // IMAGE_FILTERS_H

// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:

