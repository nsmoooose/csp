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

#ifndef __spatialindex_h
#define __spatialindex_h

#include <assert.h>
#include <iostream>
#include <vector>
#include <map>
#include <limits>
#include <stack>
#include <queue>
#include <set>
#include <cmath>
#include <string>
#include <sstream>

typedef unsigned char byte;

#define interface class

namespace SpatialIndex
{
	//
	// Helper classes
	//
	class Region;
	class Point;

	class Variant
	{
	public:
		Variant();

		int varType;

		union
		{
			long lVal;			// VT_LONG
			byte bVal;			// VT_BYTE
			short iVal;			// VT_SHORT
			float fltVal;			// VT_FLOAT
			double dblVal;		// VT_DOUBLE
			char cVal;			// VT_CHAR
			unsigned short uiVal;	// VT_USHORT
			unsigned long  ulVal;	// VT_ULONG
			int intVal;			// VT_INT
			unsigned int uintVal;	// VT_UINT
			bool blVal;			// VT_BOOL
			char* pcVal;		// VT_PCHAR
			void* pvVal;		// VT_PVOID
		} val;
	}; // Variant

	class PropertySet
	{
	public:
		Variant getProperty(std::string property);
		void setProperty(std::string property, Variant& v);
		void removeProperty(std::string property);

	private:
		std::map<std::string, Variant> m_propertySet;
	}; // PropertySet

	//
	// Enumerations
	//
	enum VariantType
	{
		VT_LONG = 0x0,
		VT_BYTE,
		VT_SHORT,
		VT_FLOAT,
		VT_DOUBLE,
		VT_CHAR,
		VT_USHORT,
		VT_ULONG,
		VT_INT,
		VT_UINT,
		VT_BOOL,
		VT_PCHAR,
		VT_PVOID,
		VT_EMPTY
	};

	enum CommandType
	{
		CT_NODEREAD = 0x0,
		CT_NODEDELETE,
		CT_NODEWRITE
	};

	//
	// Exceptions
	//
	class Exception
	{
	public:
		virtual std::string what() = 0;
		virtual ~Exception() {}
	};

	class IndexOutOfBoundsException : public Exception
	{
	public:
		IndexOutOfBoundsException(int i);
		virtual ~IndexOutOfBoundsException();
		virtual std::string what();

	private:
		std::string m_error;
	}; // IndexOutOfBoundsException

	class IllegalArgumentException : public Exception
	{
	public:
		IllegalArgumentException(std::string s);
		virtual ~IllegalArgumentException();
		virtual std::string what();

	private:
		std::string m_error;
	}; // IllegalArgumentException

	class IllegalStateException : public Exception
	{
	public:
		IllegalStateException(std::string s);
		virtual ~IllegalStateException();
		virtual std::string what();

	private:
		std::string m_error;
	}; // IllegalStateException

	class ResourceLockedException : public Exception
	{
	public:
		ResourceLockedException(std::string s);
		virtual ~ResourceLockedException();
		virtual std::string what();

	private:
		std::string m_error;
	}; // ResourceLockedException

	//
	// Interfaces
	//
	interface IInterval
	{
	public:
		virtual double getLowerBound() const = 0;
		virtual double getUpperBound() const = 0;
		virtual bool intersectsInterval(const IInterval&) const = 0;
		virtual bool intersectsInterval(const double start, const double end) const = 0;
		virtual bool containsInterval(const IInterval&) const = 0;
		virtual ~IInterval() {}
	}; // IInterval

	interface IShape
	{
	public:
		virtual bool intersectsShape(const IShape& in) const = 0;
		virtual bool containsShape(const IShape& in) const = 0;
		virtual bool touchesShape(const IShape& in) const = 0;
		virtual void getCenter(Point& out) const = 0;
		virtual unsigned long getDimension() const = 0;
		virtual void getMBR(Region& out) const = 0;
		virtual double getArea() const = 0;
		virtual double getMinimumDistance(const IShape& in) const = 0;
		virtual ~IShape() {}
	}; // IShape

	interface IEntry
	{
	public:
		virtual long getIdentifier() const = 0;
		virtual void getShape(IShape** out) const throw (std::exception) = 0;
		virtual ~IEntry() {}
	}; // IEntry

	interface INode : public IEntry
	{
	public:
		virtual unsigned long getChildrenCount() const = 0;
		virtual long getChildIdentifier(unsigned long index) const throw (IndexOutOfBoundsException) = 0;
		virtual void getChildShape(unsigned long index, IShape** out) const throw (IndexOutOfBoundsException, std::exception) = 0;
		virtual unsigned long getLevel() const = 0;
		virtual bool isIndex() const = 0;
		virtual bool isLeaf() const = 0;
		virtual ~INode() {}
	}; // INode

	interface IData : public IEntry
	{
	public:
		virtual void getData(unsigned long& len, byte** data) const = 0;
		virtual ~IData() {}
	}; // IData

	interface ICommand
	{
	public:
		virtual void execute(const INode& in) = 0;
		virtual ~ICommand() {}
	}; // ICommand

	interface INearestNeighborComparator
	{
	public:
		virtual double getMinimumDistance(const IShape& query, const IShape& entry) = 0;
		virtual double getMinimumDistance(const IShape& query, const IData& data) = 0;
		virtual ~INearestNeighborComparator() {}
	}; // INearestNeighborComparator

	interface IStorageManager
	{
	public:
		virtual void loadByteArray(const long id, unsigned long& len, byte** data) = 0;
		virtual void storeByteArray(long& id, const unsigned long len, const byte* const data) = 0;
		virtual void deleteByteArray(const long id) = 0;
		virtual ~IStorageManager() {}
	}; // IStorageManager

