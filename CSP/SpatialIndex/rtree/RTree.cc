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

#include <queue>

#include "../spatialindex/SpatialIndexImpl.h"
#include "../storagemanager/StorageManager.h"

#include "Node.h"
#include "Leaf.h"
#include "Index.h"

#include "RTree.h"

using std::stack;
using std::vector;
using std::priority_queue;
using namespace SpatialIndex::RTree;

SpatialIndex::ISpatialIndex* SpatialIndex::RTree::returnRTree(SpatialIndex::IStorageManager& sm, SpatialIndex::PropertySet& ps)
	throw (std::exception, SpatialIndex::Exception)
{
	SpatialIndex::ISpatialIndex* si = new SpatialIndex::RTree::RTree(sm, ps);
	return si;
}

SpatialIndex::ISpatialIndex* SpatialIndex::RTree::createNewRTree(SpatialIndex::IStorageManager& sm, double fillFactor, unsigned long indexCapacity,
	unsigned long leafCapacity, unsigned long dimension, RTreeVariant rv, long& indexIdentifier)
	throw (std::exception, SpatialIndex::Exception)
{
	Variant var;
	PropertySet ps;

	var.varType = VT_DOUBLE;
	var.val.dblVal = fillFactor;
	ps.setProperty("FillFactor", var);

	var.varType = VT_ULONG;
	var.val.ulVal = indexCapacity;
	ps.setProperty("IndexCapacity", var);

	var.varType = VT_ULONG;
	var.val.ulVal = leafCapacity;
	ps.setProperty("LeafCapacity", var);

	var.varType = VT_ULONG;
	var.val.ulVal = dimension;
	ps.setProperty("Dimension", var);

	var.varType = VT_LONG;
	var.val.lVal = rv;
	ps.setProperty("TreeVariant", var);

	ISpatialIndex* ret = returnRTree(sm, ps);

	var = ps.getProperty("IndexIdentifier");
	indexIdentifier = var.val.lVal;

	return ret;
}

SpatialIndex::ISpatialIndex* SpatialIndex::RTree::loadRTree(IStorageManager& sm, long indexIdentifier)
	throw (std::exception, SpatialIndex::Exception)
{
	Variant var;
	PropertySet ps;

	var.varType = VT_LONG;
	var.val.lVal = indexIdentifier;
	ps.setProperty("IndexIdentifier", var);

	return returnRTree(sm, ps);
}

RTree::RTree(IStorageManager& sm, PropertySet& ps) :
	m_pStorageManager(&sm),
	m_rootID(StorageManager::NewPage),
	m_headerID(StorageManager::NewPage),
	m_treeVariant(RV_RSTAR),
	m_fillFactor(0.7),
	m_indexCapacity(100),
	m_leafCapacity(100),
	m_nearMinimumOverlapFactor(32),
	m_splitDistributionFactor(0.4),
	m_reinsertFactor(0.3),
	m_dimension(2),
	m_bTightMBRs(true),
	m_pointPool(500),
	m_regionPool(1000),
	m_indexPool(100),
	m_leafPool(100)
{
#ifdef PTHREADS
	pthread_rwlock_init(&m_rwLock, NULL);
#else
	m_rwLock = false;
#endif

	Variant var = ps.getProperty("IndexIdentifier");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_LONG) throw IllegalArgumentException("Property IndexIdentifier must be VT_LONG");
		m_headerID = var.val.lVal;
		initOld(ps);
	}
	else
	{
		initNew(ps);
		var.varType = VT_LONG;
		var.val.lVal = m_headerID;
		ps.setProperty("IndexIdentifier", var);
	}
}

RTree::~RTree()
{
#ifdef PTHREADS
	pthread_rwlock_destroy(&m_rwLock);
#endif

	storeHeader();
}

//
// ISpatialIndex interface
//

void RTree::insertData(unsigned long len, const byte* pData, const IShape& shape, long id) throw (Exception, std::exception)
{
	if (shape.getDimension() != m_dimension) throw IllegalArgumentException("insertData: Shape has the wrong number of dimensions.");

#ifdef PTHREADS
	ExclusiveLock lock(&m_rwLock);
#else
	if (m_rwLock == false) m_rwLock = true;
	else throw ResourceLockedException("insertData cannot acquire an exclusive lock");
#endif

	try
	{
		RegionPtr mbr = m_regionPool.acquire();
		shape.getMBR(*mbr);

		byte* buffer = 0;

		if (len > 0)
		{
			buffer = new byte[len];
			memcpy(buffer, pData, len);
		}

		insertData_impl(len, buffer, *mbr, id);
			// the buffer is stored in the tree. Do not delete here.

#ifndef PTHREADS
		m_rwLock = false;
#endif
	}
	catch (...)
	{
#ifndef PTHREADS
		m_rwLock = false;
#endif
		throw;
	}
}

