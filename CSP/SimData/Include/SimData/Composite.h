/* SimData: Data Infrastructure for Simulations
 * Copyright (C) 2002, 2003 Mark Rose <tm2@stm.lbl.gov>
 *
 * This file is part of SimData.
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
 * @file Composite.h
 *
 * Composite and Visitor design pattern templates.
 *
 **/


#ifndef __SIMDATA_COMPOSITE_H__
#define __SIMDATA_COMPOSITE_H__

#include <vector>
#include <SimData/Ref.h>
#include <SimData/Log.h>


NAMESPACE_SIMDATA


/** Interface for nodes that can accept visitors.
 *
 *  V is a context specific visitor class.
 */
template<class V>
class Visitable;

#define SIMDATA_VISITOR(__visitor) \
	typedef SIMDATA(Ref)<__visitor> Ref; \
	using SIMDATA(Visitor)<__visitor>::apply

#define SIMDATA_VISITABLE(__visitor) \
	virtual SIMDATA(Ref)<__visitor> accept(SIMDATA(Ref)<__visitor> v) { \
		v->apply(*this); \
		return v; \
	}

/** Interface for nodes that contain other nodes.
 *
 *  N is a context specific visitable node type.
 */
template<class N>
class CompositeBase;


/** Common base class for all Visitors.
 */
class VisitorBase: public virtual Referenced { };


/** Core visitor functionality.
 *
 *  N is a context specific visitable node type.
 */
template <class N>
class VisitorCore: public VisitorBase {
public:
	/// Composite traversal modes.
	typedef enum {
		TRAVERSE_NONE,     //< Act on only the current node.
		TRAVERSE_CHILDREN, //< Act on the current node and all children
		TRAVERSE_PARENTS   //< Act on the current node and all parents
	} TraversalMode;

	/** Get the mode for traversing the composite graph.
	 */
	TraversalMode getTraversalMode() const { return _traversal_mode; }

	/// Base class type for nodes in the graph.
	typedef CompositeBase<N> Node;

	/** Visit a node and propagate.
	 *
	 *  This method is overloaded in the actual visitor classes
	 *  to apply distinct operations to each type of node.  Apply
	 *  methods should generally conclude with a call to
	 *  traverse(node) to continue progagation of the visitor
	 *  through the graph.
	 */
	virtual void apply(Node &node) { traverse(node); }

	/** Constructor.
	 *
	 *  @param mode The initial traversal mode.  The default is
	 *              TRAVERSE_CHILDREN.
	 */
	VisitorCore(TraversalMode mode = TRAVERSE_CHILDREN):
		_traversal_mode(mode) {}

protected:
	/** Propagate from a given node based on the current traversal
	 *  mode.
	 *
	 *  TRAVERSE_CHILDREN will visit the subgraph of the node
	 *  (depth first), while TRAVERSE_PARENTS will visit all
	 *  parents until the root of the graph is reached.
	 */
	void traverse(Node &node) {
		if (_traversal_mode == TRAVERSE_CHILDREN) {
			descend(node);
		} else
		if (_traversal_mode == TRAVERSE_PARENTS) {
			ascend(node);
		}
	}

	/** Change the traversal mode.
	 *
	 *  The initial traversal mode can be set as a contructor parameter.
	 *  This method allows the traversal mode to be changed on the fly,
	 *  for example to abort traversal of a graph once a condition is
	 *  met (such as finding a matching node during a search).
	 */
	void setTraversalMode(TraversalMode mode) { _traversal_mode = mode; }

private:
	/// The current traversal mode.
	TraversalMode _traversal_mode;

	/** Visit all parents of a node.
	 */
	inline void ascend(Node &node);

	/** Visit all children of a node.
	 */
	inline void descend(Node &node);

};


/** Base interface for composite (visitable) nodes.
 *
 *  N is a context specific visitable node type.
 */
template <class N>
class CompositeBase: public virtual Referenced {
	friend class VisitorCore<N>;
protected:
	/** Propagate a visitor to all child nodes.
	 */
	virtual void descend(VisitorBase *v) = 0;

