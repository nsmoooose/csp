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

#ifndef __spatialindex_point_h
#define __spatialindex_point_h

namespace SpatialIndex
{
	class Point : public SpatialIndex::IShape
	{
	public:
		Point();
		Point(const double* pCoords, unsigned long dimension);
		Point(const Point& p);
		virtual ~Point();

		virtual Point& operator=(const Point& p);
		virtual bool operator==(const Point& p) const;

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

		double getMinimumDistance(const Point& p) const;

		virtual double getCoordinate(unsigned long index) const throw (IndexOutOfBoundsException);

	public:
		unsigned long m_dimension;
		double* m_pCoords;

		friend class Region;
		friend std::ostream& operator<<(std::ostream& os, const Point& pt);
	}; // Point

	std::ostream& operator<<(std::ostream& os, const Point& pt);
}

#endif /*__spatialindex_point_h*/