bool RTree::deleteData(const IShape& shape, long id) throw (Exception, std::exception)
{
	if (shape.getDimension() != m_dimension) throw IllegalArgumentException("deleteData: Shape has the wrong number of dimensions.");

#ifdef PTHREADS
	ExclusiveLock lock(&m_rwLock);
#else
	if (m_rwLock == false) m_rwLock = true;
	else throw ResourceLockedException("deleteData cannot acquire an exclusive lock");
#endif

	try
	{
		RegionPtr mbr = m_regionPool.acquire();
		shape.getMBR(*mbr);
		bool ret = deleteData_impl(*mbr, id);

#ifndef PTHREADS
		m_rwLock = false;
#endif

		return ret;
	}
	catch (...)
	{
#ifndef PTHREADS
		m_rwLock = false;
#endif
		throw;
	}
}

void RTree::containsWhatQuery(const IShape& query, IVisitor& v) throw (Exception, std::exception)
{
	if (query.getDimension() != m_dimension) throw IllegalArgumentException("containmentQuery: Shape has the wrong number of dimensions.");
	rangeQuery(ContainmentQuery, query, v);
}

void RTree::intersectsWithQuery(const IShape& query, IVisitor& v) throw (Exception, std::exception)
{
	if (query.getDimension() != m_dimension) throw IllegalArgumentException("intersectionQuery: Shape has the wrong number of dimensions.");
	rangeQuery(IntersectionQuery, query, v);
}

void RTree::pointLocationQuery(const Point& query, IVisitor& v) throw (Exception, std::exception)
{
	if (query.m_dimension != m_dimension) throw IllegalArgumentException("pointLocationQuery: Shape has the wrong number of dimensions.");
	Region r(query, query);
	rangeQuery(IntersectionQuery, r, v);
}

void RTree::nearestNeighborQuery(long k, const IShape& query, IVisitor& v, INearestNeighborComparator& nnc) throw (Exception, std::exception)
{
	if (query.getDimension() != m_dimension) throw IllegalArgumentException("nearestNeighborQuery: Shape has the wrong number of dimensions.");

#ifdef PTHREADS
	SharedLock lock(&m_rwLock);
#else
	if (m_rwLock == false) m_rwLock = true;
	else throw ResourceLockedException("nearestNeighborQuery cannot acquire a shared lock");
#endif

	try
	{
		priority_queue<NNEntry*, vector<NNEntry*>, NNEntry::greater> queue;

		queue.push(new NNEntry(m_rootID, 0, 0.0));

		long count = 0;
		double knearest = 0.0;

		while (! queue.empty())
		{
			NNEntry* pFirst = queue.top();

			// report all nearest neighbors with equal greatest distances.
			// (neighbors can be more than k, if many happen to have the same greatest distance).
			if (count >= k && pFirst->m_minDist > knearest)	break;

			queue.pop();

			if (pFirst->m_pEntry == 0)
			{
				// n is a leaf or an index.
				NodePtr n = readNode(pFirst->m_id);
				v.visitNode(*n);

				for (unsigned long cChild = 0; cChild < n->m_children; cChild++)
				{
					if (n->m_level == 0)
					{
						Data* e = new Data(n->m_pDataLength[cChild], n->m_pData[cChild], n->m_ptrMBR[cChild], n->m_pIdentifier[cChild]);
						queue.push(new NNEntry(n->m_pIdentifier[cChild], e, nnc.getMinimumDistance(query, *(e->m_shape.get()))));
					}
					else
					{
						queue.push(new NNEntry(n->m_pIdentifier[cChild], 0, nnc.getMinimumDistance(query, *(n->m_ptrMBR[cChild].get()))));
					}
				}
			}
			else
			{
				v.visitData(*(static_cast<IData*>(pFirst->m_pEntry)));
				m_stats.m_queryResults++;
				count++;
				knearest = pFirst->m_minDist;
				delete pFirst->m_pEntry;
			}

			delete pFirst;
		}

		while (! queue.empty())
		{
			NNEntry* e = queue.top(); queue.pop();
			if (e->m_pEntry != 0) delete e->m_pEntry;
			delete e;
		}

#ifndef PTHREADS
		m_rwLock = false;
#endif
	}
	catch (...)
	{
#ifndef PTHREADS
		m_rwLock = false;
#endif
		throw;
	}
}

void RTree::nearestNeighborQuery(long k, const IShape& query, IVisitor& v) throw (Exception, std::exception)
{
	if (query.getDimension() != m_dimension) throw IllegalArgumentException("nearestNeighborQuery: Shape has the wrong number of dimensions.");
	NNComparator nnc;
	nearestNeighborQuery(k, query, v, nnc);
}

void RTree::queryStrategy(IQueryStrategy& qs) throw (Exception, std::exception)
{
#ifdef PTHREADS
	SharedLock lock(&m_rwLock);
#else
	if (m_rwLock == false) m_rwLock = true;
	else throw ResourceLockedException("queryStrategy cannot acquire a shared lock");
#endif

	long next = m_rootID;
	bool hasNext = true;

	try
	{
		while (hasNext)
		{
			NodePtr n = readNode(next);
			qs.getNextEntry(*n, next, hasNext);
		}

#ifndef PTHREADS
		m_rwLock = false;
#endif
	}
	catch (...)
	{
#ifndef PTHREADS
		m_rwLock = false;
#endif
		throw;
	}
}

