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

Region::Region() : m_dimension(0), m_pLow(0), m_pHigh(0)
{
}

Region::Region(const double* pLow, const double* pHigh, unsigned long dimension) : m_dimension(dimension), m_pLow(0), m_pHigh(0)
{
#ifndef NDEBUG
	for (unsigned long cDim = 0; cDim < m_dimension; cDim++)
	{
		if (pLow[cDim] > pHigh[cDim]) throw IllegalArgumentException("Low point has larger coordinates than High point.");
	}
#endif

	try
	{
		m_pLow = new double[m_dimension];
		m_pHigh = new double[m_dimension];
	}
	catch (...)
	{
		delete[] m_pLow;
		delete[] m_pHigh;
		throw;
	}

	memcpy(m_pLow, pLow, m_dimension * sizeof(double));
	memcpy(m_pHigh, pHigh, m_dimension * sizeof(double));
}

Region::Region(const Point& low, const Point& high) : m_dimension(low.m_dimension), m_pLow(0), m_pHigh(0)
{
	if (low.m_dimension != high.m_dimension) throw IllegalArgumentException("Region: arguments have different number of dimensions.");

#ifndef NDEBUG
	for (unsigned long cDim = 0; cDim < m_dimension; cDim++)
	{
		if (low.m_pCoords[cDim] > high.m_pCoords[cDim]) throw IllegalArgumentException("Low point has larger coordinates than High point.");
	}
#endif

	try
	{
		m_pLow = new double[m_dimension];
		m_pHigh = new double[m_dimension];
	}
	catch (...)
	{
		delete[] m_pLow;
		delete[] m_pHigh;
		throw;
	}

	memcpy(m_pLow, low.m_pCoords, m_dimension * sizeof(double));
	memcpy(m_pHigh, high.m_pCoords, m_dimension * sizeof(double));
}

Region::Region(const Region& r) : IShape(r), m_dimension(r.m_dimension)
{
	try
	{
		m_pLow = new double[m_dimension];
		m_pHigh = new double[m_dimension];
	}
	catch (...)
	{
		delete[] m_pLow;
		delete[] m_pHigh;
		throw;
	}

	memcpy(m_pLow, r.m_pLow, m_dimension * sizeof(double));
	memcpy(m_pHigh, r.m_pHigh, m_dimension * sizeof(double));
}

Region::~Region()
{
	delete[] m_pLow;
	delete[] m_pHigh;
}

Region& Region::operator=(const Region& r)
{
	if(this != &r)
	{
		if (m_dimension != r.m_dimension)
		{
			m_dimension = r.m_dimension;

			delete[] m_pLow;
			delete[] m_pHigh;

			try
			{
				m_pLow = new double[m_dimension];
				m_pHigh = new double[m_dimension];
			}
			catch (...)
			{
				delete[] m_pLow;
				delete[] m_pHigh;
				throw;
			}
		}

		memcpy(m_pLow, r.m_pLow, m_dimension * sizeof(double));
		memcpy(m_pHigh, r.m_pHigh, m_dimension * sizeof(double));
	}

	return *this;
}

bool Region::operator==(const Region& r) const
{
	if (m_dimension != r.m_dimension) throw IllegalArgumentException("operator ==: Regions have different number of dimensions.");

	for (unsigned long i = 0; i < m_dimension; i++)
	{
		if (
			m_pLow[i] < r.m_pLow[i] - std::numeric_limits<double>::epsilon() ||
			m_pLow[i] > r.m_pLow[i] + std::numeric_limits<double>::epsilon() ||
			m_pHigh[i] < r.m_pHigh[i] - std::numeric_limits<double>::epsilon() ||
			m_pHigh[i] > r.m_pHigh[i] + std::numeric_limits<double>::epsilon())
			return false;
	}
	return true;
}

//
// IShape interface
//
bool Region::intersectsShape(const SpatialIndex::IShape& s) const
{
	const Region* pr = dynamic_cast<const Region*>(&s);
	if (pr != 0) return intersectsRegion(*pr);

	const Point* ppt = dynamic_cast<const Point*>(&s);
	if (ppt != 0) return containsPoint(*ppt);

	throw IllegalStateException("intersectsShape: Not implemented yet!");
}

