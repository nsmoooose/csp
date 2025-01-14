#pragma once
/* Integer QuadTree Spatial Index
 * Copyright 2004 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */


/**
 * @file QuadTree.h
 * @brief An integer quadtree spatial index.
 *
 * Implements a simple 2D spatial index for points using 32-bit integer
 * coordinates.  Elements in the index consist of a unique id number
 * and (x, y) coordinates in the range [-2**31, +2**31].  Inserting,
 * deleting, and updating points are fast operations compared to more
 * elaborate spatial index structures.  For example, these operations
 * on an index of 10000 random points require about 500 ns on a 2.5 GHz
 * machine.
 *
 * The index supports simple queries over rectangular regions.  The
 * time for such queries depends largely on the number of elements
 * returned (linear).  For queries returning on the order of 100 points,
 * times in the 1 us range are typical on the hardware mentioned above.
 *
 * The index is not thread safe, so appropriate locks must be maintained
 * externally when multiple threads need access to the structure.
 *
 */

#include <csp/csplib/util/Export.h>
#include <csp/csplib/util/Uniform.h>

#include <vector>
#include <cassert>
#include <iosfwd>


namespace csp {

/** Spatial representation and indexing. */
namespace spatial {

typedef uint32_t Coordinate;

/** A simple 2D point class, using usigned 32-bit integer coordinates.
 */
class CSPLIB_EXPORT Point {
	uint32_t _x, _y;
public:

	/** Construct a point using unsigned coordinates.
	 */
	Point(uint32_t x, uint32_t y): _x(x), _y(y) { }

	/** Copy ctor.
	 */
	Point(Point const &point): _x(point._x), _y(point._y) { }

	/** Copy by assignment.
	 */
	Point const &operator=(Point const &point) {
		_x = point._x;
		_y = point._y;
		return *this;
	}

	// coordinate accessors (unsigned).
	inline uint32_t x() const { return _x; }
	inline uint32_t y() const { return _y; }

	/** Helper method for converting to signed coordinates.
	 */
	void toSigned(int32_t &x, int32_t &y) {
		x = static_cast<int32_t>(_x ^ 0x80000000ul);
		y = static_cast<int32_t>(_y ^ 0x80000000ul);
	}

	/** Helper function for creating a point from signed coordinates.
	 */
	static inline Point fromSigned(int32_t x, int32_t y) {
		uint32_t ux = static_cast<uint32_t>(x ^ 0x80000000UL);
		uint32_t uy = static_cast<uint32_t>(y ^ 0x80000000UL);
		return Point(ux, uy);
	}
};


/** A simple 2D rectangular region class, using unsigned 32-bit integer
 *  coordinates.
 */
class CSPLIB_EXPORT Region {
	uint32_t _x0, _y0, _x1, _y1;
public:

	/** Construct a region from the lower left and upper right corners.
	 *
	 *  It is the caller's responsibility to ensure that p0.x <= p1.x
	 *  and p0.y <= p1.y.
	 *
	 *  @param p0 the lower left coordinate.
	 *  @param p1 the upper right coordinate.
	 */
	Region(Point const &p0, Point const &p1):
		_x0(p0.x()), _y0(p0.y()), _x1(p1.x()), _y1(p1.y()) { }

	/** Construct a region from the edge coordinates.
	 *
	 *  It is the caller's responsibility to ensure that x0 <= x1
	 *  and y0 <= y1.
	 *
	 *  @param x0 the left edge coordinate.
	 *  @param y0 the bottom edge coordinate.
	 *  @param x1 the rightt edge coordinate.
	 *  @param y1 the top edge coordinate.
	 */
	Region(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1):
		_x0(x0), _y0(y0), _x1(x1), _y1(y1) { }

	/** Copy ctor.
	 */
	Region(const Region &region):
		_x0(region._x0), _y0(region._y0), _x1(region._x1), _y1(region._y1) { }

	/** Copy by assignment.
	 */
	Region const &operator=(Region &region) {
		_x0 = region._x0;
		_y0 = region._y0;
		_x1 = region._x1;
		_y1 = region._y1;
		return *this;
	}

	// coordinate accessors.
	inline uint32_t x0() const { return _x0; }
	inline uint32_t y0() const { return _y0; }
	inline uint32_t x1() const { return _x1; }
	inline uint32_t y1() const { return _y1; }

	/** Test if a region overlaps this region.
	 *
	 *  @returns true if overlap.  Edge equality is treated as overlapping.
	 */
	inline bool overlaps(const Region &region) const {
		return (region._x0 <= _x1) && (region._y0 <= _y1) && (region._x1 >= _x0) && (region._y1 >= _y0);
	}

	/** Test if a point lies within this region.
	 *
	 *  @returns true if the point is inside or on the border of this region.
	 */
	inline bool contains(const Point &point) const {
		uint32_t x = point.x();
		uint32_t y = point.y();
		return (x <= _x1) && (y <= _y1) && (x >= _x0) && (y >= _y0);
	}

	/** Expend this region to include the specified point.
	 */
	void expand(const Point &point) {
		if (_x0 > point.x()) {
			_x0 = point.x();
		} else if (_x1 < point.x()) {
			_x1 = point.x();
		}
		if (_y0 > point.y()) {
			_y0 = point.y();
		} else if (_y1 < point.y()) {
			_y1 = point.y();
		}
	}

