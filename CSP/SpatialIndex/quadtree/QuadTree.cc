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
 * @file Quadtree.cc
 * @brief An integer quadtree spatial index.
 */


#include <QuadTree.h>

namespace SpatialIndex {
namespace IQuadTree {


/** Base class for Branches and Leaves in a QuadTree.
 */
class Node {
	uint32 _level;
	Region _region;
public:
	// temporary for diagnostics
	static uint32 __n_created;
	static uint32 __n_destroyed;
	static uint32 __max_depth;
public:
	Node(uint32 level, Region const &region): _level(level), _region(region) {
		__n_created++; __max_depth = std::max(__max_depth, level);
	}
	virtual ~Node() { __n_destroyed++; }
	virtual bool isLeaf() const=0;
	virtual bool insert(Child *child, uint32 x, uint32 y, Node *& replacement, TreeConstraint &constaint)=0;
	virtual bool remove(Child *child, uint32 x, uint32 y)=0;
	virtual bool update(Child *child, uint32 x, uint32 y, uint32 x_new, uint32 y_new, TreeConstraint &constraint)=0;
	virtual void query(const Region &region, std::vector<Child*> &result) const=0;
	virtual void dump(std::ostream &os) const=0;
	virtual uint32 childCount() const=0;
	inline bool overlaps(const Region &region) const { return _region.overlaps(region); }
	inline bool contains(const Point &point) const { return _region.contains(point); }
	inline uint32 getLevel() const { return _level; }
	inline Region const &getRegion() const { return _region; }
};

// temporary for diagnostics
uint32 Node::__n_created = 0;
uint32 Node::__n_destroyed = 0;
uint32 Node::__max_depth = 0;



/** QuadTree node representing a subdivision of a region into four
 *  quadrants.  Branches to do not directly contain children.
 */
class Branch: public Node {
	uint32 _child_count;
	Node* _subnodes[4];

	static inline uint32 getIndex(uint32 x, uint32 y) {
		uint32 idx = ((x >> 1) & 0x40000000UL) | (y & 0x80000000UL);
		return idx >> 30;
	}

	inline Region getSubregion(uint32 index) {
		uint32 offset = 0x80000000ul >> getLevel();
		uint32 x0 = getRegion().x0();
		uint32 x1 = getRegion().x1();
		uint32 y0 = getRegion().y0();
		uint32 y1 = getRegion().y1();
		switch (index) {
			case 0:
				x1 = x0 + offset;
				y1 = y0 + offset;
				break;
			case 1:
				x0 = x0 + offset;
				y1 = y0 + offset;
				break;
			case 2:
				y0 = y0 + offset;
				x1 = x0 + offset;
				break;
			case 3:
				x0 = x0 + offset;
				y0 = y0 + offset;
				break;
		}
		return Region(x0, y0, x1, y1);
	}

public:
	Branch(uint32 level, const Region &region);
	virtual ~Branch() {
		for (uint32 i = 0; i < 4; ++i) { delete _subnodes[i]; }
	}
	virtual bool isLeaf() const { return false; }
	virtual bool insert(Child *child, uint32 x, uint32 y, Node *&, TreeConstraint &constraint);
	virtual void dump(std::ostream &os) const;
	virtual bool update(Child *child, uint32 x, uint32 y, uint32 x_new, uint32 y_new, TreeConstraint &constraint);
	virtual bool remove(Child *child, uint32 x, uint32 y);
	virtual void query(Region const &region, std::vector<Child*> &result) const;
	virtual uint32 childCount() const { return _child_count; }
};


/** QuadTree node representing a rectangular region containing
 *  child elements.  A leaf has no finer subdivisions.
 */
class Leaf: public Node {
	std::vector<Child*> _children;

public:
	Leaf(uint32 level, Region const &region): Node(level, region) { }

	virtual bool insert(Child *child, uint32 x, uint32 y, Node *&replacement, TreeConstraint &constraint) {
		assert(contains(child.point()));
		//std::cout << "LI: " << x << " " << y << "\n";
		if (_children.size() >= constraint.getLeafLimit() && getLevel() <= constraint.getMaxDepth()) {
			//std::cout << "NEW BRANCH\n";
			Node *b = new Branch(getLevel(), getRegion());
			Node *subreplace;
			for (uint32 i = 0; i < _children.size(); ++i) {
				uint32 rx, ry;
				_children[i]->getLevelCoordinates(rx, ry, getLevel());
				b->insert(_children[i], rx, ry, subreplace, constraint);
			}
			b->insert(child, x, y, subreplace, constraint);
			replacement = b;
			return true;
		} else {
			_children.push_back(child);
			return false;
		}
	}

	virtual bool update(Child *child, uint32, uint32, uint32, uint32, TreeConstraint &) {
		for (uint32 i = 0; i < _children.size(); ++i) {
			if (_children[i]->id() == child->id()) {
				_children[i] = child;
				return true;
			}
		}
		//std::cout << "not found\n";
		return false;
	}

	virtual bool remove(Child *child, uint32, uint32) {
		for (uint32 i = 0; i < _children.size(); ++i) {
			if (_children[i]->id() == child->id()) {
				_children.erase(_children.begin() + i);
				return true;
			}
		}
		//std::cout << "not found\n";
		return false;
	}

	virtual void query(Region const &region, std::vector<Child*> &result) const {
		for (uint32 i = 0; i < _children.size(); ++i) {
			if (region.contains(_children[i]->point())) {
				result.push_back(_children[i]);
			}
		}
	}

