# Copyright 2004-2005 Mark Rose <mkrose@users.sourceforge.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import sys
import os
import os.path
from . import domtree
import xml.dom.minidom
import tempfile

from . import ObjectInterface
from csp.tools.layout2 import layout_module
from .DataPath import DataPath

class RecursionError(Exception):
	pass


def roundTo(x, dx):
	return round(x / dx) * dx

class NodeMap:
	"""
	NodeMap stores a map from C++ LayoutNode instances to Python Node instances.
	The former are wrappers around OSG subgraphs that support placement and
	rotation.  The latter represent primitive and non-primitive elements in the
	XML tree.  NodeMaps are constructed when a Node hierarchy is first realized
	(i.e. its LayoutNode dual is constructed), and are used to find Nodes
	corresponding to elements in the scene.
	"""

	def __init__(self, graph):
		"""
		Construct a new NodeMap for a node hierarchy.  The graph argument is
		a Node instance at the root of the hierarchy.
		"""
		self._map = {}
		self._group_map = {}
		self._group_cache = {}
		self._root = None  # LayoutNode at the root of the graph
		self._path = []
		self._graph = graph

	def add(self, node, layout_node):
		"""
		Add a Node to LayoutNode association.  The first LayoutNode added to the
		map will be treated as the root of the graph.
		"""
		# layout_node will be None for unsupported nodes; these are omitted from the map
		if layout_node is not None:
			# store this ptr as string; LayoutNodes are owned by the C++ code so it
			# isn't safe to hold a real pointer.
			key = str(layout_node.asNode().this)
			self._map[key] = node
			if self._root is None:
				self._root = layout_node

	def addGroup(self, group, feature_node):
		if feature_node is not None:
			# store this ptr as string; LayoutNodes are owned by the C++ code so it
			# isn't safe to hold a real pointer.
			key = str(feature_node.asNode().this)
			self._group_map[key] = group
	
	def cacheGroup(self, xmlpath, group):
		self._group_cache[xmlpath] = group

	def getCachedGroup(self, xmlpath):
		return self._group_cache.get(xmlpath)

	def getRoot(self):
		root = self._root
		self._root = None
		return root

	def clearRoot(self):
		self._root = None

	def get(self, ptr):
		if not isinstance(ptr, str):
			ptr = str(ptr.asNode().this)
		return self._map.get(ptr)

	def getGroup(self, ptr):
		if not isinstance(ptr, str):
			ptr = str(ptr.asNode().this)
		return self._group_map.get(ptr)

	def graph(self):
		return self._graph

	def nodes(self):
		return list(self._map.values())

	def pushLayoutNodePath(self, feature_node):
		assert(feature_node.isGroup())
		self._path.append(feature_node)
	
	def popLayoutNodePath(self):
		self._path.pop()

	def setLayoutNodePath(self, path):
		self._path = path[:]

	def getParentLayoutNode(self):
		assert(self._path)
		return self._path[-1]

	def merge(self, node_map):
		self._map.update(node_map._map)

	def save(self):
		self._graph.save()
		self._graph.makeClean();


