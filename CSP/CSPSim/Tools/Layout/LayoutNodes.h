// CSPLayout - Copyright 2003-2005 Mark Rose <mkrose@users.sourceforge.net>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, US

#ifndef __CSP_LAYOUT_LAYOUT_NODES_H__
#define __CSP_LAYOUT_LAYOUT_NODES_H__

#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/Material>
#include <osg/BlendFunc>
#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>

#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <algorithm>

class LayoutNode;
class LayoutModel;
class LayoutGroup;

typedef std::vector<LayoutNode*> LayoutNodePath;

class LayoutNodeVisitor {
	LayoutNodePath m_Path;
public:
	virtual ~LayoutNodeVisitor() {}
	void push(LayoutNode *node);
	void pop();
	LayoutNodePath const &path() { return m_Path; }
	LayoutNode *parent() { return (m_Path.size() > 1) ? m_Path[m_Path.size() - 2] : 0; }
	LayoutNode *current() { return (m_Path.size() > 0) ? m_Path[m_Path.size() - 1] : 0; }
	virtual void apply(LayoutNode *node);
	virtual void apply(LayoutModel *node);
	virtual void apply(LayoutGroup *node);
	inline void traverse(LayoutNode *node);
};


class NodeReference: public osg::Referenced {
public:
	NodeReference(LayoutNode* n): m_Node(n) {}
	LayoutNode *getNode() { return m_Node; }
private:
	LayoutNode *m_Node;
};


class LayoutNode {
	friend class LayoutGroup;

public:

	static osg::Matrix getPathTransform(LayoutNodePath const *path);

	LayoutNode(): _selected(false), _dim(false) {}

	virtual ~LayoutNode() {
		std::cout << "~LayoutNode " << _name << " " << this <<"\n";
	}

	virtual bool isGroup() const = 0;
	virtual LayoutGroup *asGroup() = 0;
	virtual LayoutNode *asNode() { return this; } // helpful for ptr comparisons in swig'd python code

	virtual osg::Node* getGraph() { return _move.get(); }

	virtual std::string const &getName() const { return _name; }
	virtual void setName(std::string const & name) { _name = name; }

	virtual void accept(LayoutNodeVisitor &v)=0;
	virtual void traverse(LayoutNodeVisitor &v)=0;

	unsigned int id() { return reinterpret_cast<unsigned int>(this); }

	virtual void transform(osg::Matrix const &t) {
		osg::Matrix m = _move->getMatrix();
		m.setTrans(m.getTrans() + t.getTrans());
		_move->setMatrix(m);
		osg::Matrix s = _spin->getMatrix();
		s.preMult(t);
		s.setTrans(osg::Vec3());
		_spin->setMatrix(s);
	}

	virtual const osg::BoundingSphere & getBound() const {
		return _move->getBound();
	}

	virtual osg::Matrix getMatrix() const {
		osg::Matrix m = _spin->getMatrix();
		m.preMult(_move->getMatrix());
		return m;
	}

	// convenience methods for python
	virtual float getX() const { return _move->getMatrix().getTrans().x(); }
	virtual float getY() const { return _move->getMatrix().getTrans().y(); }
	virtual float getAngle() const {
		osg::Vec3 x_axis = osg::Vec3(1, 0, 0) * _spin->getMatrix();
		return atan2(x_axis.y(), x_axis.x());
	}

	virtual void setMatrix(osg::Matrix const &m) {
		osg::Vec3 v = m.getTrans();
		osg::Matrix s(m);
		s.setTrans(osg::Vec3());
		_spin->setMatrix(s);
		_move->setMatrix(osg::Matrix::translate(s*v));
	}

	virtual void move(float x, float y) {
		osg::Matrix m = _move->getMatrix();
		m.setTrans(osg::Vec3(x,y,0.0));
		_move->setMatrix(m);
	}

	virtual void moveBy(float x, float y) {
		osg::Matrix m =_move->getMatrix();
		m.setTrans(m.getTrans() + osg::Vec3(x,y,0.0));
		_move->setMatrix(m);
	}

	virtual void rotate(float angle) {
		osg::Matrix m =_spin->getMatrix();
		m.makeRotate(angle, osg::Z_AXIS);
		_spin->setMatrix(m);
	}

	virtual void rotateBy(float angle) {
		osg::Matrix r, m =_spin->getMatrix();
		r.makeRotate(angle, osg::Z_AXIS);
		_spin->setMatrix(m*r);
	}

	virtual void reset() {
		move(0.0f, 0.0f);
		rotate(0.0f);
	}

	bool isSelected() const { return _selected; }
	virtual void setSelected(bool select=true);
	void setDim(bool dim);

	static LayoutNode *FromOSGNode(osg::Node *node) {
		if (!node) return 0;
		NodeReference *data = dynamic_cast<NodeReference*>(node->getUserData());
		return data ? data->getNode() : 0;
	}

protected:

	void rebind() {
		if (!_spin) {
			_spin = new osg::MatrixTransform();
		}
		if (!_move) {
			_move = new osg::MatrixTransform();
			_move->addChild(_spin.get());
		}
		_spin->removeChild(0, _spin->getNumChildren());
		if (_graph.get()) {
			_spin->addChild(_graph.get());
		} else {
			std::cerr << "WARNING: adding null graph to LayoutNode " << _name << "\n";
		}
		_move->setUserData(new NodeReference(this));
	}

	osg::ref_ptr<osg::Node> _graph;
	osg::ref_ptr<osg::MatrixTransform> _move;
	osg::ref_ptr<osg::MatrixTransform> _spin;

	bool _selected;
	bool _dim;
	std::string _name;
};