void RTree::getIndexProperties(PropertySet& out) const
{
	Variant var;

	// dimension
	var.varType = VT_ULONG;
	var.val.ulVal = m_dimension;
	out.setProperty("Dimension", var);

	// index capacity
	var.varType = VT_ULONG;
	var.val.ulVal = m_indexCapacity;
	out.setProperty("IndexCapacity", var);

	// leaf capacity
	var.varType = VT_ULONG;
	var.val.ulVal = m_leafCapacity;
	out.setProperty("LeafCapacity", var);

	// R-tree variant
	var.varType = VT_LONG;
	var.val.lVal = m_treeVariant;
	out.setProperty("TreeVariant", var);

	// fill factor
	var.varType = VT_DOUBLE;
	var.val.dblVal = m_fillFactor;
	out.setProperty("FillFactor", var);

	// near minimum overlap factor
	var.varType = VT_ULONG;
	var.val.ulVal = m_nearMinimumOverlapFactor;
	out.setProperty("NearMinimumOverlapFactor", var);

	// split distribution factor
	var.varType = VT_DOUBLE;
	var.val.dblVal = m_splitDistributionFactor;
	out.setProperty("SplitDistributionFactor", var);

	// reinsert factor
	var.varType = VT_DOUBLE;
	var.val.dblVal = m_reinsertFactor;
	out.setProperty("ReinsertFactor", var);

	// tight MBRs
	var.varType = VT_BOOL;
	var.val.blVal = m_bTightMBRs;
	out.setProperty("EnsureTightMBRs", var);

	// index pool capacity
	var.varType = VT_ULONG;
	var.val.ulVal = m_indexPool.getCapacity();
	out.setProperty("IndexPoolCapacity", var);

	// leaf pool capacity
	var.varType = VT_ULONG;
	var.val.ulVal = m_leafPool.getCapacity();
	out.setProperty("LeafPoolCapacity", var);

	// region pool capacity
	var.varType = VT_ULONG;
	var.val.ulVal = m_regionPool.getCapacity();
	out.setProperty("RegionPoolCapacity", var);

	// point pool capacity
	var.varType = VT_ULONG;
	var.val.ulVal = m_pointPool.getCapacity();
	out.setProperty("PointPoolCapacity", var);
}

void RTree::addCommand(ICommand* pCommand, CommandType ct)
{
	switch (ct)
	{
		case CT_NODEREAD:
			m_readNodeCommands.push_back(SmartPointer<ICommand>(pCommand));
			break;
		case CT_NODEWRITE:
			m_writeNodeCommands.push_back(SmartPointer<ICommand>(pCommand));
			break;
		case CT_NODEDELETE:
			m_deleteNodeCommands.push_back(SmartPointer<ICommand>(pCommand));
			break;
	}
}

bool RTree::isIndexValid() throw (Exception, std::exception)
{
	bool ret = true;
	stack<ValidateEntry> st;
	NodePtr root = readNode(m_rootID);

	if (root->m_level != m_stats.m_treeHeight - 1)
	{
		std::cerr << "Invalid tree height." << std::endl;
		return false;
	}

	std::map<unsigned long, unsigned long> nodesInLevel;
	nodesInLevel.insert(std::pair<long, long>(root->m_level, 1));

	ValidateEntry e(root->m_nodeMBR, root);
	st.push(e);

	while (! st.empty())
	{
		e = st.top(); st.pop();

		Region tmpRegion = m_infiniteRegion;

		for (unsigned long cDim = 0; cDim < tmpRegion.m_dimension; cDim++)
		{
			tmpRegion.m_pLow[cDim] = std::numeric_limits<double>::max();
			tmpRegion.m_pHigh[cDim] = -std::numeric_limits<double>::max();

			for (unsigned long cChild = 0; cChild < e.m_pNode->m_children; cChild++)
			{
				tmpRegion.m_pLow[cDim] = std::min(tmpRegion.m_pLow[cDim], e.m_pNode->m_ptrMBR[cChild]->m_pLow[cDim]);
				tmpRegion.m_pHigh[cDim] = std::max(tmpRegion.m_pHigh[cDim], e.m_pNode->m_ptrMBR[cChild]->m_pHigh[cDim]);
			}
		}

		if (! (tmpRegion == e.m_pNode->m_nodeMBR))
		{
			std::cerr << "Invalid parent information." << std::endl;
			ret = false;
		}
		else if (! (tmpRegion == e.m_parentMBR))
		{
			std::cerr << "Error in parent." << std::endl;
			ret = false;
		}

		if (e.m_pNode->m_level != 0)
		{
			for (unsigned long cChild = 0; cChild < e.m_pNode->m_children; cChild++)
			{
				NodePtr ptrN = readNode(e.m_pNode->m_pIdentifier[cChild]);
				ValidateEntry tmpEntry(*(e.m_pNode->m_ptrMBR[cChild]), ptrN);

				std::map<unsigned long, unsigned long>::iterator itNodes = nodesInLevel.find(tmpEntry.m_pNode->m_level);

				if (itNodes == nodesInLevel.end())
				{
					nodesInLevel.insert(std::pair<long, long>(tmpEntry.m_pNode->m_level, 1l));
				}
				else
				{
					nodesInLevel[tmpEntry.m_pNode->m_level] = nodesInLevel[tmpEntry.m_pNode->m_level] + 1;
				}

				st.push(tmpEntry);
			}
		}
	}

	unsigned long nodes = 0;
	for (unsigned long cLevel = 0; cLevel < m_stats.m_treeHeight; cLevel++)
	{
		if (nodesInLevel[cLevel] != m_stats.m_nodesInLevel[cLevel])
		{
			std::cerr << "Invalid nodesInLevel information." << std::endl;
			ret = false;
		}

		nodes += m_stats.m_nodesInLevel[cLevel];
	}

	if (nodes != m_stats.m_nodes)
	{
		std::cerr << "Invalid number of nodes information." << std::endl;
		ret = false;
	}

	return ret;
}