	virtual bool isLeaf() const { return true; }

	virtual void dump(std::ostream &os) const {
		os << "LEAF " << getLevel() << " {";
		for (uint32 i = 0; i < _children.size(); ++i) {
			os << " " << _children[i]->id();
		}
		os << " }\n";
	}

	virtual uint32 childCount() const {
		return _children.size();
	}
};


Branch::Branch(uint32 level, Region const &region): Node(level, region), _child_count(0) {
	for (uint32 i = 0; i < 4; ++i) {
		_subnodes[i] = 0;
	}
}

bool Branch::insert(Child *child, uint32 x, uint32 y, Node *&, TreeConstraint &constraint) {
	_child_count++;
	uint32 idx = getIndex(x, y);
	//std::cout << "BI: " << x << " " << y << " : " << idx << "\n";
	Node *replacement;
	Node *subnode = _subnodes[idx];
	if (!subnode) {
		subnode = new Leaf(getLevel() + 1, getSubregion(idx));
		_subnodes[idx] = subnode;
		//std::cout << "NEW LEAF\n";
	}
	bool replace = subnode->insert(child, x<<1, y<<1, replacement, constraint);
	if (replace) {
		delete subnode;
		_subnodes[idx] = replacement;
	}
	return false;
}

bool Branch::update(Child *child, uint32 x, uint32 y, uint32 x_new, uint32 y_new, TreeConstraint &constraint) {
	uint32 idx = getIndex(x, y);
	uint32 idx_new = getIndex(x_new, y_new);
	if (idx == idx_new) {
		Node *subnode = _subnodes[idx];
		if (!subnode) return false;
		return subnode->update(child, x<<1, y<<1, x_new<<1, y_new<<1, constraint);
	}
	if (!remove(child, x, y)) return false;
	Node *nada;
	insert(child, x_new, y_new, nada, constraint);
	return true;
}

bool Branch::remove(Child *child, uint32 x, uint32 y) {
	uint32 idx = getIndex(x, y);
	Node *subnode = _subnodes[idx];
	if (!subnode) {
		//std::cout << "no node\n";
		return false;
	}
	if (!subnode->remove(child, x<<1, y<<1)) return false;
	--_child_count;
	if (subnode->childCount() == 0) {
		delete _subnodes[idx];
		_subnodes[idx] = 0;
	}
	return true;
}

void Branch::query(Region const &region, std::vector<Child*> &result) const {
	//std::cout << "querying branch\n";
	for (uint32 i = 0; i < 4; ++i) {
		Node *subnode = _subnodes[i];
		if (!subnode) continue;
		if (subnode->overlaps(region)) {
			subnode->query(region, result);
		}
	}
}

void Branch::dump(std::ostream &os) const {
	os << "BRANCH " << getLevel() << " {\n";
	for (uint32 i = 0; i < 4; ++i) {
		if (_subnodes[i]) {
			std::cout << i << ": ";
			_subnodes[i]->dump(os);
		}
	}
	os << "}\n";
}


QuadTree::QuadTree(uint32 max_depth, uint32 leaf_limit): _root(0), _constraint(max_depth, leaf_limit) {
}

QuadTree::~QuadTree() {
	delete _root;
}

void QuadTree::insert(Child &child) {
	if (!_root) {
		_root = new Leaf(0, Region(0, 0, 0xfffffffful, 0xfffffffful));
	}
	uint32 rx, ry;
	child.getLevelCoordinates(rx, ry);

	Node *replacement;
	bool replace = _root->insert(&child, rx, ry, replacement, _constraint);
	if (replace) {
		delete _root;
		_root = replacement;
	}
}

bool QuadTree::remove(Child &child) {
	if (!_root) return false;
	uint32 rx, ry;
	child.getLevelCoordinates(rx, ry);
	return _root->remove(&child, rx, ry);
}

/*
 update is somewhat dangerous, since the child object's coordinates
 must be changed.  this can safely be done in one quadtree, but fails
 if the child is stored in multiple quadtrees.  for now, delete and
 reinsert the child.

bool QuadTree::update(Child &child, uint32 new_x, uint32 new_y) {
	if (!_root) return false;
	uint32 old_x = child.x();
	uint32 old_y = child.y();
	child.point() = Point(new_x, new_y);
	bool success = _root->update(&child, old_x, old_y, new_x, new_y, _constraint);
	if (!success) {
		// restore coordinates
		child.point() = Point(old_x, old_y);
	}
	return success;
}
*/

void QuadTree::query(Region const &region, std::vector<Child*> &result) const {
	if (_root != 0) {
		_root->query(region, result);
	}
}

void QuadTree::clear() {
	delete _root;
	_root = 0;
}

uint32 QuadTree::childCount() const {
	return (!_root) ? 0 : _root->childCount();
}

void QuadTree::dump(std::ostream &os) const {
	os << "QUADTREE {\n";
	if (_root != 0) {
		_root->dump(os);
	}
	os << "}\n";
}

std::ostream & operator << (std::ostream &os, QuadTree const &q) {
	q.dump(os);
	return os;
}

std::ostream & operator << (std::ostream &os, Point const &p) {
	return os << '(' << p.x() << ',' << p.y() << ')';
}

std::ostream & operator << (std::ostream &os, Region const &r) {
	return os << '{' << r.x0() << ',' << r.y0() << ',' << r.x1() << ',' << r.y1() << '}';
}

} // namespace IQuadTree
} // namespace SpatialIndex

