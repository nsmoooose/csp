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

#include <SpatialIndex.h>

using namespace SpatialIndex;

Point::Point() : m_dimension(0), m_pCoords(0)
{
}

Point::Point(const double* pCoords, unsigned long dimension) : m_dimension(dimension), m_pCoords(0)
{
	m_pCoords = new double[m_dimension];
	memcpy(m_pCoords, pCoords, m_dimension * sizeof(double));
}

Point::Point(const Point& p) : m_dimension(p.m_dimension), m_pCoords(0)
{
	m_pCoords = new double[m_dimension];
	memcpy(m_pCoords, p.m_pCoords, m_dimension * sizeof(double));
}

Point::~Point()
{
	delete[] m_pCoords;
}

Point& Point::operator=(const Point& p)
{
	if (this != &p)
	{
		if (m_dimension != p.m_dimension)
		{
			delete[] m_pCoords;
			m_dimension = p.m_dimension;
			m_pCoords = new double[m_dimension];
		}

		memcpy(m_pCoords, p.m_pCoords, m_dimension * sizeof(double));
	}

	return *this;
}

bool Point::operator==(const Point& p) const
{
	if (m_dimension != p.m_dimension) throw IllegalArgumentException("operator ==: Points have different number of dimensions.");

	for (unsigned long i = 0; i < m_dimension; i++)
	{
		if (
			m_pCoords[i] < p.m_pCoords[i] - std::numeric_limits<double>::epsilon() ||
			m_pCoords[i] > p.m_pCoords[i] + std::numeric_limits<double>::epsilon())  return false;
	}

	return true;
}

//
// IShape interface
//
bool Point::intersectsShape(const SpatialIndex::IShape& s) const
{
	const Region* pr = dynamic_cast<const Region*>(&s);
	if (pr != 0)
	{
		return pr->containsPoint(*this);
	}

	throw IllegalStateException("intersectsShape: Not implemented yet!");
}

bool Point::containsShape(const SpatialIndex::IShape& s) const
{
	return false;
}

bool Point::touchesShape(const SpatialIndex::IShape& s) const
{
	const Point* ppt = dynamic_cast<const Point*>(&s);
	if (ppt != 0)
	{
		if (*this == *ppt) return true;
		return false;
	}

	const Region* pr = dynamic_cast<const Region*>(&s);
	if (pr != 0)
	{
		return pr->touchesPoint(*this);
	}

	throw IllegalStateException("touches: Not implemented yet!");
}

void Point::getCenter(Point& out) const
{
	out = *this;
}

unsigned long Point::getDimension() const
{
	return m_dimension;
}

void Point::getMBR(Region& out) const
{
	out = Region(m_pCoords, m_pCoords, m_dimension);
}

double Point::getArea() const
{
	return 0.0;
}

double Point::getMinimumDistance(const SpatialIndex::IShape& s) const
{
	const Point* ppt = dynamic_cast<const Point*>(&s);
	if (ppt != 0)
	{
		return getMinimumDistance(*ppt);
	}

	const Region* pr = dynamic_cast<const Region*>(&s);
	if (pr != 0)
	{
		return pr->getMinimumDistance(*this);
	}

	throw IllegalStateException("getMinimumDistance: Not implemented yet!");
}

double Point::getMinimumDistance(const Point& p) const
{
	if (m_dimension != p.m_dimension) throw IllegalArgumentException("getMinimumDistance: Shapes have different number of dimensions.");

	double ret = 0.0;

	for (unsigned long i = 0; i < m_dimension; i++)
	{
		ret += std::pow(m_pCoords[i] - p.m_pCoords[i], 2.0);
	}

	return std::sqrt(ret);
}

double Point::getCoordinate(unsigned long index) const throw (IndexOutOfBoundsException)
{
	if (index < 0 || index >= m_dimension) throw IndexOutOfBoundsException(index);

	return m_pCoords[index];
}

std::ostream& SpatialIndex::operator<<(std::ostream& os, const Point& pt)
{
	unsigned long i;

	for (i = 0; i < pt.m_dimension; i++)
	{
		os << pt.m_pCoords[i] << " ";
	}

	return os;
}