	/** Propagate a visitor to all parent nodes.
	 */
	virtual void ascend(VisitorBase *v) = 0;
};


/** A visitable node class that can have multiple children and parents.
 *
 *  Composite should be used as a base class for nodes
 *  in a Composite/Visitor design pattern.  First declare
 *  a context specific visitor class (see Visitor), then
 *  create use Composite<> as a base class for the
 *  primary node type.  Starting from a context specific
 *  visitor (see Visitor for details) named MyNodeVisitor,
 *  one would declare MyNode as:
 *
 *     class MyNode: public Composite<MyNodeVisitor> {
 *     public:
 *       SIMDATA_VISITABLE(MyNodeVisitor);
 *       // the rest of MyNode's declaration...
 *     };
 *
 *  MyNode and each of its subclasses must define an
 *  accept() method, which is just a couple lines of
 *  boilerplate code handled by the SIMDATA_VISITABLE
 *  macro (feel free to write it out by hand if you
 *  despise macros).  Defining accept() in each subclass
 *  allows the overloaded apply() method in MyNodeVisitor
 *  to be correctly selected based on the type of each
 *  node.
 *
 *  Notes:
 *
 *  Although Composite supports multiple parents, this
 *  capability can easily be disabled within a given
 *  context by overriding the addChild() method of the
 *  primary node class and testing that getNumOfParents()
 *  equals zero for each child to be added.
 *
 *  The current implementation does not support non-
 *  composite leaf nodes.  Any node can contain other
 *  nodes, although again this feature can be disabled
 *  for specific node classes simply by overriding
 *  addChild().
 *
 *  @param V is a context specific visitor class.
 */
template <class V>
class Composite: public Visitable<V>, public CompositeBase< Visitable<V> > {
public:
	typedef Composite Node;
	typedef Ref<Node> NodeRef;
	typedef Ref<V> VisitorRef;

	/// A list of child node references.
	typedef std::vector< NodeRef > ChildList;
	
	/// A list of parent node pointers (which avoids circular references).
	typedef std::vector< Node* > ParentList;

	/** Accept a visitor.
	 *
	 *  This method must be implemented in each subclasses to
	 *  ensure proper dispatch of the visitor's apply methods.
	 *  See the Composite class documentation for details.
	 */
	virtual VisitorRef accept(VisitorRef v)=0;

	/** Add a child to this node.
	 *
	 *  Extend this method to inforce constraints on the graph structure,
	 *  such as preventing nodes from having multiple parents.
	 *
	 *  @returns True if the child was added, false if it already exists.
	 */
	virtual bool addChild(Node *node) {
		if (!isContainer()) {
			SIMDATA_LOG(LOG_ALL, LOG_WARNING, "simdata::Composite<>::addChild() to non-container.");
			return false;
		}
		if (!node) {
			SIMDATA_LOG(LOG_ALL, LOG_WARNING, "simdata::Composite<>::addChild() null node.");
			return false;
		}
		if (!node->canBeAdded()) {
			SIMDATA_LOG(LOG_ALL, LOG_WARNING, "simdata::Composite<>::addChild() cannot be added.");
			return false;
		}
		if (containsNode(node)) {
			SIMDATA_LOG(LOG_ALL, LOG_WARNING, "simdata::Composite<>::addChild() duplicate node.");
			return false;
		}
		_children.push_back(node);
		node->addParent(this);
		return true;
	}

	/** Remove a child from this node.
	 *
	 *  @returns True if the child was removed, false if it was not found.
	 */
	virtual bool removeChild(Node *node) {
		return removeChild(getChildIndex(node));
	}

