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

#include "SpatialIndexImpl.h"

#include "../rtree/RTree.h"

using namespace SpatialIndex;

IndexOutOfBoundsException::IndexOutOfBoundsException(int i)
{
	std::ostringstream s;
	s << "Invalid index " << i;
	m_error = s.str();
}

IndexOutOfBoundsException::~IndexOutOfBoundsException()
{
}

std::string IndexOutOfBoundsException::what()
{
	return "IndexOutOfBoundsException: " + m_error;
}

IllegalArgumentException::IllegalArgumentException(std::string s) : m_error(s)
{
}

IllegalArgumentException::~IllegalArgumentException()
{
}

std::string IllegalArgumentException::what()
{
	return "IllegalArgumentException: " + m_error;
}

IllegalStateException::IllegalStateException(std::string s) : m_error(s)
{
}

IllegalStateException::~IllegalStateException()
{
}

std::string IllegalStateException::what()
{
	return "IllegalStateException: " + m_error + "\nPlease contact " + EMAIL;
}

ResourceLockedException::ResourceLockedException(std::string s) : m_error(s)
{
}

ResourceLockedException::~ResourceLockedException()
{
}

std::string ResourceLockedException::what()
{
	return "ResourceLockedException: " + m_error;
}

Variant::Variant() : varType(VT_EMPTY)
{
}

Variant PropertySet::getProperty(std::string property)
{
	std::map<std::string, Variant>::iterator it = m_propertySet.find(property);
	if (it != m_propertySet.end()) return (*it).second;
	else return Variant();
}

void PropertySet::setProperty(std::string property, Variant& v)
{
	m_propertySet.insert(std::pair<std::string, Variant>(property, v));
}

void PropertySet::removeProperty(std::string property)
{
	std::map<std::string, Variant>::iterator it = m_propertySet.find(property);
	if (it != m_propertySet.end()) m_propertySet.erase(it);
}

std::ostream& SpatialIndex::operator<<(std::ostream& os, const ISpatialIndex& i)
{
	using std::endl;

	const SpatialIndex::RTree::RTree* pRTree = dynamic_cast<const SpatialIndex::RTree::RTree*>(&i);
	if (pRTree != 0)
	{
		os << *pRTree;
		return os;
	}

	std::cerr << "ISpatialIndex operator<<: Not implemented yet for this index type." << std::endl;
	return os;
}

std::ostream& SpatialIndex::operator<<(std::ostream& os, const IStatistics& s)
{
	using std::endl;

	const SpatialIndex::RTree::Statistics* pRTreeStats = dynamic_cast<const SpatialIndex::RTree::Statistics*>(&s);
	if (pRTreeStats != 0)
	{
		os << *pRTreeStats;
		return os;
	}

	std::cerr << "IStatistics operator<<: Not implemented yet for this index type." << std::endl;
	return os;
}