void RTree::getStatistics(IStatistics** out) const throw (std::exception)
{
	*out = new Statistics(m_stats);
}

void RTree::initNew(PropertySet& ps)
{
	Variant var;

	// tree variant
	var = ps.getProperty("TreeVariant");
	if (var.varType != VT_EMPTY)
	{
		if (	var.varType != VT_LONG ||
			(var.val.lVal != RV_LINEAR &&
			var.val.lVal != RV_QUADRATIC &&
			var.val.lVal != RV_RSTAR))
			throw IllegalArgumentException("Property TreeVariant must be VT_LONG and of RTreeVariant type");

		m_treeVariant = static_cast<RTreeVariant>(var.val.lVal);
	}

	// fill factor
	// it cannot be larger than 50%, since linear and quadratic split algorithms
	// require assigning to both nodes the same number of entries.
	var = ps.getProperty("FillFactor");
	if (var.varType != VT_EMPTY)
	{
		if (
			var.varType != VT_DOUBLE ||
			var.val.dblVal <= 0.0 ||
			((m_treeVariant == RV_LINEAR || m_treeVariant == RV_QUADRATIC) && var.val.dblVal > 0.5) ||
			var.val.dblVal >= 1.0)
			throw IllegalArgumentException("Property FillFactor must be VT_DOUBLE and in (0.0, 1.0) for RSTAR, (0.0, 0.5) for LINEAR and QUADRATIC");

		m_fillFactor = var.val.dblVal;
	}

	// index capacity
	var = ps.getProperty("IndexCapacity");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_ULONG || var.val.ulVal < 4) throw IllegalArgumentException("Property IndexCapacity must be VT_ULONG and >= 4");

		m_indexCapacity = var.val.ulVal;
	}

	// leaf capacity
	var = ps.getProperty("LeafCapacity");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_ULONG || var.val.ulVal < 4) throw IllegalArgumentException("Property LeafCapacity must be VT_ULONG and >= 4");

		m_leafCapacity = var.val.ulVal;
	}

	// near minimum overlap factor
	var = ps.getProperty("NearMinimumOverlapFactor");
	if (var.varType != VT_EMPTY)
	{
		if (
			var.varType != VT_ULONG ||
			var.val.ulVal < 1 ||
			var.val.ulVal > m_indexCapacity ||
			var.val.ulVal > m_leafCapacity)
			throw IllegalArgumentException("Property NearMinimumOverlapFactor must be VT_ULONG and less than both index and leaf capacities");

		m_nearMinimumOverlapFactor = var.val.ulVal;
	}

	// split distribution factor
	var = ps.getProperty("SplitDistributionFactor");
	if (var.varType != VT_EMPTY)
	{
		if (
			var.varType != VT_DOUBLE ||
			var.val.dblVal <= 0.0 ||
			var.val.dblVal >= 1.0)
			throw IllegalArgumentException("Property SplitDistributionFactor must be VT_DOUBLE and in (0.0, 1.0)");

		m_splitDistributionFactor = var.val.dblVal;
	}

	// reinsert factor
	var = ps.getProperty("ReinsertFactor");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_DOUBLE || var.val.dblVal <= 0.0 || var.val.dblVal >= 1.0)
			throw IllegalArgumentException("Property ReinsertFactor must be VT_DOUBLE and in (0.0, 1.0)");

		m_reinsertFactor = var.val.dblVal;
	}

	// dimension
	var = ps.getProperty("Dimension");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_ULONG) throw IllegalArgumentException("Property Dimension must be VT_ULONG");
		if (var.val.ulVal <= 1) throw IllegalArgumentException("Property Dimension must be greater than 1");

		m_dimension = var.val.ulVal;
	}

	// tight MBRs
	var = ps.getProperty("EnsureTightMBRs");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_BOOL) throw IllegalArgumentException("Property EnsureTightMBRs must be VT_BOOL");

		m_bTightMBRs = var.val.blVal;
	}

	// index pool capacity
	var = ps.getProperty("IndexPoolCapacity");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_ULONG) throw IllegalArgumentException("Property IndexPoolCapacity must be VT_ULONG");

		m_indexPool.setCapacity(var.val.ulVal);
	}

	// leaf pool capacity
	var = ps.getProperty("LeafPoolCapacity");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_ULONG) throw IllegalArgumentException("Property LeafPoolCapacity must be VT_ULONG");

		m_leafPool.setCapacity(var.val.ulVal);
	}

	// region pool capacity
	var = ps.getProperty("RegionPoolCapacity");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_ULONG) throw IllegalArgumentException("Property RegionPoolCapacity must be VT_ULONG");

		m_regionPool.setCapacity(var.val.ulVal);
	}

	// point pool capacity
	var = ps.getProperty("PointPoolCapacity");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_ULONG) throw IllegalArgumentException("Property PointPoolCapacity must be VT_ULONG");

		m_pointPool.setCapacity(var.val.ulVal);
	}

	m_infiniteRegion.m_pLow = new double[m_dimension];
	m_infiniteRegion.m_pHigh = new double[m_dimension];

	m_infiniteRegion.m_dimension = m_dimension;

	for (unsigned long cDim = 0; cDim < m_infiniteRegion.m_dimension; cDim++)
	{
		m_infiniteRegion.m_pLow[cDim] = std::numeric_limits<double>::max();
		m_infiniteRegion.m_pHigh[cDim] = -std::numeric_limits<double>::max();
	}

	m_stats.m_treeHeight = 1;
	m_stats.m_nodesInLevel.push_back(0);

	Leaf root(this, -1);
	m_rootID = writeNode(&root);

	storeHeader();
}