	/** Remove one or more children.
	 *
	 *  @param pos The index of the first child to remove.
	 *  @param count The number of children to remove (default 1).
	 *  @returns True if one or more children were removed, false
	 *           otherwise.
	 */
	virtual bool removeChild(unsigned int pos, unsigned int count=1) {
		if (pos >= _children.size() || count == 0) return false;
		unsigned int end = pos + count;
		if (end > _children.size()) {
			SIMDATA_LOG(LOG_ALL, LOG_WARNING, "simdata::Composite<>::removeChild() index range truncated.");
			end = _children.size();
		}
		for (unsigned int i = pos; i < end; ++i) {
			_children[i]->removeParent(this);
		}
		_children.erase(_children.begin()+pos,_children.begin()+end);
		return true;
	}

	/** Get the number of immediate children of this node.
	 */
        inline unsigned int getNumChildren() const { return _children.size(); }

	/** Replace an existing child node with a new one.
	 *
	 *  The original node will be destroyed if it is not already
	 *  referenced elsewhere.
	 *
	 *  @returns True if the child was replaced, false otherwise.
	 */
	virtual bool setChild(unsigned int i, Node* node) {
		if (i >= _children.size() || !node) return false;
		NodeRef original = _children[i];
		original->removeParent(this);
		_children[i] = node;
		node->addParent(this);
		return true;
	}

	/** Get a child by index number.
	 *
	 *  @returns 0 if the index is out of range.
	 */
	inline Node* getChild(unsigned int i) {
		if (i >= _children.size()) return 0;
		return _children[i].get();
	}

	/** Get a child by index number.
	 *
	 *  @returns 0 if the index is out of range.
	 */
	inline const Node* getChild(unsigned int i) const {
		if (i >= _children.size()) return 0;
		return _children[i].get();
	}

	/** Get a list of immediate children of this node.
	 */
	inline ChildList const &getChildren() const { return _children; }

	/** Test if this node contains a given node.
	 *
	 *  @returns True if the node is an immediate child, false otherwise.
	 */
	inline bool containsNode(Node const *node) const {
		if (!node) return false;
		return std::find(_children.begin(), _children.end(), node) != _children.end();
	}

	/** Get the index of a child node.
	 *
	 *  @returns The index if found, otherwise the number of children.
	 */
	inline unsigned int getChildIndex(Node const *node) const {
		for (unsigned int idx = 0; idx < _children.size(); ++idx) {
			if (_children[idx] == node) return idx;
		}
		return _children.size(); // not found
	}

	/** Get a list of immediate parents of this node.
	 */
	inline const ParentList &getParents() const { return _parents; }

	/** Get a parent by index number.
	 *
	 *  @returns 0 if the index is out of range.
	 */
	inline Node* getParent(unsigned int i)  {
		if (i >= _parents.size()) return 0;
		return _parents[i];
	}

	/** Get a parent by index number.
	 *
	 *  @returns 0 if the index is out of range.
	 */
	inline const Node* getParent(unsigned int i) const  {
		if (i >= _parents.size()) return 0;
		return _parents[i];
	}

	/** Get the number of immediate parents of this node.
	 */
	inline unsigned int getNumParents() const { return _parents.size(); }

	virtual bool isContainer() const { return true; }
	virtual bool canBeAdded() const { return true; }

protected:
	/** Get a copy of this node's parent list.
	 */
	inline ParentList getParents() { return _parents; }

	/** A function adapter for visiting child or parent nodes.
	 */
	class AcceptOp {
		typename Composite::VisitorRef _visitor;
	public:
		AcceptOp(VisitorBase *visitor) { _visitor = visitor; }
		void operator()(NodeRef node) { node->accept(_visitor); }
	};

	/** Propagate a visitor through all immediate children.
	 */
	void descend(VisitorBase *visitor) {
		std::for_each(_children.begin(), _children.end(), AcceptOp(visitor));
	}

	/** Propagate a visitor through all immediate parents.
	 */
	void ascend(VisitorBase *visitor) {
		std::for_each(_parents.begin(), _parents.end(), AcceptOp(visitor));
	}

private:
	/** Add a new parent to this node.
	 */
	void addParent(Node *node) {
		_parents.push_back(node);
	}

	/** Remove a parent from this node.
	 */
	void removeParent(Node *node) {
		typename ParentList::iterator iter;
		iter = std::find(_parents.begin(), _parents.end(), node);
		if (iter != _parents.end()) _parents.erase(iter);
	}

