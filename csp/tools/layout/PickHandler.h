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

#ifndef __CSP_LAYOUT_PICKHANDLER_H__
#define __CSP_LAYOUT_PICKHANDLER_H__

#include <osgGA/GUIEventHandler>
#include <osgProducer/Viewer>

namespace osgProducer { class Viewer; }
namespace osgGA { class GUIEventAdapter; }

class View;
class LayoutNode;
typedef std::vector<LayoutNode*> LayoutNodePath;


/** An event handler class that select LayoutNodes from the scene based
 *  on mouse clicks.  Uses osg::IntersectionVisitor to find a list of
 *  scene graph elements that are intersected by a ray from the camera
 *  position through the mouse pointer, determines the corresponding
 *  LayoutNodes, and adjusts the active selection accordingly.
 */
class PickHandler : public osgGA::GUIEventHandler {
	// Selection modes.
	enum {
		SELECT_ONE,
		SELECT_EXTEND,
		SELECT_REMOVE,
	};

public:
	PickHandler(View* viewer);

	~PickHandler() {}

	/** Handle a UI event.  Calls pick() For left mouse clicks.  Picking is
	 *  actually done when the mouse button is released, provided that no
	 *  motion has occurred since the button was pressed.  The default action
	 *  is to pick a single item (deselecting any previous selection), but
	 *  the shift and control keys may also be used in conjunction with left
	 *  clicks to add/remove items from the current selection (respectively).
	 */
	bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& us);

	/** Enable or disable object picking.
	 */
	void enable(bool enable);

protected:

	/** Determine which featuren node(s) in the scene to select or deselect based
	 *  on the position of the mouse.  If mode is SELECT_ONE, the deselects any
	 *  currently selected nodes before adding a new node to the selection.  If
	 *  mode is SELECET_EXTEND, picked nodes are added to the selection.  If mode
	 *  is SELECT_REMOVE, picked nodes are removed from the selection.  Returns
	 *  true if a node was found and selected/deselected.  Note that only feature
	 *  nodes in the active group are considered.
	 */
	virtual bool pick(const osgGA::GUIEventAdapter& ea, int mode=SELECT_ONE);

	/** Walks an osg::NodePath to the selected object, returning the first
	 *  LayoutNode along the path that is in the active FeatureGroup.  Builds
	 *  a LayoutNodePath from the root of the FeatureGraph to (but not including)
	 *  the selected LayoutNode.  Hence the LayoutNodePath ends at the active
	 *  FeatureGroup.
	 */
	LayoutNode *walkPath(osg::NodePath const &node_path, LayoutNodePath &feature_path);

	osgProducer::Viewer* _viewer;
	View *_view;
	bool _enabled;
	bool _selecting;
};


#endif // __CSP_LAYOUT_PICKHANDLER_H__