void RTree::initOld(PropertySet& ps)
{
	loadHeader();

	// only some of the properties may be changed.
	// the rest are just ignored.

	Variant var;

	// tree variant
	var = ps.getProperty("TreeVariant");
	if (var.varType != VT_EMPTY)
	{
		if (
			var.varType != VT_LONG ||
			(var.val.lVal != RV_LINEAR &&
			 var.val.lVal != RV_QUADRATIC &&
			 var.val.lVal != RV_RSTAR))
			throw IllegalArgumentException("Property TreeVariant must be VT_LONG and of RTreeVariant type");

		m_treeVariant = static_cast<RTreeVariant>(var.val.lVal);
	}

	// near minimum overlap factor
	var = ps.getProperty("NearMinimumOverlapFactor");
	if (var.varType != VT_EMPTY)
	{
		if (	
			var.varType != VT_ULONG ||
			var.val.ulVal < 1 ||
			var.val.ulVal > m_indexCapacity ||
			var.val.ulVal > m_leafCapacity)
			throw IllegalArgumentException("NearMinimumOverlapFactor must be VT_ULONG and less than both index and leaf capacities");

		m_nearMinimumOverlapFactor = var.val.ulVal;
	}

	// split distribution factor
	var = ps.getProperty("SplitDistributionFactor");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_DOUBLE || var.val.dblVal <= 0.0 || var.val.dblVal >= 1.0)
			throw IllegalArgumentException("Property SplitDistributionFactor must be VT_DOUBLE and in (0.0, 1.0)");

		m_splitDistributionFactor = var.val.dblVal;
	}

	// reinsert factor
	var = ps.getProperty("ReinsertFactor");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_DOUBLE || var.val.dblVal <= 0.0 || var.val.dblVal >= 1.0)
			throw IllegalArgumentException("Property ReinsertFactor must be VT_DOUBLE and in (0.0, 1.0)");

		m_reinsertFactor = var.val.dblVal;
	}

	// tight MBRs
	var = ps.getProperty("EnsureTightMBRs");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_BOOL) throw IllegalArgumentException("Property EnsureTightMBRs must be VT_BOOL");

		m_bTightMBRs = var.val.blVal;
	}

	// index pool capacity
	var = ps.getProperty("IndexPoolCapacity");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_ULONG) throw IllegalArgumentException("Property IndexPoolCapacity must be VT_ULONG");

		m_indexPool.setCapacity(var.val.ulVal);
	}

	// leaf pool capacity
	var = ps.getProperty("LeafPoolCapacity");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_ULONG) throw IllegalArgumentException("Property LeafPoolCapacity must be VT_ULONG");

		m_leafPool.setCapacity(var.val.ulVal);
	}

	// region pool capacity
	var = ps.getProperty("RegionPoolCapacity");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_ULONG) throw IllegalArgumentException("Property RegionPoolCapacity must be VT_ULONG");

		m_regionPool.setCapacity(var.val.ulVal);
	}

	// point pool capacity
	var = ps.getProperty("PointPoolCapacity");
	if (var.varType != VT_EMPTY)
	{
		if (var.varType != VT_ULONG) throw IllegalArgumentException("Property PointPoolCapacity must be VT_ULONG");

		m_pointPool.setCapacity(var.val.ulVal);
	}

	m_infiniteRegion.m_pLow = new double[m_dimension];
	m_infiniteRegion.m_pHigh = new double[m_dimension];

	m_infiniteRegion.m_dimension = m_dimension;

	for (unsigned long cDim = 0; cDim < m_infiniteRegion.m_dimension; cDim++)
	{
		m_infiniteRegion.m_pLow[cDim] = std::numeric_limits<double>::max();
		m_infiniteRegion.m_pHigh[cDim] = -std::numeric_limits<double>::max();
	}
}