	/// The immediate children of this node.
	ChildList _children;

	/// The immediate parents of this node.
	ParentList _parents;
};


template <class N>
void VisitorCore<N>::descend(Node &node) {
	node.descend(this);
};


template <class N>
void VisitorCore<N>::ascend(Node &node) {
	node.ascend(this);
};


/** Base class for nodes that accept visitors.
 */
template <class V>
class Visitable: public virtual Referenced {
public:
	virtual Ref<V> accept(Ref<V> v)=0;
};


/** A context specific visitor class.
 *
 *  Subclass Visitor to define a custom visitor class
 *  for a specific context.  Although somewhat counter
 *  intuitive, the template parameter must be the sub-
 *  class being defined.  For example:
 *
 *     class MyNode;
 *     class MySpecialNode;
 *     // forward declarations of other subclasses of MyNode
 *
 *     class MyNodeVisitor: public Visitor<MyNodeVisitor> {
 *     public:
 *       using Visitor<MyNodeVisitor>::apply;
 *       virtual void apply(MyNode &node) { apply((Node&)node); }
 *       virtual void apply(MySpecialNode &node) { apply((MyNode&)node); }
 *       // apply stubs for any other subclasses of MyNode
 *     }
 *
 *  Note first that the base class apply() method must be brought
 *  into scope with a using directive.  There is also a macro
 *  SIMDATA_VISITOR() which does this and defines a Ref typedef,
 *  if you prefer using macros for boilerplate.
 *
 *  The rest of the class should define apply() stubs for each of
 *  the node classes in the current context.  Each of these node
 *  classes must be forward declared, and the apply stubs should
 *  chain from subclass to baseclass, ending finally with a call
 *  to apply((Node&)node), where Node is a type defined by Visitor
 *  that will be a base class of MyNode as long as MyNode
 *  derives from Composite<>.  See Composite for more details.
 */
template <class V>
class Visitor: public VisitorCore< Visitable<V> > {};


/** A visitor class for searching a node graph.
 *
 *  The visitor traverses the graph until the match()
 *  method returns true.  Implement this method in a
 *  derived class to specify the search condition.
 *  Traversal stops as soon as a match is found. After
 *  traversal the getNode() method can be used
 *  to retrieve the matching node (if any).
 */
template <class N, class V>
class FindVisitor: public V {
public:
	/** The search condition.
	 *
	 *  Implement this condition in derived classes to
	 *  deterimine which node is found.
	 */
	virtual bool match(N &node) = 0;

	/** Search for a matching node.
	 *
	 *  Don't call this directly; use node->accept(visitor);
	 */
	void apply(N &node) {
		if (_node.valid()) return;
		if (match(node)) {
			_node = &node;
			setTraversalMode(TRAVERSE_NONE);
		} else {
			traverse(node);
		}
	}

	/** Get the node that match the search condition, if any.
	 */
	simdata::Ref<N> getNode() const { return _node; }
private:
	/// The matching node.
	simdata::Ref<N> _node;
};


/** A visitor class for searching a node graph.
 *
 *  This visitor is similar to FindVisitor, but retrieves
 *  all nodes that match the condition.
 */
template <class N, class V>
class FindAllVisitor: public V {
public:
	typedef std::vector< simdata::Ref<N> > NodeList;

	/** The search condition.
	 *
	 *  Implement this condition in derived classes to
	 *  deterimine which nodes are found.
	 */
	virtual bool match(N &node) = 0;

	/** Search for and accumulate matching nodes.
	 *
	 *  Don't call this directly; use node->accept(visitor);
	 */
	void apply(N &node) {
		if (match(node)) {
			_nodes.push_back(&node);
		}
		traverse(node);
	}
	/** Get all nodes that match the search condition.
	 */
	NodeList getNodes() const { return _nodes; }
private:
	/// The matching nodes.
	NodeList _nodes;
};


NAMESPACE_SIMDATA_END

#endif // __SIMDATA_COMPOSITE_H__