bool Region::containsShape(const SpatialIndex::IShape& s) const
{
	const Region* pr = dynamic_cast<const Region*>(&s);
	if (pr != 0) return containsRegion(*pr);

	const Point* ppt = dynamic_cast<const Point*>(&s);
	if (ppt != 0) return containsPoint(*ppt);

	throw IllegalStateException("containsShape: Not implemented yet!");
}

bool Region::touchesShape(const SpatialIndex::IShape& s) const
{
	const Region* pr = dynamic_cast<const Region*>(&s);
	if (pr != 0) return touchesRegion(*pr);

	const Point* ppt = dynamic_cast<const Point*>(&s);
	if (ppt != 0) return touchesPoint(*ppt);

	throw IllegalStateException("touches: Not implemented yet!");
}

void Region::getCenter(Point& out) const
{
	if (m_dimension != out.m_dimension)
	{
		delete[] out.m_pCoords;
		out.m_dimension = m_dimension;
		out.m_pCoords = new double[m_dimension];
	}

	for (unsigned long i = 0; i < m_dimension; i++)
	{
		out.m_pCoords[i] = (m_pLow[i] + m_pHigh[i]) / 2.0;
	}
}

unsigned long Region::getDimension() const
{
	return m_dimension;
}

void Region::getMBR(Region& out) const
{
	out = *this;
}

double Region::getArea() const
{
	double area = 1.0;

	for (unsigned long i = 0; i < m_dimension; i++)
	{
		area *= m_pHigh[i] - m_pLow[i];
	}

	return area;
}

double Region::getMinimumDistance(const IShape& s) const
{
	const Region* pr = dynamic_cast<const Region*>(&s);
	if (pr != 0) return getMinimumDistance(*pr);

	const Point* ppt = dynamic_cast<const Point*>(&s);
	if (ppt != 0) return getMinimumDistance(*ppt);

	throw IllegalStateException("getMinimumDistance: Not implemented yet!");
}

bool Region::intersectsRegion(const Region& r) const
{
	if (m_dimension != r.m_dimension) throw IllegalArgumentException("intersectsRegion: Regions have different number of dimensions.");

	for (unsigned long i = 0; i < m_dimension; i++)
	{
		if (m_pLow[i] > r.m_pHigh[i] || m_pHigh[i] < r.m_pLow[i]) return false;
	}
	return true;
}

bool Region::containsRegion(const Region& r) const
{
	if (m_dimension != r.m_dimension) throw IllegalArgumentException("containsRegion: Regions have different number of dimensions.");

	for (unsigned long i = 0; i < m_dimension; i++)
	{
		if (m_pLow[i] > r.m_pLow[i] || m_pHigh[i] < r.m_pHigh[i]) return false;
	}
	return true;
}

bool Region::touchesRegion(const Region& r) const
{
	if (m_dimension != r.m_dimension) throw IllegalArgumentException("touchesRegion: Regions have different number of dimensions.");

	for (unsigned long i = 0; i < m_dimension; i++)
	{
		if (
			(m_pLow[i] > r.m_pLow[i] - std::numeric_limits<double>::epsilon() &&
			 m_pLow[i] < r.m_pLow[i] + std::numeric_limits<double>::epsilon()) ||
			(m_pHigh[i] > r.m_pHigh[i] - std::numeric_limits<double>::epsilon() &&
			 m_pHigh[i] < r.m_pHigh[i] + std::numeric_limits<double>::epsilon()))
			return true;
	}
	return false;
}

double Region::getMinimumDistance(const Region& r) const
{
	if (m_dimension != r.m_dimension) throw IllegalArgumentException("getMinimumDistance: Shapes have different number of dimensions.");

	double ret = 0.0;

	for (unsigned long i = 0; i < m_dimension; i++)
	{
		double x = 0.0;

		if (r.m_pHigh[i] < m_pLow[i])
		{
			x = std::abs(r.m_pHigh[i] - m_pLow[i]);
		}
		else if (m_pHigh[i] < r.m_pLow[i])
		{
			x = std::abs(r.m_pLow[i] - m_pHigh[i]);
		}

		ret += x * x;
	}

	return std::sqrt(ret);
}

bool Region::containsPoint(const Point& p) const
{
	if (m_dimension != p.m_dimension) throw IllegalArgumentException("containsPoint: Shapes have different number of dimensions.");

	for (unsigned long i = 0; i < m_dimension; i++)
	{
		if (m_pLow[i] > p.getCoordinate(i) || m_pHigh[i] < p.getCoordinate(i)) return false;
	}
	return true;
}