class Node:
	"""
	Node represents an abstract element in the XML DOM tree.  Node subclasses
	can represent primitive elements, such as integer values (<Int> tags), or
	complex elements such as Object classes (<Object> tags).

	Nodes form a tree that mirrors the DOM structure.  Nodes record which
	source file they originate from to allow saving modifications back to the
	original xml file.  Node instances provide accessors for their contents and
	track modifications through a dirty flag.  To support "undo" functionality,
	nodes can be marked as deleted without removing them from the tree.

	A Node tree can be "realized" to create a parallel tree of LayoutNode
	instances (C++) that wrap the objects described by the xml structure.
	During realization, a NodeMap is constructed to associate LayoutNodes with
	the corresponding Node instances.  In turn, LayoutNodes are typically
	realized to generate scene graph nodes.  These nodes are linked back to the
	LayoutNodes via the "user data" field in osg::Object, allowing
	bi-directional traversal between Nodes and osg::Objects.
	"""

	Classes = {}

	def __init__(self, node, parent, path):
		if path is None:
			assert(parent is not None)
			path = parent._path
		assert(isinstance(path, DataPath))
		self._dirty = 0
		self._deleted = 0
		self._update_marker = 0
		self._path = path
		self._node = node
		self._parent = parent
		self._children = []
		self._named_children = {}
		for child in node.children:
			child_node = Node.MakeNode(child, self, path)
			if child.type == domtree.Node.ELEMENT_NODE:
				name = child.attr('name')
				self._named_children[name] = child_node
			self._children.append(child_node)
		self.init()

	def filepath(self):
		return self._path.abspath()

	def xmlpath(self):
		return self._path.datapath()

	def xmlroot(self):
		return self._path.absroot()

	def setPath(self, path, old_path=None):
		if old_path is None:
			old_path = self.filepath()
		if old_path == self.filepath():
			self._path = path
			self._dirty = 1
			for child in self._children:
				child.setPath(path, old_path)

	def init(self):
		pass

	def __str__(self):
		return 'Node %s' % (self.__class__)

	def __repr__(self):
		return str(self)

	def get(self, key, default=None):
		return self._named_children.get(key, default)

	def __getattr__(self, key):
		if key in self.__dict__: return self.__dict__[key]
		value = self.__dict__['_named_children'].get(key, None)
		if value is not None:
			if isinstance(value, SimpleNode):
				return value.get()
		if value is None and key.startswith('__'):
			# needed for iteration to find __getitem__ instead of __iter__,
			# probably important for other reasons as well.
			raise AttributeError
		return value

	def __setattr__(self, key, value):
		if key not in self.__dict__ and '_named_children' in self.__dict__:
			child = self._named_children.get(key, None)
			if child is not None:
				if isinstance(child, SimpleNode):
					child.set(value)
					return
		self.__dict__[key] = value

	def ParseFile(file, root, parent=None):
		tree = domtree.ParseFile(file)
		assert(tree.type == domtree.Node.DOCUMENT_NODE)
		assert(len(tree) == 1)
		return Node.MakeNode(tree, parent, DataPath(file, root))
	ParseFile = staticmethod(ParseFile)

	def MakeNode(node, parent, path):
		assert(isinstance(path, DataPath))
		if node.type == domtree.Node.ELEMENT_NODE:
			if node.name == 'Object':
				NodeClass = Node.Classes.get(node.attr('class'))
			else:
				NodeClass = Node.Classes.get(node.name)
			if NodeClass:
				return NodeClass(node, parent, path)
		return GenericNode(node, parent, path)
	MakeNode = staticmethod(MakeNode)

	def presave(self):
		pass

	def create(self, node_map, mapto):
		return None

	def _realize(self, node_map):
		"""
		Internal method to generate LayoutNode instances for all supported Nodes in the subgraph,
		realize() is the corresponding public method.
		"""
		# traverse the entire subgraph, buiding a list of FeatureGroups
		for child in self._children:
			child._realize(node_map)

	def realize(self):
		"""
		Generate the LayoutNode graph for all supported Nodes in the subgraph, returning a NodeMap
		instance.
		"""
		node_map = NodeMap(self)
		self._realize(node_map)
		return node_map

	def realizeAsChild(self, parent, node_map):
		node_map.setLayoutNodePath([parent])
		self._realize(node_map)
		return node_map

	def addChild(self, node):
		assert(isinstance(node, Node))
		if self.hasRecursion(node):
			raise RecursionError
		self._children.append(node)
		self._node.addchild(node._node)
		self._dirty = 1

	def dirty(self):
		if self._dirty: return 1
		for child in self._children:
			if isinstance(child, Node):
				if child.dirty():
					return 1
		return 0

	def makeClean(self):
		self._dirty = 0
		for child in self._children:
			if isinstance(child, Node):
				child.makeClean()

	def delete(self):
		if not self._deleted:
			self._deleted = 1
			print('delete', self._parent, self)
			self._parent._node.removeChild(self._node)
			self._parent._dirty = 1

	def undelete(self):
		if self._deleted:
			self._parent._node.appendChild(self._node)   # order will not be preserved
			self._deleted = 0
		self._update_marker = 0

	def updateFrom(self, feature_node):
		self._update_marker = 1

	def wasUpdated(self):
		return self._update_marker

	def save(self):
		if self._deleted:
			return
		self.presave()
		for child in self._children:
			if isinstance(child, Node):
				child.save()
		if self._node.type == domtree.Node.DOCUMENT_NODE and self.dirty():
			tmp_fn = os.path.join(os.path.dirname(self.filepath()), '.csplayout-tmp.sav')
			with open(tmp_fn, 'w+b') as tmp:
			        tmp.write(self._node.node.toprettyxml().encode())
			os.rename(tmp_fn, self.filepath())
			self.makeClean()

	def addLayoutNode(self, feature_node):
		raise RuntimeError('Adding feature nodes to xml node %s not supported' % self.__class__)

	def containsNode(self, node_type):
		if isinstance(self, node_type): return 1
		for child in self._children:
			if child.containsNode(node_type): return 1
		return 0

	def rootNodeIs(self, node_type):
		if self._node.type == domtree.Node.DOCUMENT_NODE:
			if isinstance(self._children[0], node_type): return True
		return False

	def _parentFiles(self, files):
		if not self._path.isNoPath():
			files[self.filepath()] = 1
		if self._parent:
			self._parent._parentFiles(files)

	def _hasFileIn(self, files, test=0):
		if self._node.type == domtree.Node.DOCUMENT_NODE:
			test = 1
		if test and self.filepath() in files: return 1
		for child in self._children:
			if child._hasFileIn(files, test): return 1
		return 0

	def hasRecursion(self, subgraph):
		files = {}
		self._parentFiles(files)
		return subgraph._hasFileIn(files)


