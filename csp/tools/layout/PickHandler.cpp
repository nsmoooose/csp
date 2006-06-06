// CSPLayout
// Copyright 2003, 2005 Mark Rose <mkrose@users.sourceforge.net>
//
// This file based on osgpick sample from OpenSceneGraph
// Copyright (C) 1998-2003 Robert Osfield
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

#include <csp/tools/layout/PickHandler.h>
#include <csp/tools/layout/LayoutNodes.h>
#include <csp/tools/layout/FeatureGraph.h>
#include <csp/tools/layout/View.h>

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osg/Projection>

#include <osgDB/ReadFile>
#include <osgText/Text>
#include <osgUtil/Optimizer>


PickHandler::PickHandler(View *view): _view(view), _enabled(true), _selecting(false) {
	_viewer = view->getViewer();
}

LayoutNode *PickHandler::walkPath(osg::NodePath const &node_path, LayoutNodePath &feature_path) {
	bool select_next = false;
	for (unsigned i = 0; i < node_path.size(); ++i) {
		LayoutNode *node = LayoutNode::FromOSGNode(node_path[i]);
		if (node) {
			if (select_next) {
				return node;
			}
			feature_path.push_back(node);
			if (node->isGroup() && node->asGroup()->isActive()) {
				select_next = true;
			}
		}
	}
	return 0;
}

bool PickHandler::pick(const osgGA::GUIEventAdapter& ea, int mode) {
	osgUtil::IntersectVisitor::HitList hlist;
	/*
	if (mode == SELECT_ONE) {
		_view->graph()->deselectAll();
	}
	*/
	if (_viewer->computeIntersections(ea.getX(), ea.getY(), hlist)) {
		osgUtil::IntersectVisitor::HitList::iterator hitr;
		for (hitr=hlist.begin(); hitr!=hlist.end(); ++hitr) {
			//osg::Vec3 ip = hitr->getLocalIntersectPoint();
			//osg::Vec3 in = hitr->getLocalIntersectNormal();
			if (hitr->_geode.get()) {
				LayoutNodePath path;
				LayoutNode* node = walkPath(hitr->_nodePath, path);
				if (node != 0) {
					std::cout << "PICK: HIT FEATURE\n";
					if (mode == SELECT_REMOVE) {
						_view->graph()->deselect(node);
					} else if (mode == SELECT_ONE) {
						if (node->isSelected()) continue;
						_view->graph()->deselectAll();
						_view->graph()->select(node);
					} else {
						_view->graph()->select(node);
					}
					return true;
				}
			}
		}
	}
	return false;
}

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter&) {
	if (!_enabled) return false;
	int mod = ea.getModKeyMask();
	int key = ea.getKey();
	switch (ea.getEventType()) {
		case (osgGA::GUIEventAdapter::PUSH):
			if (ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
				_selecting = true;
			} else {
				_selecting = false;
			}
			return false;
		case (osgGA::GUIEventAdapter::DRAG):
			_selecting = false;
			return false;
		case (osgGA::GUIEventAdapter::RELEASE):
			if (ea.getButtonMask() == 0) {
				if (_selecting) {
					int mode = SELECT_ONE;
					if (mod & osgGA::GUIEventAdapter::MODKEY_SHIFT) {
						mode = SELECT_EXTEND;
					} else
					if (mod & osgGA::GUIEventAdapter::MODKEY_CTRL) {
						mode = SELECT_REMOVE;
					}
					/*bool hit = */ pick(ea, mode);
					//return hit;
				}
			}
			_selecting = false;
			return false;
		case (osgGA::GUIEventAdapter::KEYDOWN):
			switch (key) {
				case osgGA::GUIEventAdapter::KEY_Escape:
					_view->graph()->deselectAll();
					return true;
			}
			return false;
		default:
			return false;
	}
}


void PickHandler::enable(bool enable) {
	_enabled = enable;
}

