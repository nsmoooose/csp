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

#include "FeatureGraph.h"


/** Visitor to deselect nodes.
 */
class ClearSelectionVisitor: public LayoutNodeVisitor {
public:
	virtual void apply(LayoutNode *node);
};

void ClearSelectionVisitor::apply(LayoutNode *node) {
	node->setSelected(false);
	traverse(node);
}


FeatureGraph::FeatureGraph(): m_Scene(new osg::Group), m_RootGroup(new LayoutGroup), m_ActiveGroup(0) {
	m_RootGroup->setName("default_group");
	m_Scene->addChild(m_RootGroup->getGraph());
	activateRoot();
}

FeatureGraph::~FeatureGraph() {
	deleteBelowRoot();
	delete m_RootGroup;
}

void FeatureGraph::setRoot(LayoutGroup *group) {
	deleteBelowRoot();
	m_ActiveGroup = 0;
	delete m_RootGroup;
	if (!group) {
		group = new LayoutGroup();
		group->setName("default_group");
	}
	m_RootGroup = group;
	m_Scene->removeChild(0, 1);
	m_Scene->addChild(m_RootGroup->getGraph());
	activateRoot();
	std::cout << "set root...\n";
	signalRefresh();
}

void FeatureGraph::signalActivate() {
	osg::Matrix active_group_to_world = getActiveGroupToWorldMatrix();
	for (GraphCallbackList::iterator iter = m_GraphCallbacks.begin(); iter != m_GraphCallbacks.end(); ++iter) {
		(*iter)->onActivate(&m_ActivePath, active_group_to_world);
	}
}

void FeatureGraph::clearAllSelections() {
	ClearSelectionVisitor v;
	m_RootGroup->accept(v);
}

void FeatureGraph::activateRoot() {
	if (m_RootGroup->isActive()) {
		assert(m_ActivePath.size() == 1);
		return;
	}
	if (m_ActiveGroup) m_ActiveGroup->setActive(false);
	m_ActiveGroup = m_RootGroup;
	m_ActivePath.clear();
	m_ActivePath.push_back(m_ActiveGroup);
	m_ActiveGroup->setActive(true);
	m_RootGroup->setDim(false);
	clearAllSelections();
	signalActivate();
}

void FeatureGraph::activateParent() {
	if (m_ActivePath.size() <= 1) return;
	LayoutGroup *old = m_ActiveGroup;
	m_ActiveGroup->setActive(false);
	m_ActivePath.pop_back();
	m_ActiveGroup = m_ActivePath.back()->asGroup();
	m_ActiveGroup->setActive(true);
	m_RootGroup->setDim(m_ActiveGroup != m_RootGroup);
	clearAllSelections();
	old->setSelected(true);
	signalActivate();
}

void FeatureGraph::activateChild(LayoutGroup *group) {
	assert(m_ActiveGroup->contains(group));
	assert(group);
	m_ActiveGroup->setActive(false);
	m_ActiveGroup = group;
	m_ActiveGroup->setActive(true);
	m_ActivePath.push_back(m_ActiveGroup);
	m_RootGroup->setDim(m_ActiveGroup != m_RootGroup);
	clearAllSelections();
	signalActivate();
}

void FeatureGraph::deepSelect(LayoutNodePath const &node_path) {
	activateRoot();
	std::cout << "deep select " << node_path.size() << "\n";
	if (node_path.size() > 1) {
		m_ActiveGroup->setActive(false);
		for (unsigned i = 0; i < node_path.size() - 1; ++i) {
			std::cout << "add path " << i << "\n";
			assert(node_path[i]);
			assert(node_path[i]->isGroup());
			m_ActiveGroup = node_path[i]->asGroup();
			m_ActivePath.push_back(m_ActiveGroup);
		}
		std::cout << "set active group\n";
		m_ActiveGroup->setActive(true);
		m_RootGroup->setDim(m_ActiveGroup != m_RootGroup);
		signalActivate();
		std::cout << "active group set\n";
		//m_RootGroup->clearAllSelections();
	}
	if (!node_path.empty()) {
		LayoutNode *selected = node_path[node_path.size() - 1];
		std::cout << "setting selection\n";
		select(selected);
	}
}

