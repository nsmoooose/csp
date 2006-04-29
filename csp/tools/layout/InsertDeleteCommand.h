// CSPLayout
// Copyright 2003, 2005 Mark Rose <mkrose@users.sourceforge.net>
//
// Based in part on osgpick sample code
// OpenSceneGraph - Copyright (C) 1998-2003 Robert Osfield
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

#ifndef __CSP_LAYOUT_INSERTDELETECOMMAND_H__
#define __CSP_LAYOUT_INSERTDELETECOMMAND_H__

#include "LayoutNodes.h"
#include "Undo.h"

#include <string>
#include <cassert>


/** A command subclass for tracking added and deleted layout nodes.
 *  Allows insert and delete events to be undone/redone.
 */
class InsertDeleteCommand: public Command {
	LayoutGroup *_parent;
	// FIXME leaks nodes that aren't in the graph
	LayoutGroup::ChildList _nodes;
	bool _insert;
	std::string _label;
public:
	InsertDeleteCommand(LayoutGroup *parent, LayoutNode *node, bool insert, std::string const &label): _parent(parent), _insert(insert), _label(label) {
		assert(node);
		assert(parent);
		_nodes.push_back(node);
	}
	InsertDeleteCommand(LayoutGroup *parent, LayoutGroup::ChildList const &nodes, bool insert, std::string const &label): _parent(parent), _nodes(nodes), _insert(insert), _label(label) {
		assert(!nodes.empty());
		assert(parent);
	}
	virtual void undo() {
		for (LayoutGroup::ChildList::iterator iter = _nodes.begin(); iter != _nodes.end(); ++iter) {
			LayoutNode *node = *iter;
			if (_insert) {
				_parent->removeChild(node);
			} else {
				_parent->addChild(node);
			}
		}
	}
	virtual void redo() {
		for (LayoutGroup::ChildList::iterator iter = _nodes.begin(); iter != _nodes.end(); ++iter) {
			LayoutNode *node = *iter;
			if (_insert) {
				_parent->addChild(node);
			} else {
				_parent->removeChild(node);
			}
		}
	};
	virtual std::string label() const { return _label; }
};

#endif //  __CSP_LAYOUT_INSERTDELETECOMMAND_H__