class LayoutModel: public LayoutNode {
	typedef std::map<std::string, osg::ref_ptr<osg::Node> > ModelCache;
	static ModelCache _cache;
public:
	virtual void load(std::string const &fn,
	                  float scale=1.0, float offset_x=0.0, float offset_y=0.0, float offset_z=0.0,
	                  float axis0_x=0.0, float axis0_y=0.0, float axis0_z=0.0,
	                  float axis1_x=0.0, float axis1_y=0.0, float axis1_z=0.0) {
		ModelCache::iterator iter = _cache.find(fn);
		if (iter == _cache.end()) {
			_graph = osgDB::readNodeFile(fn);
			_cache[fn] = _graph;
		} else {
			_graph = iter->second;
		}
		if (_name.empty()) _name = _graph->getName();
		osg::Vec3 offset(offset_x, offset_y, offset_z);
		osg::Vec3 axis0(axis0_x, axis0_y, axis0_z);
		osg::Vec3 axis1(axis1_x, axis1_y, axis1_z);
		osg::Vec3 axis2 = axis0 ^ axis1;
		if (scale != 1.0 || offset.length() > 0.0 || axis0.y() != 0.0 || axis0.z() != 0.0 || axis1.x() != 0.0 || axis1.z() != 0.0) {
			std::cout << "adding transform node for scale\n";
			osg::MatrixTransform *mt = new osg::MatrixTransform;
			if (scale != 1.0) {
				mt->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
			}
			osg::Matrix align;
			align.set(axis0.x(), axis0.y(), axis0.z(), 0.0,
			          axis1.x(), axis1.y(), axis1.z(), 0.0,
			          axis2.x(), axis2.y(), axis2.z(), 0.0,
			          0.0, 0.0, 0.0, 1.0);
			mt->setMatrix(align * osg::Matrix::scale(scale, scale, scale) * osg::Matrix::translate(offset));
			mt->addChild(_graph.get());
			_graph = mt;
		}
		rebind();
	}
	virtual void accept(LayoutNodeVisitor &v) {
		v.push(this);
		v.apply(this);
		v.pop();
	}
	virtual void traverse(LayoutNodeVisitor &v) {
	}
	virtual bool isGroup() const { return false; }
	virtual LayoutGroup *asGroup() { assert(0); }
	virtual void setModel(osg::ref_ptr<osg::Node> node) {
		_graph = node;
		rebind();
	}
};



class FeatureGroupData : public osg::Referenced {
public:
	typedef osg::ref_ptr<FeatureGroupData> ref_ptr;
	typedef std::list<LayoutNode*> ChildList;
	FeatureGroupData() { } //: active(false) { }
	~FeatureGroupData() { std::cout << "~FeatureGroupData " << this << "\n"; }
	ChildList children;
	osg::ref_ptr<osg::Node> subgraph;
};


class LayoutGroup: public LayoutNode {
	typedef std::map<std::string, FeatureGroupData::ref_ptr> GroupDataCache;
	static GroupDataCache _cache;
	bool _active;

public:
	typedef FeatureGroupData::ChildList ChildList;

	LayoutGroup(): _active(false) {
		_graph = makeGraph();
		_data = new FeatureGroupData;
		_data->subgraph = _graph;
		_child_group = new osg::Group();
		_graph->asGroup()->addChild(_child_group.get());
		rebind();
	}

	LayoutGroup(LayoutGroup const &clone): _active(false) {
		_graph = clone._graph;
		_data = clone._data;
		_child_group = clone._child_group;
		rebind();
	}

	~LayoutGroup() { }

	virtual osg::Group *makeGraph();

	virtual bool isGroup() const { return true; }
	virtual LayoutGroup *asGroup() { return this; }

	FeatureGroupData::ChildList const & children() {
		return _data->children; // TODO copy?
	}

	bool contains(LayoutNode *node) {
		return std::find(_data->children.begin(), _data->children.end(), node) != _data->children.end();
	}

	virtual void traverse(LayoutNodeVisitor &v) {
		FeatureGroupData::ChildList::iterator i = _data->children.begin();
		for (; i != _data->children.end(); i++) {
			(*i)->accept(v);
		}
	}

	virtual void accept(LayoutNodeVisitor &v) {
		v.push(this);
		v.apply(this);
		v.pop();
	}

	virtual void addChild(LayoutNode *node) {
		// TODO visit node subgraph to check for cyclic graphs
		_child_group->addChild(node->getGraph());
		_data->children.push_back(node);
	}

	virtual void removeChild(LayoutNode *node) {
		_child_group->removeChild(node->getGraph());
		_data->children.remove(node);
	}

	virtual void removeAllChildren() {
		_child_group->removeChild(0, _child_group->getNumChildren());
		_data->children.clear();
	}

	osg::Matrix getFullTransform(osg::NodePath const &path) const {
		osg::Matrix matrix;
		osg::NodePath::const_iterator iter = path.begin();
		for(; iter != path.end(); ++iter) {
			LayoutNode *node = LayoutNode::FromOSGNode(*iter);
			if (node) {
				matrix.preMult(node->_move->getMatrix());
				matrix.preMult(node->_spin->getMatrix());
			}
			if (node == this) break;
		}
		assert(iter != path.end()); // not found
		return matrix;
	}

	virtual void setActive(bool active);
	virtual bool isActive() const { return _active; }
	virtual void setSelected(bool select=true);

	int numSelectedChildren() const;
	LayoutNode *getOnlySelectedChild();
	ChildList getSelectedChildren();

private:
	osg::ref_ptr<osg::Group> _child_group;
	void walkActiveChain(LayoutGroup *child, bool active);
	FeatureGroupData::ref_ptr _data;
};


inline void LayoutNodeVisitor::traverse(LayoutNode *node) {
	node->traverse(*this);
}


#endif  // __CSP_LAYOUT_LAYOUT_NODES_H__