osg::Matrix FeatureGraph::getActiveGroupToWorldMatrix() const {
	osg::Matrix m;
	for (unsigned i = 0; i < m_ActivePath.size(); ++i) {
		m.postMult(m_ActivePath[i]->getMatrix());
	}
	return m;
}

void FeatureGraph::select(LayoutNode *node) {
	assert(m_ActiveGroup->contains(node));
	node->setSelected(true);
	for (GraphCallbackList::iterator iter = m_GraphCallbacks.begin(); iter != m_GraphCallbacks.end(); ++iter) {
		(*iter)->onSelect(&m_ActivePath, node);
	}
}

void FeatureGraph::deselect(LayoutNode *node) {
	node->setSelected(false);
	for (GraphCallbackList::iterator iter = m_GraphCallbacks.begin(); iter != m_GraphCallbacks.end(); ++iter) {
		(*iter)->onDeselect(&m_ActivePath, node);
	}
}

void FeatureGraph::deselectAll() {
	std::cout << "deselect all\n";
	clearAllSelections();
	for (GraphCallbackList::iterator iter = m_GraphCallbacks.begin(); iter != m_GraphCallbacks.end(); ++iter) {
		(*iter)->onSelectionClear();
	}
}

class DeleteVisitor: public LayoutNodeVisitor {
	std::set<LayoutNode*> _delete;
	bool _flushed;
public:
	DeleteVisitor() : _flushed(false) { }
	~DeleteVisitor() { flush(); }
	void flush() {
		if (!_flushed) {
			_flushed = 1;
			for (std::set<LayoutNode*>::iterator iter = _delete.begin(); iter != _delete.end(); ++iter) {
				delete *iter;
			}
		}
	}
	virtual void apply(LayoutGroup *group) {
		group->traverse(*this);
		FeatureGroupData::ChildList::const_iterator i = group->children().begin();
		for (; i != group->children().end(); ++i) {
			std::cout << "remove " << ((*i)->getName()) << "\n";
			_delete.insert(*i);
		}
		group->removeAllChildren();
	}
};

void FeatureGraph::signalRefresh() {
	for (GraphCallbackList::iterator iter = m_GraphCallbacks.begin(); iter != m_GraphCallbacks.end(); ++iter) {
		(*iter)->onRefresh();
	}
}

void FeatureGraph::deleteBelowRoot() {
	m_CommandHistory.clear();
	deselectAll();
	activateRoot();
	DeleteVisitor dv;
	m_RootGroup->accept(dv);
	dv.flush();
}

void FeatureGraph::undo() {
	m_CommandHistory.undo();
	std::cout << "undo...\n";
	signalRefresh();
}

void FeatureGraph::redo() {
	m_CommandHistory.redo();
	std::cout << "redo...\n";
	signalRefresh();
}

void FeatureGraph::clearHistory() {
	m_CommandHistory.clear();
}

void FeatureGraph::runCommand(Command *command) {
	m_CommandHistory.runCommand(command);
	std::cout << "run command...\n";
	signalRefresh();
}

void FeatureGraph::addCommand(Command *command) {
	m_CommandHistory.addCommand(command);
}

CallbackId FeatureGraph::addCallback(GraphCallback *callback) {
	assert(callback);
	m_GraphCallbacks.push_back(callback);
	return reinterpret_cast<CallbackId>(callback);
}

void FeatureGraph::removeCallback(CallbackId id) {
	for (GraphCallbackList::iterator iter = m_GraphCallbacks.begin(); iter != m_GraphCallbacks.end(); ++iter) {
		if ((*iter) == reinterpret_cast<GraphCallback*>(id)) {
			m_GraphCallbacks.erase(iter);
			return;
		}
	}
	assert(id == 0);
}

void FeatureGraph::accept(LayoutNodeVisitor &visitor) {
	m_RootGroup->accept(visitor);
}

void GraphCallback::onSelect(LayoutNodePath*, LayoutNode*) { }
void GraphCallback::onDeselect(LayoutNodePath*, LayoutNode*) { }
void GraphCallback::onActivate(LayoutNodePath*, osg::Matrix const &) { }
void GraphCallback::onSelectionClear() { }
void GraphCallback::onRefresh() { }