void RTree::storeHeader()
{
	const long headerSize =
		sizeof(long) +							// m_rootID
		sizeof(long) +							// m_treeVariant
		sizeof(double) +						// m_fillFactor
		sizeof(unsigned long) +					// m_indexCapacity
		sizeof(unsigned long) +					// m_leafCapacity
		sizeof(unsigned long) +					// m_nearMinimumOverlapFactor
		sizeof(double) +						// m_splitDistributionFactor
		sizeof(double) +						// m_reinsertFactor
		sizeof(unsigned long) +					// m_dimension
		sizeof(char) +							// m_bTightMBRs
		sizeof(unsigned long) +					// m_stats.m_nodes
		sizeof(unsigned long) +					// m_stats.m_data
		sizeof(unsigned long) +					// m_stats.m_treeHeight
		m_stats.m_treeHeight * sizeof(unsigned long); 	// m_stats.m_nodesInLevel

	byte* header = new byte[headerSize];
	byte* ptr = header;

	memcpy(ptr, &m_rootID, sizeof(long));
	ptr += sizeof(long);
	memcpy(ptr, &m_treeVariant, sizeof(long));
	ptr += sizeof(long);
	memcpy(ptr, &m_fillFactor, sizeof(double));
	ptr += sizeof(double);
	memcpy(ptr, &m_indexCapacity, sizeof(unsigned long));
	ptr += sizeof(unsigned long);
	memcpy(ptr, &m_leafCapacity, sizeof(unsigned long));
	ptr += sizeof(unsigned long);
	memcpy(ptr, &m_nearMinimumOverlapFactor, sizeof(unsigned long));
	ptr += sizeof(unsigned long);
	memcpy(ptr, &m_splitDistributionFactor, sizeof(double));
	ptr += sizeof(double);
	memcpy(ptr, &m_reinsertFactor, sizeof(double));
	ptr += sizeof(double);
	memcpy(ptr, &m_dimension, sizeof(unsigned long));
	ptr += sizeof(unsigned long);
	char c = (char) m_bTightMBRs;
	memcpy(ptr, &c, sizeof(char));
	ptr += sizeof(char);
	memcpy(ptr, &(m_stats.m_nodes), sizeof(unsigned long));
	ptr += sizeof(unsigned long);
	memcpy(ptr, &(m_stats.m_data), sizeof(unsigned long));
	ptr += sizeof(unsigned long);
	memcpy(ptr, &(m_stats.m_treeHeight), sizeof(unsigned long));
	ptr += sizeof(unsigned long);

	for (unsigned long cLevel = 0; cLevel < m_stats.m_treeHeight; cLevel++)
	{
		memcpy(ptr, &(m_stats.m_nodesInLevel[cLevel]), sizeof(unsigned long));
		ptr += sizeof(unsigned long);
	}

	assert(headerSize == (ptr - header));

	m_pStorageManager->storeByteArray(m_headerID, headerSize, header);

	delete[] header;
}

void RTree::loadHeader()
{
	unsigned long headerSize;
	byte* header = 0;
	m_pStorageManager->loadByteArray(m_headerID, headerSize, &header);

	byte* ptr = header;

	memcpy(&m_rootID, ptr, sizeof(long));
	ptr += sizeof(long);
	memcpy(&m_treeVariant, ptr, sizeof(long));
	ptr += sizeof(long);
	memcpy(&m_fillFactor, ptr, sizeof(double));
	ptr += sizeof(double);
	memcpy(&m_indexCapacity, ptr, sizeof(unsigned long));
	ptr += sizeof(unsigned long);
	memcpy(&m_leafCapacity, ptr, sizeof(unsigned long));
	ptr += sizeof(unsigned long);
	memcpy(&m_nearMinimumOverlapFactor, ptr, sizeof(unsigned long));
	ptr += sizeof(unsigned long);
	memcpy(&m_splitDistributionFactor, ptr, sizeof(double));
	ptr += sizeof(double);
	memcpy(&m_reinsertFactor, ptr, sizeof(double));
	ptr += sizeof(double);
	memcpy(&m_dimension, ptr, sizeof(unsigned long));
	ptr += sizeof(unsigned long);
	char c;
	memcpy(&c, ptr, sizeof(char));
	m_bTightMBRs = (bool) c;
	ptr += sizeof(char);
	memcpy(&(m_stats.m_nodes), ptr, sizeof(unsigned long));
	ptr += sizeof(unsigned long);
	memcpy(&(m_stats.m_data), ptr, sizeof(unsigned long));
	ptr += sizeof(unsigned long);
	memcpy(&(m_stats.m_treeHeight), ptr, sizeof(unsigned long));
	ptr += sizeof(unsigned long);

	for (unsigned long cLevel = 0; cLevel < m_stats.m_treeHeight; cLevel++)
	{
		unsigned long cNodes;
		memcpy(&cNodes, ptr, sizeof(unsigned long));
		ptr += sizeof(unsigned long);
		m_stats.m_nodesInLevel.push_back(cNodes);
	}

	delete[] header;
}

