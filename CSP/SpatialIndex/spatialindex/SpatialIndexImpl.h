// Spatial Index Library
//
// Copyright (C) 2002  Navel Ltd.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Contact information:
//  Mailing address:
//    Marios Hadjieleftheriou
//    University of California, Riverside
//    Department of Computer Science
//    Surge Building, Room 310
//    Riverside, CA 92521
//
//  Email:
//    marioh@cs.ucr.edu

#ifndef __spatialindex__impl_h
#define __spatialindex__impl_h

#include "SpatialIndex.h"
#include "SmartPointer.h"
#include "PointerPool.h"

#ifdef PTHREADS
	#include <pthread.h>
#endif

#define EMAIL "marioh@cs.ucr.edu"
#define VERSION "0.62b"
#define DATE "1 October 2003"

typedef SpatialIndex::PoolPointer<SpatialIndex::Region> RegionPtr;
typedef SpatialIndex::PoolPointer<SpatialIndex::Point> PointPtr;
typedef SpatialIndex::PoolPointer<SpatialIndex::TimeRegion> TimeRegionPtr;

#endif /*__spatialindex_impl_h*/