class GenericNode(Node):
	pass


class SimpleNode(Node):
	def get(self):
		return self._value
	def set(self, value):
		self._dirty = 1
		self._value = value
	def MakeDomNode(type, name, value, doc=None):
		if doc is None:
			node = xml.dom.minidom.Element(type)
		else:
			node = doc.createElement(type)
		node.setAttribute('name', name)
		text = xml.dom.minidom.Text()
		text.data = str(value)
		node.appendChild(text)
		return node
	MakeDomNode = staticmethod(MakeDomNode)

class Float(SimpleNode):
	def init(self):
		self._value = float(self._node.text)
	def presave(self):
		self._node.text = str(self._value)
	def MakeDomNode(name, value, doc=None):
		return SimpleNode.MakeDomNode('Float', name, value, doc=doc)
	MakeDomNode = staticmethod(MakeDomNode)
Node.Classes['Float'] = Float

class Int(SimpleNode):
	def init(self):
		self._value = int(self._node.text)
	def presave(self):
		self._node.text = str(self._value)
	def MakeDomNode(name, value, doc=None):
		return SimpleNode.MakeDomNode('Int', name, value, doc=doc)
	MakeDomNode = staticmethod(MakeDomNode)
Node.Classes['Int'] = Int

class Path(Node):
	def init(self):
		path = self._path.makeRelativeFilePath(self._node.text)
		child = Node.ParseFile(path, self.xmlroot(), parent=self)
		self._children = [child]
		self._child = child
		self._child_root = child._children[0]
		self._value = self._node.text
	def presave(self):
		self._child.save()
		self._node.text = self._path.makeRelativeXmlPath(self._value)
	def MakeDomNode(name, value, doc=None):
		return SimpleNode.MakeDomNode('Path', name, value, doc=doc)
	MakeDomNode = staticmethod(MakeDomNode)
Node.Classes['Path'] = Path

class External(SimpleNode):
	def init(self):
		self._value = self._node.text
	def presave(self):
		self._node.text = self._value
	def MakeDomNode(name, value, doc=None):
		return SimpleNode.MakeDomNode('External', name, value, doc=doc)
	MakeDomNode = staticmethod(MakeDomNode)
Node.Classes['External'] = External

class Link(SimpleNode):
	def init(self):
		self._value = self._node.text
	def presave(self):
		self._node.text = self._value
	def MakeDomNode(name, value, doc=None):
		return SimpleNode.MakeDomNode('Link', name, value, doc=doc)
	MakeDomNode = staticmethod(MakeDomNode)
Node.Classes['Link'] = Link

class String(SimpleNode):
	def init(self):
		self._value = self._node.text
	def presave(self):
		self._node.text = self._value
	def MakeDomNode(name, value, doc=None):
		return SimpleNode.MakeDomNode('String', name, value, doc=doc)
	MakeDomNode = staticmethod(MakeDomNode)
Node.Classes['String'] = String

class Vector2(SimpleNode):
	def init(self):
		self._value = list(map(float, self._node.text.split()))
	def presave(self):
		self._node.text = '%f %f   ' % tuple(self._value)
	def MakeDomNode(name, value, doc=None):
		return SimpleNode.MakeDomNode('String', name, '%f %f' % value, doc=doc)
	MakeDomNode = staticmethod(MakeDomNode)
Node.Classes['Vector2'] = Vector2

class Vector3(SimpleNode):
	def init(self):
		self._value = list(map(float, self._node.text.split()))
	def presave(self):
		self._node.text = '%f %f %f' % tuple(self._value)
	def MakeDomNode(name, value, doc=None):
		return SimpleNode.MakeDomNode('String', name, '%f %f %f' % value, doc=doc)
	MakeDomNode = staticmethod(MakeDomNode)
