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

#include <time.h>
#include <stdlib.h>

#include "../spatialindex/SpatialIndexImpl.h"

#include "StorageManager.h"
#include "RandomEvictionsBuffer.h"

using namespace SpatialIndex;
using namespace SpatialIndex::StorageManager;
using std::map;

IBuffer* SpatialIndex::StorageManager::returnRandomEvictionsBuffer(IStorageManager& sm, PropertySet& ps)
	throw (std::exception, SpatialIndex::Exception)
{
	try
	{
		IBuffer* b = new RandomEvictionsBuffer(sm, ps);
		return b;
	}
	catch (...)
	{
		throw;
	}
}

IBuffer* SpatialIndex::StorageManager::createNewRandomEvictionsBuffer(IStorageManager& sm, unsigned int capacity, bool bWriteThrough)
	throw (std::exception, SpatialIndex::Exception)
{
	Variant var;
	PropertySet ps;

	var.varType = VT_ULONG;
	var.val.ulVal = capacity;
	ps.setProperty("Capacity", var);

	var.varType = VT_BOOL;
	var.val.blVal = bWriteThrough;
	ps.setProperty("WriteThrough", var);

	return returnRandomEvictionsBuffer(sm, ps);
}

RandomEvictionsBuffer::RandomEvictionsBuffer(IStorageManager& sm, PropertySet& ps) : Buffer(sm, ps)
{
	srand48(time(NULL));
}

RandomEvictionsBuffer::~RandomEvictionsBuffer()
{
}

void RandomEvictionsBuffer::addEntry(long id, Entry* e)
{
	assert(m_buffer.size() <= m_capacity);

	if (m_buffer.size() == m_capacity) removeEntry();
	assert(m_buffer.find(id) == m_buffer.end());
	m_buffer.insert(std::pair<long, Entry*>(id, e));
}

void RandomEvictionsBuffer::removeEntry()
{
	if (m_buffer.size() == 0) return;

	unsigned int entry = (unsigned int) floor(((double) m_buffer.size()) * drand48());

	map<long, Entry*>::iterator it = m_buffer.begin();
	for (unsigned int cIndex = 0; cIndex < entry; cIndex++) it++;

	if ((*it).second->m_bDirty)
	{
		long id = (*it).first;
		m_pStorageManager->storeByteArray(id, ((*it).second)->m_length, (const byte *) ((*it).second)->m_pData);
	}

	delete (*it).second;
	m_buffer.erase(it);
}
