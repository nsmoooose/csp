// CSPLayout
// Copyright 2003-2005 Mark Rose <mkrose@users.sourceforge.net>
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

#ifndef __CSP_LAYOUT_FEATUREGRAPH_H__
#define __CSP_LAYOUT_FEATUREGRAPH_H__

#include <csp/tools/layout/LayoutNodes.h>
#include <csp/tools/layout/Undo.h>
#include <osg/Referenced>
#include <osg/ref_ptr>

namespace csp {
namespace layout {

/** Callback for receiving feature graph events.
 */
class GraphCallback: public osg::Referenced {
public:
	virtual ~GraphCallback() {}
	virtual void onSelect(LayoutNodePath *path, LayoutNode *node);
	virtual void onDeselect(LayoutNodePath *path, LayoutNode *node);
	virtual void onActivate(LayoutNodePath *path, osg::Matrix const &to_world);
	virtual void onSelectionClear();
	virtual void onRefresh();
};

typedef int CallbackId;


/** Class representing the entire layout node graph.  Manages node selection,
 *  command history (undo/redo), as well as the active group/path/transform.
 */
class FeatureGraph {
	osg::ref_ptr<osg::Group> m_Scene;
	LayoutGroup *m_RootGroup;
	LayoutGroup *m_ActiveGroup;
	LayoutNodePath m_ActivePath;
	typedef std::list< osg::ref_ptr<GraphCallback> > GraphCallbackList;
	GraphCallbackList m_GraphCallbacks;
	CommandHistory m_CommandHistory;

	void signalActivate();
	void signalRefresh();
	void clearAllSelections();
	void deleteBelowRoot();

public:
	FeatureGraph();
	~FeatureGraph();

	/** Deletes the existing graph, if any, and makes the specified group the
	 *  root node of the new graph.  The group instance is owned by the FeatureGraph
	 *  and will be deleted when the feature graph is deleted or replaced.  If
	 *  group is NULL, a default LayoutGroup will be created.
	 */
	void setRoot(LayoutGroup *group);

	LayoutGroup *getRoot() { return m_RootGroup; }
	LayoutGroup *getActiveGroup() { return m_ActiveGroup; }
	LayoutNodePath const &getActivePath() const { return m_ActivePath; }

	/** Get the top-level scene graph node for the feature graph.
	 */
	osg::ref_ptr<osg::Group> getScene() { return m_Scene; }

	/** Set the active group to the root node of the feature graph.
	 */
	void activateRoot();

	/** Set the active group to the parent of the current active group.
	 */
	void activateParent();

	/** Set the active group to be a specific child group of the currently active group.
	 */
	void activateChild(LayoutGroup *group);

	/** Select a node using the given node path from the root of the feature graph.
	 *  The parent of the selected node will become the active group.
	 */
	void deepSelect(LayoutNodePath const &node_path);

	/** Accept a visitor to traverse the feature graph.
	 */
	void accept(LayoutNodeVisitor &visitor);

	/** Get the matrix transforming from the local coordinates of the active group
	 *  to world coordinates.  Note that this transform follows OSG convensions, so
	 *  that a transformed vector is computed as V * M, not M * V.
	 */
	osg::Matrix getActiveGroupToWorldMatrix() const;

	/** Select the specified node, which must be a direct child of the active group.
	 */
	void select(LayoutNode *node);

	/** Deselect a currently selected node.
	 */
	void deselect(LayoutNode *node);

	/** Deselect the current selection.
	 */
	void deselectAll();

	/** Clear the entire feature graph, replacing it with an empty group.
	 */
	void clear() { setRoot(0); }

	/** Test if any commands have been applied to the feature graph.
	 */
	bool isModified() const { return m_CommandHistory.hasUndo(); }

	/** Add a graph callback handler.
	 */
	CallbackId addCallback(GraphCallback *callback);

	/** Remove a graph callback handler, where id is the value returned by addCallback().
	 */
	void removeCallback(CallbackId id);

	// Trampoline methods to access the command history, allowing us to hook
	// in update signals as needed.
	void undo();
	void redo();
	void clearHistory();
	void runCommand(Command *command);
	void addCommand(Command *command);
	std::string getUndoLabel() const { return m_CommandHistory.getUndoLabel(); }
	std::string getRedoLabel() const { return m_CommandHistory.getRedoLabel(); }
	bool hasUndo() const { return m_CommandHistory.hasUndo(); }
	bool hasRedo() const { return m_CommandHistory.hasRedo(); }
};

} // end namespace layout
} // end namespace csp

#endif // __CSP_LAYOUT_FEATUREGRAPH_H__
