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

#ifndef __spatialindex_region_h
#define __spatialindex_region_h

namespace SpatialIndex
{
	class Region : public SpatialIndex::IShape
	{
	public:
		Region();
		Region(const double* pLow, const double* pHigh, unsigned long dimension);
		Region(const Point& low, const Point& high);
		Region(const Region& in);
		virtual ~Region();

		virtual Region& operator=(const Region& r);
		virtual bool operator==(const Region&) const;

		//
		// IShape interface
		//
		virtual bool intersectsShape(const SpatialIndex::IShape& in) const;
		virtual bool containsShape(const SpatialIndex::IShape& in) const;
		virtual bool touchesShape(const SpatialIndex::IShape& in) const;
		virtual void getCenter(Point& out) const;
		virtual unsigned long getDimension() const;
		virtual void getMBR(Region& out) const;
		virtual double getArea() const;
		virtual double getMinimumDistance(const SpatialIndex::IShape& in) const;

		bool intersectsRegion(const Region& in) const;
		bool containsRegion(const Region& in) const;
		bool touchesRegion(const Region& in) const;
		double getMinimumDistance(const Region& in) const;

		bool containsPoint(const Point& in) const;
		bool touchesPoint(const Point& in) const;
		double getMinimumDistance(const Point& in) const;

		double getIntersectingArea(const Region& in) const;
		double getMargin() const;

		void combineRegion(const Region& in);
		void getCombinedRegion(Region& out, const Region& in) const;

		virtual double getLow(unsigned long index) const throw (IndexOutOfBoundsException);
		virtual double getHigh(unsigned long index) const throw (IndexOutOfBoundsException);

	public:
		unsigned long m_dimension;
		double* m_pLow;
		double* m_pHigh;

		friend std::ostream& operator<<(std::ostream& os, const Region& r);
	}; // Region

	std::ostream& operator<<(std::ostream& os, const Region& r);
}

#endif /*__spatialindex_region_h*/
