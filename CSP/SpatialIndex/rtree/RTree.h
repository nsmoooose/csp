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

#ifndef __spatialindex_rtree_h
#define __spatialindex_rtree_h

#include "Statistics.h"
#include "Node.h"
#include "PointerPoolNode.h"

namespace SpatialIndex
{
	namespace RTree
	{
		enum PersistenObjectIdentifier
		{
			PersistentIndex = 0x1,
			PersistentLeaf = 0x2
		};

		enum RangeQueryType
		{
			ContainmentQuery = 0x1,
			IntersectionQuery = 0x2
		};

		class RTree : public ISpatialIndex
		{
			class NNEntry;

		public:
			RTree(IStorageManager&, PropertySet&);
				// String                   Value     Description
				// ----------------------------------------------
				// IndexIndentifier			VT_LONG	If specified an existing index will be openened from the supplied
				//							storage manager with the given index id. Behaviour is unspecified
				//							if the index id or the storage manager are incorrect.
				// Dimension				VT_ULONG	Dimensionality of the data that will be inserted.
				// IndexCapacity			VT_ULONG	The index node capacity. Default is 100.
				// LeafCapactiy			VT_ULONG	The leaf node capacity. Default is 100.
				// FillFactor				VT_DOUBLE	The fill factor. Default is 70%
				// TreeVariant			VT_LONG	Can be one of Linear, Quadratic or Rstar. Default is Rstar
				// NearMinimumOverlapFactor	VT_ULONG	Default is 32.
				// SplitDistributionFactor		VT_DOUBLE	Default is 0.4
				// ReinsertFactor			VT_DOUBLE	Default is 0.3
				// EnsureTightMBRs			VT_BOOL	Default is true
				// IndexPoolCapacity			VT_LONG	Default is 100
				// LeafPoolCapacity			VT_LONG	Default is 100
				// RegionPoolCapacity		VT_LONG	Default is 1000
				// PointPoolCapacity			VT_LONG	Default is 500

			virtual ~RTree();

			//
			// ISpatialIndex interface
			//
			virtual void insertData(unsigned long len, const byte* pData, const IShape& shape, long id) throw (Exception, std::exception);
			virtual bool deleteData(const IShape& shape, long id) throw (Exception, std::exception);
			virtual void containsWhatQuery(const IShape& query, IVisitor& v) throw (Exception, std::exception);
			virtual void intersectsWithQuery(const IShape& query, IVisitor& v) throw (Exception, std::exception);
			virtual void pointLocationQuery(const Point& query, IVisitor& v) throw (Exception, std::exception);
			virtual void nearestNeighborQuery(long k, const IShape& query, IVisitor& v, INearestNeighborComparator&) throw (Exception, std::exception);
			virtual void nearestNeighborQuery(long k, const IShape& query, IVisitor& v) throw (Exception, std::exception);
			virtual void queryStrategy(IQueryStrategy& qs) throw (Exception, std::exception);
			virtual void getIndexProperties(PropertySet& out) const;
			virtual void addCommand(ICommand* pCommand, CommandType ct);
			virtual bool isIndexValid() throw (Exception, std::exception);
			virtual void getStatistics(IStatistics** out) const throw (std::exception);

		private:
			void initNew(PropertySet&);
			void initOld(PropertySet& ps);
			void storeHeader();
			void loadHeader();

			void insertData_impl(unsigned long dataLength, byte* pData, Region& mbr, long id);
			void insertData_impl(unsigned long dataLength, byte* pData, Region& mbr, long id, unsigned long level, byte* overflowTable);
			bool deleteData_impl(const Region& mbr, long id);

			long writeNode(Node*);
			NodePtr readNode(unsigned long);
			void deleteNode(Node*);

			void rangeQuery(RangeQueryType type, const IShape& query, IVisitor& v) throw (Exception, std::exception);

			IStorageManager* m_pStorageManager;

			long m_rootID;
			long m_headerID;

			RTreeVariant m_treeVariant;

			double m_fillFactor;

			unsigned long m_indexCapacity;

			unsigned long m_leafCapacity;

			unsigned long m_nearMinimumOverlapFactor;
				// The R*-Tree 'p' constant, for calculating nearly minimum overlap cost.
				// [Beckmann, Kriegel, Schneider, Seeger 'The R*-tree: An efficient and Robust Access Method
				// for Points and Rectangles', Section 4.1]