	/** Expand this region to include the specified region.
	 */
	void expand(const Region &region) {
		if (_x0 > region.x0()) {
			_x0 = region.x0();
		}
		if (_x1 < region.x1()) {
			_x1 = region.x1();
		}
		if (_y0 > region.y0()) {
			_y0 = region.y0();
		}
		if (_y1 < region.y1()) {
			_y1 = region.y1();
		}
	}

	/** Helper function for constructing a region from signed coordinates.
	 *  The coordinates are translated such that (-2**31, -2**31) becomes
	 *  (0, 0).
	 */
	static inline Region fromSigned(int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
		return Region(Point::fromSigned(x0, y0), Point::fromSigned(x1, y1));
	}
};


/** Element class stored in the quadtree, consisting of a unique id number
 *  and a Point.  Child is meant to be subclassed.  Subclass instances may
 *  be stored in the quadtree as base class pointers.  If only one type of
 *  subclass is stored in the quadtree (recommended), then query results
 *  can be static_cast back to the subclass.  Child subclasses should only
 *  be deleted via a subclass pointer, since Child does not define a virtual
 *  destructor.
 */
class CSPLIB_EXPORT Child {
	int _id;
	Point _point;
public:

	/** Construct a new child, given an id number and <i>unsigned</i> 32-bit
	 *  coordinates.
	 */
	Child(int id, uint32_t x, uint32_t y):
		_id(id),
		_point(x, y) {
	}

	/** Copy ctor.
	 */
	Child(const Child &child): _id(child._id), _point(child._point) { }

	/** Copy by assignment.
	 */
	Child const &operator=(const Child &child) {
		_id = child._id;
		_point = child._point;
		return *this;
	}

	// accessors
	inline int id() const { return _id; }
	inline Point const &point() const { return _point; }

	/** Returns a non-const reference to the coordinates of the child.
	 *  <b>Use this method with extreme care!</b>  The coordinates cannot
	 *  be modified without updating all quadtrees that contain this child.
	 *  The safest approach is to remove the child from all quadtrees,
	 *  update coordinates, and reinsert the child.
	 */
	inline Point &mutablePoint() { return _point; }

	inline uint32_t x() const { return _point.x(); }
	inline uint32_t y() const { return _point.y(); }

	/** Get the coordinates adjusted for a given quadtree level.
	 */
	void getLevelCoordinates(uint32_t &rx, uint32_t &ry, uint32_t level=0) {
		if (level > 0) {
			rx = _point.x() << level;
			ry = _point.y() << level;
		} else {
			rx = _point.x();
			ry = _point.y();
		}
	}
};


/** Helper class for passing tree configuration data down to subnodes
 *  when traversing the tree.
 */
class CSPLIB_EXPORT TreeConstraint {
	uint32_t _max_depth;
	uint32_t _leaf_limit;

public:

	/** Construct a new TreeConstraint.
	 *
	 *  @param max_depth The maximum depth of the tree.  Leaves at this level will
	 *    accumulate children without branching (32 is an upper bound given the
	 *    coordinate resolution).
	 *  @param leaf_limit The maximum number of children per leaf.  Additional
	 *    children will cause the leaf to branch.
	 */
	TreeConstraint(uint32_t max_depth, uint32_t leaf_limit)
		: _max_depth(max_depth), _leaf_limit(leaf_limit) {
		assert(max_depth <= 32);
	}

	/** Get the maximum tree depth.
	 */
	inline uint32_t getMaxDepth() const { return _max_depth; }

	/** Get the maximum number of children per leaf.
	 */
	inline uint32_t getLeafLimit() const { return _leaf_limit; }

};


class Node;


/** A 2D spatial index over 32-bit integer coordinates.
 */
class CSPLIB_EXPORT QuadTree {
	Node *_root;
	TreeConstraint _constraint;

public:
	QuadTree(uint32_t max_depth, uint32_t leaf_limit);
	~QuadTree();

	/** Insert a new element.
	 *
	 *  @param child A Child instance to insert.  The object pointer must remain
	 *    valid for as long as the child is stored in the index.
	 */
	void insert(Child &child);

	/** Remove an element.
	 *
	 *  @param child A Child instance to remove.
	 *  @return true if the element was found and removed.
	 */
	bool remove(Child &child);

#if 0
	/** Update the position of an element.
	 *  This is slightly more efficient than removing and reinserting the element.
	 *
	 *  Note: this method is problematic with regard to child coordinate updates
	 *  for children stored in multiple quadtrees.  It has been disabled for now;
	 *  use remove + insert to update coordinates.
	 *
	 *  @param child A Child instance to update.
	 *  @param new_x The new x coordinate of the element in the range [0, 2**32).
	 *  @param new_y The new y coordinate of the element in the range [0, 2**32).
	 *  @return true if the element was found and updated.
	 */
	bool update(Child &child, uint32_t old_x, uint32_t old_y);
#endif

	/** Query the index to find all elements within a region.
	 *
	 *  @param region The region to search.
	 *  @param result A vector to accumulate elements found within the region.
	 */
	void query(Region const &region, std::vector<Child*> &result) const;

	/** Remove all children from the index.
	 */
	void clear();

	/** Get the number of child elements stored in the quadtree.
	 */
	uint32_t childCount() const;

	/** Dump the quadtree structure to an output stream (for diagnostics).
	 */
	void dump(std::ostream &os) const;
};

extern CSPLIB_EXPORT std::ostream & operator << (std::ostream &os, QuadTree const &q);
extern CSPLIB_EXPORT std::ostream & operator << (std::ostream &os, Point const &p);
extern CSPLIB_EXPORT std::ostream & operator << (std::ostream &os, Region const &r);


} // namespace spatial
} // namespace csp
