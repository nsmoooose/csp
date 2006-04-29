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


#include "LayoutNodes.h"
#include <iostream>

#include <osg/Switch>
#include <osg/Geometry>
#include <osg/Light>
#include <osg/LightModel>


osg::Matrix LayoutNode::getPathTransform(LayoutNodePath const *path) {
	osg::Matrix matrix;
	LayoutNodePath::const_iterator iter = path->begin();
	for(; iter != path->end(); ++iter) {
		LayoutNode *node = *iter;
		if (node) {
			matrix.preMult(node->_move->getMatrix());
			matrix.preMult(node->_spin->getMatrix());
		}
	}
	return matrix;
}

void LayoutNodeVisitor::apply(LayoutNode *node) {}

void LayoutNodeVisitor::apply(LayoutModel *node) {
	apply(static_cast<LayoutNode*>(node)); 
}

void LayoutNodeVisitor::apply(LayoutGroup *node) { 
	apply(static_cast<LayoutNode*>(node)); 
}

void LayoutNodeVisitor::push(LayoutNode *node) {
	m_Path.push_back(node);
}

void LayoutNodeVisitor::pop() {
	m_Path.pop_back();
}

void LayoutGroup::setActive(bool active) {
	if (active != _active) {
		// TODO setdim
		_active = active;
		if (active) {
			_move->getOrCreateStateSet()->setAttributeAndModes(new osg::Material(), osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
			_move->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
		} else {
			_move->getOrCreateStateSet()->setAttributeAndModes(new osg::Material(), osg::StateAttribute::OFF);
			_move->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::OFF);
		}
	}
}

int LayoutGroup::numSelectedChildren() const {
	int count = 0;
	FeatureGroupData::ChildList::const_iterator i = _data->children.begin();
	for (; i != _data->children.end(); i++) {
		if ((*i)->isSelected()) count++;
	}
	return count;
}

LayoutNode *LayoutGroup::getOnlySelectedChild() {
	LayoutNode *node = 0;
	FeatureGroupData::ChildList::iterator i = _data->children.begin();
	for (; i != _data->children.end(); i++) {
		if ((*i)->isSelected()) {
			if (node) return 0;
			node = *i;
		}
	}
	return node;
}

void LayoutNode::setSelected(bool select) {
	if (_selected == select) return;
	_selected = select;
	if (select) {
		osg::Material *mat = new osg::Material();
		mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1,0,1,1));
		mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1,0,1,1));
		//mat->setTransparency(osg::Material::FRONT_AND_BACK, 0.7);
		_spin->getOrCreateStateSet()->setAttributeAndModes(mat, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
		//_spin->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	} else {
		_spin->getOrCreateStateSet()->setAttributeAndModes(new osg::Material(), osg::StateAttribute::OFF);
		_spin->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::OFF);
	}
}

void LayoutNode::setDim(bool dim) {
	if (dim == _dim) return;
	if (dim) {
		osg::Material *mat = new osg::Material();
		mat->setTransparency(osg::Material::FRONT_AND_BACK, 0.9);
		_move->getOrCreateStateSet()->setAttributeAndModes(mat, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
		_move->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	} else {
		_move->getOrCreateStateSet()->setAttributeAndModes(new osg::Material(), osg::StateAttribute::OFF);
		_move->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::OFF);
	}
	_dim = dim;
}

LayoutModel::ModelCache LayoutModel::_cache;

LayoutGroup::ChildList LayoutGroup::getSelectedChildren() {
	ChildList list;
	FeatureGroupData::ChildList::iterator i = _data->children.begin();
	for (; i != _data->children.end(); i++) {
		if ((*i)->isSelected()) {
			list.push_back(*i);
		}
	}
	return list;
}


osg::Geometry *makeDiamond(osg::Vec3 const &pos, float s) {
	float vv[][3] =
	{
		{ 0.0, 0.0,   s },
		{   s, 0.0, 0.0 },
		{ 0.0,   s, 0.0 },
		{  -s, 0.0, 0.0 },
		{ 0.0,  -s, 0.0 },
		{   s, 0.0, 0.0 },
		{ 0.0, 0.0,  -s },
		{   s, 0.0, 0.0 },
		{ 0.0,  -s, 0.0 },
		{  -s, 0.0, 0.0 },
		{ 0.0,   s, 0.0 },
		{   s, 0.0, 0.0 },
	};

	osg::Vec3Array& v = *(new osg::Vec3Array(12));
	osg::Vec4Array& l = *(new osg::Vec4Array(1));

	int i;

	l[0][0] = 1;
	l[0][1] = 1;
	l[0][2] = 0.5;
	l[0][3] = 1;

	for( i = 0; i < 12; i++ )
	{
		v[i][0] = vv[i][0] + pos.x();
		v[i][1] = vv[i][1] + pos.y();
		v[i][2] = vv[i][2] + pos.z();
	}

	osg::Geometry *geom = new osg::Geometry;
	geom->setVertexArray(&v);
	geom->setColorArray(&l);
	geom->setColorBinding(osg::Geometry::BIND_OVERALL);
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN,0,6));
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN,6,6));

	return geom;
}

osg::Group *LayoutGroup::makeGraph() {
	osg::Switch *group = new osg::Switch();
	osg::Geode *axis = new osg::Geode();
	axis->addDrawable(makeDiamond(osg::Vec3(0,0,1), 1.0));
	axis->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);
	group->addChild(axis, false);
	return group;
}

void LayoutGroup::setSelected(bool select) {
	LayoutNode::setSelected(select);
	osg::Switch *group = dynamic_cast<osg::Switch*>(_graph.get());
	// hide the center marker unless we are selected
	if (group) {
		group->setValue(0, isSelected());
	}
}

