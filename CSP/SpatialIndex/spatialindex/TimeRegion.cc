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
using namespace std;

TimeRegion::TimeRegion()
	: Region(), m_startTime(-std::numeric_limits<double>::max()), m_endTime(std::numeric_limits<double>::max())
{
}

TimeRegion::TimeRegion(const double* pLow, const double* pHigh, const IInterval& ti, unsigned long dimension)
	: Region(pLow, pHigh, dimension), m_startTime(ti.getLowerBound()), m_endTime(ti.getUpperBound())
{
}

TimeRegion::TimeRegion(const double* pLow, const double* pHigh, double tStart, double tEnd, unsigned long dimension)
	: Region(pLow, pHigh, dimension), m_startTime(tStart), m_endTime(tEnd)
{
}

TimeRegion::TimeRegion(const Point& low, const Point& high, const IInterval& ti)
	: Region(low, high), m_startTime(ti.getLowerBound()), m_endTime(ti.getUpperBound())
{
}

TimeRegion::TimeRegion(const Point& low, const Point& high, double tStart, double tEnd)
	: Region(low, high), m_startTime(tStart), m_endTime(tEnd)
{
}

TimeRegion::TimeRegion(const Region& r, const IInterval& ti)
	:	Region(r), m_startTime(ti.getLowerBound()), m_endTime(ti.getUpperBound())
{
}

TimeRegion::TimeRegion(const Region& r, double tStart, double tEnd)
	:	Region(r), m_startTime(tStart), m_endTime(tEnd)
{
}

TimeRegion::TimeRegion(const TimePoint& low, const TimePoint& high)
	:	Region((Point&) low, (Point&) high), m_startTime(low.m_startTime), m_endTime(high.m_endTime)
{
}

TimeRegion::TimeRegion(const TimeRegion& r)
	: Region(r), IInterval(r), m_startTime(r.m_startTime), m_endTime(r.m_endTime)
{
	m_dimension = r.m_dimension;

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

TimeRegion::~TimeRegion()
{
}

TimeRegion& TimeRegion::operator=(const TimeRegion& r)
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

		m_startTime = r.m_startTime;
		m_endTime = r.m_endTime;
	}

	return *this;
}

bool TimeRegion::operator==(const TimeRegion& r) const
{
	if (	m_startTime < r.m_startTime - std::numeric_limits<double>::epsilon() ||
		m_startTime > r.m_startTime + std::numeric_limits<double>::epsilon() ||
		m_endTime < r.m_endTime - std::numeric_limits<double>::epsilon() ||
		m_endTime > r.m_endTime + std::numeric_limits<double>::epsilon())
		return false;

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

bool TimeRegion::intersectsRegionInTime(const TimeRegion& r) const
{
	// they should just intersect in time.
	if (
		m_startTime != r.m_startTime &&
		(m_startTime >= r.m_endTime || m_endTime <= r.m_startTime))
		return false;

	return Region::intersectsRegion(r);
}

bool TimeRegion::containsRegionInTime(const TimeRegion& r) const
{
	// they should just intersect in time.
	if (	m_startTime != r.m_startTime &&
		(m_startTime >= r.m_endTime || m_endTime <= r.m_startTime))
		return false;


	return Region::containsRegion(r);
}

bool TimeRegion::touchesRegionInTime(const TimeRegion& r) const
{
	// they should just intersect in time.
	if (	m_startTime != r.m_startTime &&
		(m_startTime >= r.m_endTime || m_endTime <= r.m_startTime))
		return false;


	return Region::touchesRegion(r);
}

bool TimeRegion::containsPointInTime(const TimePoint& p) const
{
	// they should just intersect in time.
	if (	m_startTime != p.m_startTime &&
		(m_startTime >= p.m_endTime || m_endTime <= p.m_startTime))
		return false;


	return Region::containsPoint(p);
}

bool TimeRegion::touchesPointInTime(const TimePoint& p) const
{
	// they should just intersect in time.
	if (	m_startTime != p.m_startTime &&
		(m_startTime >= p.m_endTime || m_endTime <= p.m_startTime))
		return false;


	return Region::touchesPoint(p);
}

void TimeRegion::combineRegionInTime(const TimeRegion& r)
{
	Region::combineRegion(r);

	m_startTime = std::min(m_startTime, r.m_startTime);
	m_endTime = std::max(m_endTime, r.m_endTime);
}

void TimeRegion::getCombinedRegionInTime(TimeRegion& out, const TimeRegion& in) const
{
	Region::getCombinedRegion(out, in);

	out.m_startTime = std::min(m_startTime, in.m_startTime);
	out.m_endTime = std::max(m_endTime, in.m_endTime);
}

//
// IInterval interface
//

double TimeRegion::getLowerBound() const
{
	return m_startTime;
}

double TimeRegion::getUpperBound() const
{
	return m_endTime;
}

bool TimeRegion::intersectsInterval(const IInterval& ti) const
{
	return intersectsInterval(ti.getLowerBound(), ti.getUpperBound());
}

bool TimeRegion::intersectsInterval(const double start, const double end) const
{
	//if (m_startTime != start &&
	//		(m_startTime >= end || m_endTime <= start)) return false;
	if (m_startTime >= end || m_endTime <= start) return false;

	return true;
}

bool TimeRegion::containsInterval(const IInterval& ti) const
{
	if (m_startTime <= ti.getLowerBound() && m_endTime >= ti.getUpperBound()) return true;
	return false;
}

void TimeRegion::makeInfinite(unsigned long dimension)
{
	if (dimension != m_dimension)
	{
		delete[] m_pLow;
		delete[] m_pHigh;

		m_pLow = 0;
		m_pHigh = 0;
		m_dimension = dimension;

		try
		{
			m_pLow = new double[m_dimension];
			m_pHigh = new double[m_dimension];
		}
		catch (...)
		{
			delete[] m_pLow;
			delete[] m_pHigh;
		}
	}

	for (unsigned long cIndex = 0; cIndex < m_dimension; cIndex++)
	{
		m_pLow[cIndex] = std::numeric_limits<double>::max();
		m_pHigh[cIndex] = -std::numeric_limits<double>::max();
	}

	m_startTime = std::numeric_limits<double>::max();
	m_endTime = -std::numeric_limits<double>::max();
}

std::ostream& SpatialIndex::operator<<(std::ostream& os, const TimeRegion& r)
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

	os << ", Start: " << r.m_startTime << ", End: " << r.m_endTime;

	return os;
}