	interface IVisitor
	{
	public:
		virtual void visitNode(const INode& in) = 0;
		virtual void visitData(const IData& in) = 0;
		virtual ~IVisitor() {}
	}; // IVisitor

	interface IQueryStrategy
	{
	public:
		virtual void getNextEntry(const IEntry& previouslyFetched, long& nextEntryToFetch, bool& bFetchNextEntry) = 0;
		virtual ~IQueryStrategy() {}
	}; // IQueryStrategy

	interface IStatistics
	{
	public:
		virtual unsigned long getReads() const = 0;
		virtual unsigned long getWrites() const = 0;
		virtual unsigned long getNumberOfNodes() const = 0;
		virtual unsigned long getNumberOfData() const = 0;
		virtual ~IStatistics() {}
	}; // IStatistics

	interface ISpatialIndex
	{
	public:
		virtual void insertData(unsigned long len, const byte* pData, const IShape& shape, long shapeIdentifier) throw (Exception, std::exception) = 0;
		virtual bool deleteData(const IShape& shape, long shapeIdentifier) throw (Exception, std::exception) = 0;
		virtual void containsWhatQuery(const IShape& query, IVisitor& v) throw (Exception, std::exception) = 0;
		virtual void intersectsWithQuery(const IShape& query, IVisitor& v) throw (Exception, std::exception) = 0;
		virtual void pointLocationQuery(const Point& query, IVisitor& v) throw (Exception, std::exception) = 0;
		virtual void nearestNeighborQuery(long k, const IShape& query, IVisitor& v, INearestNeighborComparator& nnc) throw (Exception, std::exception) = 0;
		virtual void nearestNeighborQuery(long k, const IShape& query, IVisitor& v) throw (Exception, std::exception) = 0;
		virtual void queryStrategy(IQueryStrategy& qs) throw (Exception, std::exception) = 0;
		virtual void getIndexProperties(PropertySet& out) const = 0;
		virtual void addCommand(ICommand* in, CommandType ct) = 0;
		virtual bool isIndexValid() throw (Exception, std::exception) = 0;
		virtual void getStatistics(IStatistics** out) const throw (std::exception) = 0;
		virtual ~ISpatialIndex() {}
	}; // ISpatialIndex

	namespace StorageManager
	{
		class InvalidPageException : public Exception
		{
		public:
			InvalidPageException(long id);
			virtual ~InvalidPageException();
			virtual std::string what();

		private:
			std::string m_error;
		}; // InvalidPageException

		interface IBuffer : public IStorageManager
		{
		public:
			virtual unsigned long getHits() = 0;
			virtual void clear() = 0;
			virtual ~IBuffer() {}
		}; // IBuffer

		extern IStorageManager* returnMemoryStorageManager(PropertySet& in) throw (std::exception, SpatialIndex::Exception);
		extern IStorageManager* createNewMemoryStorageManager() throw (std::exception, SpatialIndex::Exception);

		extern IStorageManager* returnDiskStorageManager(PropertySet& in) throw (std::exception, SpatialIndex::Exception);
		extern IStorageManager* createNewDiskStorageManager(std::string& baseName, unsigned long pageSize) throw (std::exception, SpatialIndex::Exception);
		extern IStorageManager* loadDiskStorageManager(std::string& baseName) throw (std::exception, SpatialIndex::Exception);

		extern IBuffer* returnRandomEvictionsBuffer(IStorageManager& in, PropertySet& in) throw (std::exception, SpatialIndex::Exception);
		extern IBuffer* createNewRandomEvictionsBuffer(IStorageManager& in, unsigned int capacity, bool bWriteThrough)
			throw (std::exception, SpatialIndex::Exception);
	}

	namespace RTree
	{
		enum RTreeVariant
		{
			RV_LINEAR = 0x0,
			RV_QUADRATIC,
			RV_RSTAR
		};

		extern ISpatialIndex* returnRTree(IStorageManager& in, PropertySet& in) throw (std::exception, SpatialIndex::Exception);
		extern ISpatialIndex* createNewRTree(
			IStorageManager& in, double fillFactor, unsigned long indexCapacity,
			unsigned long leafCapacity, unsigned long dimension, RTreeVariant rv, long& out_indexIdentifier)
			throw (std::exception, SpatialIndex::Exception);
		extern ISpatialIndex* loadRTree(IStorageManager& in, long indexIdentifier) throw (std::exception, SpatialIndex::Exception);
	}

	namespace PPRTree
	{
		enum PPRTreeVariant
		{
			RV_LINEAR = 0x0,
			RV_QUADRATIC,
			RV_RSTAR
		};

		extern ISpatialIndex* returnPPRTree(IStorageManager& in, PropertySet& in) throw (std::exception, SpatialIndex::Exception);
		extern ISpatialIndex* createNewPPRTree(
			IStorageManager& in, double fillFactor, unsigned long indexCapacity,
			unsigned long leafCapacity, unsigned long dimension, PPRTreeVariant rv, long& out_indexIdentifier)
			throw (std::exception, SpatialIndex::Exception);
		extern ISpatialIndex* loadPPRTree(IStorageManager& in, long indexIdentifier) throw (std::exception, SpatialIndex::Exception);
	}

	//
	// Global functions
	//
	extern std::ostream& operator<<(std::ostream&, const ISpatialIndex&);
	extern std::ostream& operator<<(std::ostream&, const IStatistics&);
}

#include <Point.h>
#include <Region.h>
#include <TimePoint.h>
#include <TimeRegion.h>

#endif /*__spatialindex_h*/
