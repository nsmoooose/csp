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

#ifndef __spatialindex_timeregion_h
#define __spatialindex_timeregion_h

namespace SpatialIndex
{
	class TimeRegion : public SpatialIndex::Region, public SpatialIndex::IInterval
	{
	public:
		TimeRegion();
		TimeRegion(const double* pLow, const double* pHigh, const SpatialIndex::IInterval& ti, unsigned long dimension);
		TimeRegion(const double* pLow, const double* pHigh, double tStart, double tEnd, unsigned long dimension);
		TimeRegion(const SpatialIndex::Point& low, const SpatialIndex::Point& high, const SpatialIndex::IInterval& ti);
		TimeRegion(const SpatialIndex::Point& low, const SpatialIndex::Point& high, double tStart, double tEnd);
		TimeRegion(const SpatialIndex::Region& in, const SpatialIndex::IInterval& ti);
		TimeRegion(const SpatialIndex::Region& in, double tStart, double tEnd);
		TimeRegion(const TimePoint& low, const TimePoint& high);
		TimeRegion(const TimeRegion& in);
		virtual ~TimeRegion();

		virtual TimeRegion& operator=(const TimeRegion& r);
		virtual bool operator==(const TimeRegion&) const;

		bool intersectsRegionInTime(const TimeRegion& in) const;
		bool containsRegionInTime(const TimeRegion& in) const;
		bool touchesRegionInTime(const TimeRegion& in) const;

		bool containsPointInTime(const TimePoint& in) const;
		bool touchesPointInTime(const TimePoint& in) const;

		void combineRegionInTime(const TimeRegion& in);
		void getCombinedRegionInTime(TimeRegion& out, const TimeRegion& in) const;

		//
		// IInterval interface
		//
		virtual double getLowerBound() const;
		virtual double getUpperBound() const;
		virtual bool intersectsInterval(const IInterval& ti) const;
		virtual bool intersectsInterval(const double start, const double end) const;
		virtual bool containsInterval(const IInterval& ti) const;

		void makeInfinite(unsigned long dimension);

	public:
		double m_startTime;
		double m_endTime;

		friend std::ostream& operator<<(std::ostream& os, const TimeRegion& r);
	}; // TimeRegion

	std::ostream& operator<<(std::ostream& os, const TimeRegion& r);
}

#endif /* __spatialindex_timeregion_h */