Node.Classes['Vector3'] = Vector3

class Bool(SimpleNode):
	def init(self):
		self._value = self._node.text.strip().lower() == 'true'
	def presave(self):
		if self._value:
			self._node.text = 'true'
		else:
			self._node.text = 'false'
	def MakeDomNode(name, value, doc=None):
		text = 'true'
		if not value: text = 'false'
		return SimpleNode.MakeDomNode('Bool', name, text, doc=doc)
	MakeDomNode = staticmethod(MakeDomNode)
Node.Classes['Bool'] = Bool

class List(Node):
	def init(self):
		pass
	def presave(self):
		pass
	def __len__(self):
		return len(self._children)
	def __getitem__(self, index):
		return self._children[index]
	def MakeDomNode(name, children, doc=None):
		if doc is None:
			node = xml.dom.minidom.Element('List')
		else:
			node = doc.createElement('List')
		node.setAttribute('name', name)
		for child in children:
			node.appendChild(child)
		return node
	MakeDomNode = staticmethod(MakeDomNode)
Node.Classes['List'] = List


class CustomLayoutModel(Node):
	def init(self):
		self.__group = None
	def makeGroup(self, node_map):
		cloned = 0
		if self.__group is None:
			# a little dangerous to hang onto a disowned pointer, but it
			# is only used during realization (before C++ gets it).
			group = node_map.getCachedGroup(self.xmlpath())
			if group is None:
				self.__group = layout_module.LayoutGroup()
				node_map.cacheGroup(self.xmlpath(), self.__group)
			else:
				self.__group = layout_module.LayoutGroup(group)
				cloned = 1
			self.__group.setName(str(self.xmlpath()))
			self.__group.thisown = 0
		return self.__group, cloned
	def _realize(self, node_map):
		group, cloned = self.makeGroup(node_map)
		print('CustomLayoutModel._realize', group)
		node_map.add(self, group)
		node_map.addGroup(self, group)
		node_map.pushLayoutNodePath(group)
		if not cloned: Node._realize(self, node_map)
		node_map.popLayoutNodePath()
	def create(self, node_map, mapto):
		group, cloned = self.makeGroup(node_map)
		print('CustomLayoutModel._create', group)
		node_map.getParentLayoutNode().addChild(group)
		node_map.add(mapto, group)
		node_map.addGroup(self, group)
		node_map.pushLayoutNodePath(group)
		if not cloned: Node._realize(self, node_map)
		node_map.popLayoutNodePath()
		return group
	def MakeDomNode(children, doc=None):
		if doc is None:
			object = xml.dom.minidom.Element('Object')
		else:
			object = doc.createElement('Object')
		object.setAttribute('class', 'CustomLayoutModel')
		# XXX fixme convert children to domnodes
		object.appendChild(List.MakeDomNode('layout', children, doc=doc))
		return object
	MakeDomNode = staticmethod(MakeDomNode)

	def New(path):
		"""
		Factory for creating a new CustomLayoutModel node that is not part of
		an existing xml hierarchy.  This creates a top-level document node and
		a CustomLayoutModel node; this factory does not yet support the creation
		of nested CustomLayoutModel nodes.
		"""
		doc = xml.dom.minidom.Document()
		node = CustomLayoutModel.MakeDomNode([], doc=doc)
		doc.appendChild(node)
		tree = domtree.DomTree(doc)
		graph = Node.MakeNode(tree, None, path)
		graph._dirty = 1
		clm = graph._children[0]
		return graph, clm
	New = staticmethod(New)

	def addLayoutNode(feature_node):
		node = FeatureLayout.MakeDomNode('model', feature_node.getX(), feature_node.getY(), feature_node.getAngle())
		print(node.toxml())
	addLayoutNode = staticmethod(addLayoutNode) # XXX temporary hack


Node.Classes['CustomLayoutModel'] = CustomLayoutModel