			double m_splitDistributionFactor;
				// The R*-Tree 'm' constant, for calculating spliting distributions.
				// [Beckmann, Kriegel, Schneider, Seeger 'The R*-tree: An efficient and Robust Access Method
				// for Points and Rectangles', Section 4.2]

			double m_reinsertFactor;
				// The R*-Tree 'p' constant, for removing entries at reinserts.
				// [Beckmann, Kriegel, Schneider, Seeger 'The R*-tree: An efficient and Robust Access Method
				//  for Points and Rectangles', Section 4.3]

			unsigned long m_dimension;

			Region m_infiniteRegion;

			Statistics m_stats;

			bool m_bTightMBRs;

			PointerPool<Point> m_pointPool;
			PointerPool<Region> m_regionPool;
			PointerPool<Node> m_indexPool;
			PointerPool<Node> m_leafPool;

			std::vector<SmartPointer<ICommand> > m_writeNodeCommands;
			std::vector<SmartPointer<ICommand> > m_readNodeCommands;
			std::vector<SmartPointer<ICommand> > m_deleteNodeCommands;

#ifdef PTHREADS
			pthread_rwlock_t m_rwLock;
#else
			bool m_rwLock;
#endif

			class NNEntry
			{
			public:
				long m_id;
				IEntry* m_pEntry;
				double m_minDist;

				NNEntry(long id, IEntry* e, double f) : m_id(id), m_pEntry(e), m_minDist(f) {}
				~NNEntry() {}

				struct greater : public std::binary_function<NNEntry*, NNEntry*, bool>
				{
					bool operator()(const NNEntry* __x, const NNEntry* __y) const { return __x->m_minDist > __y->m_minDist; }
				};
			}; // NNEntry

			class NNComparator : public INearestNeighborComparator
			{
			public:
				double getMinimumDistance(const IShape& query, const IShape& entry)
				{
					return query.getMinimumDistance(entry);
				}

				double getMinimumDistance(const IShape& query, const IData& data)
				{
					IShape* pS;
					data.getShape(&pS);
					double ret = query.getMinimumDistance(*pS);
					delete pS;
					return ret;
				}
			}; // NNComparator

			class ValidateEntry
			{
			public:
				Region m_parentMBR;
				NodePtr m_pNode;

				ValidateEntry(Region& r, NodePtr& pNode) : m_parentMBR(r), m_pNode(pNode) {}
			}; // ValidateEntry

			class SharedLock
			{
			public:
#ifdef PTHREADS
				SharedLock(pthread_rwlock_t* pLock) : m_pLock(pLock) { pthread_rwlock_rdlock(m_pLock); }
				~SharedLock() { pthread_rwlock_unlock(m_pLock); }

			private:
				pthread_rwlock_t* m_pLock;
#endif
			}; // SharedLock

			class ExclusiveLock
			{
			public:
#ifdef PTHREADS
				ExclusiveLock(pthread_rwlock_t* pLock) : m_pLock(pLock) { pthread_rwlock_wrlock(m_pLock); }
				~ExclusiveLock() { pthread_rwlock_unlock(m_pLock); }

			private:
				pthread_rwlock_t* m_pLock;
#endif
			}; // ExclusiveLock

			class Data : public IData
			{
			public:
				Data(unsigned long len, byte* pData, RegionPtr& mbr, long id)	:  m_id(id), m_shape(mbr), m_length(len), m_pData(pData)
				{
					// we can avoid the data copy here, even in case of multiple threads, since a Visitor should
					// be holding a read lock.
				}

				virtual ~Data() {}

				long getIdentifier() const { return m_id; }

				virtual void getShape(IShape** out) const throw (std::exception) { *out = new Region(*m_shape); }

				void getData(unsigned long& len, byte** data) const
				{
					len = m_length;

					if (m_length > 0)
					{
						*data = new byte[len];
						memcpy(*data, m_pData, len);
					}
				}

			protected:
				long m_id;
				RegionPtr m_shape;
				unsigned long m_length;
				byte* m_pData;

				friend class RTree;
			}; // Data

			friend class Node;
			friend class Leaf;
			friend class Index;

			friend std::ostream& operator<<(std::ostream& os, const RTree& t);
		}; // RTree

		std::ostream& operator<<(std::ostream& os, const RTree& t);
	}
}

#endif /*__spatialindex_rtree_h*/

