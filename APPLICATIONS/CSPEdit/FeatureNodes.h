// CSPEdit - Copyright (C) 2003 Mark Rose
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

#ifndef __FEATURE_NODES_H__
#define __FEATURE_NODES_H__

#include <osg/MatrixTransform>
#include <osg/Material>
#include <osg/BlendFunc>
#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>

#include <list>

class FeatureNode;
class FeatureModel;
class FeatureGroup;

class FeatureNodeVisitor {
public:
	virtual void apply(FeatureNode *node);
	virtual void apply(FeatureModel *node);
	virtual void apply(FeatureGroup *node);
};

class ClearSelectionVisitor: public FeatureNodeVisitor {
public:
	virtual void apply(FeatureNode *node);
};

class FeatureNode {
public:
	FeatureNode(): _parent(0), _selected(false) {}
	virtual ~FeatureNode() {
		reparent(0);
	}
	inline void reparent(FeatureGroup *parent, bool remove=true);
	virtual osg::Node* getGraph() { return _move.get(); }
	virtual std::string const &getName() const {
		return _graph->getName();
	}
	virtual void accept(FeatureNodeVisitor &v)=0;
	virtual void setName(std::string const & name) {
		_graph->setName(name);
	}
	virtual void move(float x, float y) {
		osg::Matrix m =_move->getMatrix();
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
	inline void clearAllSelections();
	bool isSelected() const { return _selected; }
	void setSelected(bool select=true) { 
		if (_selected == select) return;
		_selected = select; 
		if (select) {
			osg::Material *mat = new osg::Material();
			mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1,1,0,1));
			mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1,1,0,1));
			mat->setTransparency(osg::Material::FRONT_AND_BACK, 0.8);
			_spin->getOrCreateStateSet()->setAttributeAndModes(mat, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
			_spin->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
		} else {
			_spin->getOrCreateStateSet()->setAttributeAndModes(new osg::Material(), osg::StateAttribute::OFF);
			_spin->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::OFF);
		}
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
		_spin->addChild(_graph.get());
	}
	osg::ref_ptr<osg::Node> _graph;
	osg::ref_ptr<osg::MatrixTransform> _move;
	osg::ref_ptr<osg::MatrixTransform> _spin;
	FeatureGroup *_parent;
	bool _selected;
};


class NodeReference: public osg::Referenced {
public:
	NodeReference(FeatureNode* n): m_Node(n) {}
	FeatureNode *getNode() { return m_Node; }
private:
	FeatureNode *m_Node;
};


class FeatureModel: public FeatureNode {
public:
	virtual bool load(std::string const &fn) {
		_graph = osgDB::readNodeFile(fn);
		if (!_graph) return false;
		_graph->setUserData(new NodeReference(this));
		rebind();
		return true;
	}
	virtual void accept(FeatureNodeVisitor &v) {
		v.apply(this);
	}
};


class FeatureGroup: public FeatureNode {
public:
	FeatureGroup() {
		_graph = new osg::Group();
		rebind();
	}
	~FeatureGroup() {
		reparent(0);
		std::list<FeatureNode*>::iterator i = _children.begin();
		for (; i != _children.end(); i++) {
			(*i)->reparent(0, false);
		}
	}
	void traverse(FeatureNodeVisitor &v) {
		std::list<FeatureNode*>::iterator i = _children.begin();
		for (; i != _children.end(); i++) {
			(*i)->accept(v);
		}
	}
	virtual void accept(FeatureNodeVisitor &v) {
		v.apply(this);
		traverse(v);
	}
	virtual void addChild(FeatureNode *node) {
		_graph->asGroup()->addChild(node->getGraph());
		node->reparent(this);
		_children.push_back(node);
	}
	virtual void removeChild(FeatureNode *node) {
		_graph->asGroup()->removeChild(node->getGraph());
		node->reparent(0, false);
		_children.remove(node);
	}
private:
	std::list<FeatureNode*> _children;
};

inline void FeatureNode::reparent(FeatureGroup *parent, bool remove) {
	FeatureGroup *old = _parent;
	_parent = parent;
	if (remove && old) old->removeChild(this);
}

inline void FeatureNode::clearAllSelections() {
	FeatureNode *root = _parent;
	if (root == 0) root = this;
	while (root->_parent != 0) {
		root = root->_parent;
	}
	ClearSelectionVisitor visitor;
	root->accept(visitor);
}

#endif