bool Region::touchesPoint(const Point& p) const
{
	if (m_dimension != p.m_dimension) throw IllegalArgumentException("touchesPoint: Shapes have different number of dimensions.");

	for (unsigned long i = 0; i < m_dimension; i++)
	{
		if (
			(m_pLow[i] > p.getCoordinate(i) - std::numeric_limits<double>::epsilon() &&
			 m_pLow[i] < p.getCoordinate(i) + std::numeric_limits<double>::epsilon()) ||
			(m_pHigh[i] > p.getCoordinate(i) - std::numeric_limits<double>::epsilon() &&
			 m_pHigh[i] < p.getCoordinate(i) + std::numeric_limits<double>::epsilon()))
			return true;
	}
	return false;
}

double Region::getMinimumDistance(const Point& p) const
{
	if (m_dimension != p.m_dimension) throw IllegalArgumentException("getMinimumDistance: Shapes have different number of dimensions.");

	double ret = 0.0;

	for (unsigned long i = 0; i < m_dimension; i++)
	{
		if (p.getCoordinate(i) < m_pLow[i])
		{
			ret += std::pow(m_pLow[i] - p.getCoordinate(i), 2.0);
		}
		else if (p.getCoordinate(i) > m_pHigh[i])
		{
			ret += std::pow(p.getCoordinate(i) - m_pHigh[i], 2.0);
		}
	}

	return std::sqrt(ret);
}

double Region::getIntersectingArea(const Region& r) const
{
	if (m_dimension != r.m_dimension) throw IllegalArgumentException("getIntersectingArea: Shapes have different number of dimensions.");

	unsigned long i;

	// check for intersection.
	// marioh: avoid function call since this is called billions of times.
	for (i = 0; i < m_dimension; i++)
	{
		if (m_pLow[i] > r.m_pHigh[i] || m_pHigh[i] < r.m_pLow[i]) return 0.0;
	}

	double ret = 1.0;
	double f1, f2;

	for (i = 0; i < m_dimension; i++)
	{
		f1 = std::max(m_pLow[i], r.m_pLow[i]);
		f2 = std::min(m_pHigh[i], r.m_pHigh[i]);
		ret *= f2 - f1;
	}

	return ret;
}

//
// Returns the margin of a region. It is calcuated as the sum of  2^(d-1) * width in each dimension.
//
double Region::getMargin() const
{
	double mul = std::pow(2.0, static_cast<double>(m_dimension) - 1.0);
	double margin = 0.0;

	for (unsigned long i = 0; i < m_dimension; i++)
	{
		margin += (m_pHigh[i] - m_pLow[i]) * mul;
	}

	return margin;
}

void Region::combineRegion(const Region& r)
{
	if (m_dimension != r.m_dimension) throw IllegalArgumentException("combineRegion: Regions have different number of dimensions.");

	for (unsigned long cDim = 0; cDim < m_dimension; cDim++)
	{
		m_pLow[cDim] = std::min(m_pLow[cDim], r.m_pLow[cDim]);
		m_pHigh[cDim] = std::max(m_pHigh[cDim], r.m_pHigh[cDim]);
	}
}

void Region::getCombinedRegion(Region& out, const Region& in) const
{
	if (m_dimension != in.m_dimension) throw IllegalArgumentException("getCombinedRegion: Regions have different number of dimensions.");

	out = *this;
	out.combineRegion(in);
}

double Region::getLow(unsigned long index) const throw (IndexOutOfBoundsException)
{
	if (index >= m_dimension) throw IndexOutOfBoundsException(index);

	return m_pLow[index];
}

double Region::getHigh(unsigned long index) const throw (IndexOutOfBoundsException)
{
	if (index >= m_dimension) throw IndexOutOfBoundsException(index);

	return m_pHigh[index];
}

std::ostream& SpatialIndex::operator<<(std::ostream& os, const Region& r)
{
	unsigned long i;

	os << "Low: ";
	for (i = 0; i < r.m_dimension; i++)
	{
		os << r.m_pLow[i] << " ";
	}

	os << ", High: ";

	for (i = 0; i < r.m_dimension; i++)
	{
		os << r.m_pHigh[i] << " ";
	}

	return os;
}
