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

TimePoint::TimePoint() : Point(), m_startTime(-std::numeric_limits<double>::max()), m_endTime(std::numeric_limits<double>::max())
{
}

TimePoint::TimePoint(const double* pCoords, const IInterval& ti, unsigned long dimension) :
	Point(pCoords, dimension), m_startTime(ti.getLowerBound()), m_endTime(ti.getUpperBound())
{
}

TimePoint::TimePoint(const double* pCoords, double tStart, double tEnd, unsigned long dimension) :
	Point(pCoords, dimension), m_startTime(tStart), m_endTime(tEnd)
{
}

TimePoint::TimePoint(const Point& p, const IInterval& ti) :
	Point(p), m_startTime(ti.getLowerBound()), m_endTime(ti.getUpperBound())
{
}

TimePoint::TimePoint(const Point& p, double tStart, double tEnd) :
	Point(p), m_startTime(tStart), m_endTime(tEnd)
{
}

TimePoint::TimePoint(const TimePoint& p) :
	m_startTime(p.m_startTime), m_endTime(p.m_endTime)
{
	m_dimension = p.m_dimension;

	m_pCoords = new double[m_dimension];
	memcpy(m_pCoords, p.m_pCoords, m_dimension * sizeof(double));
}

TimePoint::~TimePoint()
{
}

TimePoint& TimePoint::operator=(const TimePoint& p)
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
		m_startTime = p.m_startTime;
		m_endTime = p.m_endTime;
	}

	return *this;
}

bool TimePoint::operator==(const TimePoint& p) const
{
	if (
		m_startTime < p.m_startTime - std::numeric_limits<double>::epsilon() ||
		m_startTime > p.m_startTime + std::numeric_limits<double>::epsilon() ||
		m_endTime < p.m_endTime - std::numeric_limits<double>::epsilon() ||
		m_endTime > p.m_endTime + std::numeric_limits<double>::epsilon())
		return false;

	for (unsigned long cDim = 0; cDim < m_dimension; cDim++)
	{
		if (
			m_pCoords[cDim] < p.m_pCoords[cDim] - std::numeric_limits<double>::epsilon() ||
			m_pCoords[cDim] > p.m_pCoords[cDim] + std::numeric_limits<double>::epsilon()) 
			return false;
	}

	return true;
}

//
// IInterval interface
//

double TimePoint::getLowerBound() const
{
	return m_startTime;
}

double TimePoint::getUpperBound() const
{
	return m_endTime;
}

bool TimePoint::intersectsInterval(const IInterval& ti) const
{
	return intersectsInterval(ti.getLowerBound(), ti.getUpperBound());
}

bool TimePoint::intersectsInterval(const double start, const double end) const
{
	//if (m_startTime != start &&
	//		(m_startTime >= end || m_endTime <= start)) return false;
	if (m_startTime >= end || m_endTime <= start) return false;

	return true;
}


bool TimePoint::containsInterval(const IInterval& ti) const
{
	if (m_startTime <= ti.getLowerBound() && m_endTime >= ti.getUpperBound()) return true;
	return false;
}

std::ostream& SpatialIndex::operator<<(std::ostream& os, const TimePoint& pt)
{
	unsigned long i;

	for (i = 0; i < pt.m_dimension; i++)
	{
		os << pt.m_pCoords[i] << " ";
	}

	os << ", Start: " << pt.m_startTime << ", End: " << pt.m_endTime;

	return os;
}