void RTree::insertData_impl(unsigned long dataLength, byte* pData, Region& mbr, long id)
{
	assert(mbr.getDimension() == m_dimension);

	stack<long> pathBuffer;
	byte* overflowTable = 0;

	try
	{
		NodePtr root = readNode(m_rootID);

		overflowTable = new byte[root->m_level];
		bzero(overflowTable, root->m_level * sizeof(byte));

		NodePtr l = root->chooseSubtree(mbr, 0, pathBuffer);
		if (l.get() == root.get())
		{
			assert(root.unique());
			root.relinquish();
		}
		l->insertData(dataLength, pData, mbr, id, pathBuffer, overflowTable);

		delete[] overflowTable;
		m_stats.m_data++;
	}
	catch (...)
	{
		delete[] overflowTable;
		throw;
	}
}

void RTree::insertData_impl(unsigned long dataLength, byte* pData, Region& mbr, long id, unsigned long level, byte* overflowTable)
{
	assert(mbr.getDimension() == m_dimension);

	stack<long> pathBuffer;
	NodePtr root = readNode(m_rootID);
	NodePtr n = root->chooseSubtree(mbr, level, pathBuffer);

	assert(n->m_level == level);

	if (n.get() == root.get())
	{
		assert(root.unique());
		root.relinquish();
	}
	n->insertData(dataLength, pData, mbr, id, pathBuffer, overflowTable);
}

bool RTree::deleteData_impl(const Region& mbr, long id)
{
	assert(mbr.m_dimension == m_dimension);

	stack<long> pathBuffer;
	NodePtr root = readNode(m_rootID);
	NodePtr l = root->findLeaf(mbr, id, pathBuffer);
	if (l.get() == root.get())
	{
		assert(root.unique());
		root.relinquish();
	}

	if (l.get() != 0)
	{
		Leaf* pL = static_cast<Leaf*>(l.get());
		pL->deleteData(id, pathBuffer);
		m_stats.m_data--;
		return true;
	}

	return false;
}

long RTree::writeNode(Node* n)
{
	byte* buffer;
	unsigned long dataLength;
	n->store(dataLength, &buffer);

	long page;
	if (n->m_identifier < 0) page = StorageManager::NewPage;
	else page = n->m_identifier;

	try
	{
		m_pStorageManager->storeByteArray(page, dataLength, buffer);
		delete[] buffer;
	}
	catch (StorageManager::InvalidPageException& e)
	{
		delete[] buffer;
		std::cerr << e.what() << std::endl;
		//std::cerr << *this << std::endl;
		throw IllegalStateException("writeNode failed with InvalidPageException");
	}

	if (n->m_identifier < 0)
	{
		n->m_identifier = page;
		m_stats.m_nodes++;

#ifndef NDEBUG
		try
		{
			m_stats.m_nodesInLevel[n->m_level] = m_stats.m_nodesInLevel.at(n->m_level) + 1;
		}
		catch(...)
		{
			throw IllegalStateException("writeNode: writing past the end of m_nodesInLevel.");
		}
#else
		m_stats.m_nodesInLevel[n->m_level] = m_stats.m_nodesInLevel[n->m_level] + 1;
#endif
	}

	m_stats.m_writes++;

	for (unsigned long cIndex = 0; cIndex < m_writeNodeCommands.size(); cIndex++)
	{
		m_writeNodeCommands[cIndex]->execute(*n);
	}

	return page;
}