class FeatureLayout(Node):
	def init(self):
		self._model_node = self.model._child._children[0]  # FIXME comment nodes, etc???
	def _realize(self, node_map):
		model = self._model_node.create(node_map, self)
		if model is not None:
			model.move(self.x, self.y)
			model.rotate(self.orientation)
	def updateFrom(self, feature_node):
		Node.updateFrom(self, feature_node)
		# only set if needed to we don't mark nodes as dirty unnecessarily
		if abs(self.x - feature_node.getX()) >= 0.01:
			self.x = roundTo(feature_node.getX(), 0.01);
		if abs(self.y - feature_node.getY()) >= 0.01:
			self.y = roundTo(feature_node.getY(), 0.01);
		if abs(self.orientation - feature_node.getAngle()) >= 0.0001:
			self.orientation = roundTo(feature_node.getAngle(), 0.001);

	def MakeDomNode(model, x, y, orientation):
		node = xml.dom.minidom.Element('Object')
		node.setAttribute('class', 'FeatureLayout')
		node.appendChild(Path.MakeDomNode('model', model))
		node.appendChild(Float.MakeDomNode('x', x))
		node.appendChild(Float.MakeDomNode('y', y))
		node.appendChild(Float.MakeDomNode('orientation', orientation))
		return node
	MakeDomNode = staticmethod(MakeDomNode)

	def New(parent, model_path):
		"""
		Factory for creating a new FeatureLayout node that is not part of
		an existing xml hierarchy.  The resulting node is patched into an
		existing xml hierarchy under parent (which should be a CustomLayoutModel
		node).  The model_path parameter specifies the path to child model;
		we do not yet support inlined models.  The model is placed at (0,0)
		relative to the parent's origin, with no rotation.
		"""
		node = FeatureLayout.MakeDomNode(model_path, 0, 0, 0)  # FIXME need doc?
		node = domtree.DomTree(node)
		# can only insert FeatureLayouts into nodes that have layout lists (e.g. CustomLayoutModel)
		layout_list = parent.layout
		assert(layout_list is not None)
		layout = FeatureLayout(node, layout_list, None)
		layout_list.addChild(layout)
		return layout
	New = staticmethod(New)

Node.Classes['FeatureLayout'] = FeatureLayout


class ObjectModel(Node):
	def init(self):
		pass
Node.Classes['ObjectModel'] = ObjectModel


class FeatureObjectModel(Node):
	def init(self):
		self.__model = None

	def makeModel(self):
		if self.__model is None:
			if isinstance(self.model, ObjectModel):
				object_model = self.model
			else:
				# path to another xml file, so dereference to get the model node
				object_model = self.model._child_root

			self.__model = layout_module.LayoutModel()
			self.__model.setName(str(self.xmlpath()))

			# load the model using the full ObjectModel functionality (if available)
			model = ObjectInterface.MakeObjectModel(object_model._node)
			self.__model.setModel(model)
			self.__model.thisown = 0
		return self.__model

	def _realize(self, node_map):
		model = self.makeModel()
		node_map.add(self, model)

	def create(self, node_map, mapto):
		model = self.makeModel()
		node_map.getParentLayoutNode().addChild(model)
		node_map.add(mapto, model)
		return model
Node.Classes['FeatureObjectModel'] = FeatureObjectModel


class RandomBillboardModel(Node):
	def makeModel(self):
		if self.__model is None:
			model = ObjectInterface.MakeFeatureModel(self._node)
			# TODO check valid
			self.__model = layout_module.LayoutModel()
			self.__model.setName(str(self.xmlpath()))
			self.__model.setModel(model)
			self.__model.thisown = 0
		return self.__model
	def _realize(self, node_map):
		model = self.makeModel()
		if model is not None:
			node_map.add(self, model)
	def create(self, node_map, mapto):
		model = self.makeModel()
		if model is not None:
			node_map.getParentLayoutNode().addChild(model)
			node_map.add(mapto, model)
		return model
Node.Classes['RandomBillboardModel'] = RandomBillboardModel

# RandomForestModel has the same implementation for now
Node.Classes['RandomForestModel'] = RandomBillboardModel


class GenericOsgTerrain(Node):
	pass
Node.Classes['GenericOsgTerrain'] = GenericOsgTerrain


if __name__ == '__main__':
	graph = Node.ParseFile(sys.argv[1])
	graph.realize().save()


"""
FeatureLayout:
	Path model
	Float x
	Float y
	Float orientation

CustomLayoutModel:
	List layout

FeatureObjectModel:
	Path model
	Int hit_points

RandomBillboardModel:
	List models
	List density
	Float minimum_spacing
	RectangularCurve isocontour

RectangularCurve:
	Float width
	Float height

FeatureQuad:
	External texture
	Float width
	Float height

ObjectModel:
	External model_path
	Vector3 axis_0
	Vector3 axis_1
	Vector3 offset
	Float polygon_offset
	Bool lighting
	Int cull_face
	Float scale
	List<Vector3> contacts
	Bool smooth
	Bool filter
"""


