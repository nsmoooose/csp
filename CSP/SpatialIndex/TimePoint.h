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

#ifndef __spatialindex_timepoint_h
#define __spatialindex_timepoint_h

namespace SpatialIndex
{
	class TimePoint : public Point, public IInterval
	{
	public:
		TimePoint();
		TimePoint(const double* pCoords, const IInterval& ti, unsigned long dimension);
		TimePoint(const double* pCoords, double tStart, double tEnd, unsigned long dimension);
		TimePoint(const Point& p, const IInterval& ti);
		TimePoint(const Point& p, double tStart, double tEnd);
		TimePoint(const TimePoint& p);
		virtual ~TimePoint();

		virtual TimePoint& operator=(const TimePoint& p);
		virtual bool operator==(const TimePoint& p) const;

		//
		// IInterval interface
		//
		virtual double getLowerBound() const;
		virtual double getUpperBound() const;
		virtual bool intersectsInterval(const IInterval& ti) const;
		virtual bool intersectsInterval(const double start, const double end) const;
		virtual bool containsInterval(const IInterval& ti) const;

	public:
		double m_startTime;
		double m_endTime;

		friend class TimeRegion;
		friend std::ostream& operator<<(std::ostream& os, const TimePoint& pt);
	}; // TimePoint

	std::ostream& operator<<(std::ostream& os, const TimePoint& pt);
}

#endif /*__spatialindex_timepoint_h*/
