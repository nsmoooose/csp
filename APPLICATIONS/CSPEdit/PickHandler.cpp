// Copyright 2003 Mark Rose <mrose@stm.lbl.gov>
// All rights reserved.

#include "PickHandler.h"
#include "View.h"

PickHandler::PickHandler(View *view): _view(view) {
	_viewer = view->getViewer();
}

void PickHandler::deselect() {
	_view->getRoot()->clearAllSelections();
}

FeatureNode *PickHandler::walkUp(osg::Node *node) {
	if (node == 0) return 0;
	NodeReference *data = dynamic_cast<NodeReference*>(node->getUserData());
	if (data) return data->getNode();
	if (node->getNumParents() == 0) return 0;
	return walkUp(node->getParent(0));
}

bool PickHandler::pick(const osgGA::GUIEventAdapter& ea, int mode)
{
	osgUtil::IntersectVisitor::HitList hlist;
	if (_viewer->computeIntersections(ea.getX(),ea.getY(),hlist)) {
		osgUtil::IntersectVisitor::HitList::iterator hitr;
		for(hitr=hlist.begin(); hitr!=hlist.end(); ++hitr) {
			//osg::Vec3 ip = hitr->getLocalIntersectPoint();
			//osg::Vec3 in = hitr->getLocalIntersectNormal();
			osg::Geode* geode = hitr->_geode.get();
			if (geode) {
				FeatureNode* node = walkUp(geode);
				if (node != 0) {
					if (mode == SELECT_ONE) {
						deselect();
					}
					if (mode == SELECT_REMOVE) {
						node->setSelected(false);
					} else {
						node->setSelected(true);
					}
					return true;
				}
				break;
			} 
		}
	}
	return false;
}

bool PickHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
{
	int mod = ea.getModKeyMask();
	int key = ea.getKey();
	switch (ea.getEventType()) {
		case (osgGA::GUIEventAdapter::PUSH):
			if (ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
				int mode = SELECT_ONE;
				if (mod & osgGA::GUIEventAdapter::MODKEY_SHIFT) {
					mode = SELECT_EXTEND;
				} else
				if (mod & osgGA::GUIEventAdapter::MODKEY_CTRL) {
					mode = SELECT_REMOVE;
				}
				if (mode != NONE) {
					return pick(ea, mode);
				}
			}
			return false;
		case (osgGA::GUIEventAdapter::KEYDOWN):
			switch (key) {
				case osgGA::GUIEventAdapter::KEY_Escape:
					deselect();
					return true;
			}
			return false;
		default:
			return false;
	}
}