NodePtr RTree::readNode(unsigned long id)
{
	unsigned long dataLength;
	byte* buffer;

	try
	{
		m_pStorageManager->loadByteArray(id, dataLength, &buffer);
	}
	catch (StorageManager::InvalidPageException& e)
	{
		std::cerr << e.what() << std::endl;
		//std::cerr << *this << std::endl;
		throw IllegalStateException("readNode failed with InvalidPageException");
	}

	try
	{
		long nodeType;
		memcpy(&nodeType, buffer, sizeof(long));

		NodePtr n;

		if (nodeType == PersistentIndex) n = m_indexPool.acquire();
		else if (nodeType == PersistentLeaf) n = m_leafPool.acquire();
		else throw IllegalStateException("readNode failed reading the correct node type information");

		if (n.get() == 0)
		{
			if (nodeType == PersistentIndex) n = NodePtr(new Index(this, -1, 0), &m_indexPool);
			else if (nodeType == PersistentLeaf) n = NodePtr(new Leaf(this, -1), &m_leafPool);
		}

		//n->m_pTree = this;
		n->m_identifier = id;
		n->load(dataLength, buffer);

		m_stats.m_reads++;

		for (unsigned long cIndex = 0; cIndex < m_readNodeCommands.size(); cIndex++)
		{
			m_readNodeCommands[cIndex]->execute(*n);
		}

		delete[] buffer;
		return n;
	}
	catch (...)
	{
		delete[] buffer;
		throw;
	}
}

void RTree::deleteNode(Node* n)
{
	try
	{
		m_pStorageManager->deleteByteArray(n->m_identifier);
	}
	catch (StorageManager::InvalidPageException& e)
	{
		std::cerr << e.what() << std::endl;
		//std::cerr << *this << std::endl;
		throw IllegalStateException("deleteNode failed with InvalidPageException");
	}

	m_stats.m_nodes--;
	m_stats.m_nodesInLevel[n->m_level] = m_stats.m_nodesInLevel[n->m_level] - 1;

	for (unsigned long cIndex = 0; cIndex < m_deleteNodeCommands.size(); cIndex++)
	{
		m_deleteNodeCommands[cIndex]->execute(*n);
	}
}

void RTree::rangeQuery(RangeQueryType type, const IShape& query, IVisitor& v) throw (Exception, std::exception)
{
#ifdef PTHREADS
	SharedLock lock(&m_rwLock);
#else
	if (m_rwLock == false) m_rwLock = true;
	else throw ResourceLockedException("rangeQuery cannot acquire a shared lock");
#endif

	try
	{
		stack<NodePtr> st;
		NodePtr root = readNode(m_rootID);

		if (root->m_children > 0 && query.intersectsShape(root->m_nodeMBR)) st.push(root);

		while (! st.empty())
		{
			NodePtr n = st.top(); st.pop();

			if (n->m_level == 0)
			{
				v.visitNode(*n);

				for (unsigned long cChild = 0; cChild < n->m_children; cChild++)
				{
					bool b;
					if (type == ContainmentQuery) b = query.containsShape(*(n->m_ptrMBR[cChild]));
					else b = query.intersectsShape(*(n->m_ptrMBR[cChild]));

					if (b)
					{
						Data data = Data(n->m_pDataLength[cChild], n->m_pData[cChild], n->m_ptrMBR[cChild], n->m_pIdentifier[cChild]);
						v.visitData(data);
						m_stats.m_queryResults++;
					}
				}
			}
			else
			{
				v.visitNode(*n);

				for (unsigned long cChild = 0; cChild < n->m_children; cChild++)
				{
					if (query.intersectsShape(*(n->m_ptrMBR[cChild]))) st.push(readNode(n->m_pIdentifier[cChild]));
				}
			}
		}

#ifndef PTHREADS
		m_rwLock = false;
#endif
	}
	catch (...)
	{
#ifndef PTHREADS
		m_rwLock = false;
#endif
		throw;
	}
}

std::ostream& SpatialIndex::RTree::operator<<(std::ostream& os, const RTree& t)
{
	using std::endl;

	os	<< "Dimension: " << t.m_dimension << endl
		<< "Fill factor: " << t.m_fillFactor << endl
		<< "Index capacity: " << t.m_indexCapacity << endl
		<< "Leaf capacity: " << t.m_leafCapacity << endl;

	if (t.m_treeVariant == RV_RSTAR)
	{
		os	<< "Near minimum overlap factor: " << t.m_nearMinimumOverlapFactor << endl
			<< "Reinsert factor: " << t.m_reinsertFactor << endl
			<< "Split distribution factor: " << t.m_splitDistributionFactor << endl
			<< "Tight MBRs: " << t.m_bTightMBRs << endl;
	}

	os	<< "Utilization: " << 100 * t.m_stats.getNumberOfData() / (t.m_stats.getNumberOfNodesInLevel(0) * t.m_leafCapacity) << "%" << endl
		<< t.m_stats;

	#ifndef NDEBUG
	os	<< "Leaf pool hits: " << t.m_leafPool.m_hits << endl
		<< "Leaf pool misses: " << t.m_leafPool.m_misses << endl
		<< "Index pool hits: " << t.m_indexPool.m_hits << endl
		<< "Index pool misses: " << t.m_indexPool.m_misses << endl
		<< "Region pool hits: " << t.m_regionPool.m_hits << endl
		<< "Region pool misses: " << t.m_regionPool.m_misses << endl
		<< "Point pool hits: " << t.m_pointPool.m_hits << endl
		<< "Point pool misses: " << t.m_pointPool.m_misses << endl;
	#endif

	return os;
}
