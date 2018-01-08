# Copyright 2004 Mark Rose <mkrose@users.sourceforge.net>
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
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

"""
Provides a wrapper class for the minidom nodes that exposes a simpler
interface using lots of Python magic.
"""

from xml.dom.minidom import parse, parseString, Node
import xml.dom


class DomTree:
	"""
	Dom node wrapper providing access to child nodes via instance
	attributes and iterators.
	"""

	def __init__(self, dom):
		self.name = dom.nodeName
		self.type = dom.nodeType
		self.node = dom
		text = ''
		self.children = []
		self._dict = {}
		remove = []
		for node in dom.childNodes:
			if node.nodeType == node.TEXT_NODE:
				text += node.data
				remove.append(node)
			else:
				tree = DomTree(node)
				self.children.append(tree)
				self._dict[tree.name] = tree
		for node in remove:
			dom.removeChild(node)
		self.settext(text)

	def addchild(self, dom):
		if not isinstance(dom, DomTree):
			dom = DomTree(dom)
		node = dom.node
		self.node.appendChild(node)
		if node.nodeType == node.TEXT_NODE:
			self.setattr('text', self.text + dom.data)
		else:
			self.children.append(dom)
			self._dict[dom.name] = dom

	def removeChild(self, dom):
		assert(isinstance(dom, DomTree))
		self.children.remove(dom)
		self.node.removeChild(dom.node)

	def __setattr__(self, key, value):
		if key == 'text':
			self.settext(value)
		else:
			self.__dict__[key] = value

	def setattr(self, key, value):
		self.__dict__[key] = value

	def settext(self, text):
		text = text.strip()
		set = 0
		for node in self.node.childNodes:
			if node.nodeType == node.TEXT_NODE:
				if not set:
					set = 1
					node.data = text
				else:
					node.data = ''
		self.setattr('text', text)
		if not set and text:
			text_node = xml.dom.minidom.Text()
			text_node.data = text
			self.node.appendChild(text_node)

	def __getattr__(self, key):
		if key.startswith('_'):
			return self.__dict__[key]
		return self.__dict__['_dict'].get(key, None)

	def __eq__(self, other):
		return self is other

	def attr(self, name, default=None):
		return self.node.getAttribute(name)

	def attrs(self):
		attrs = getattr(self.node, '_attrs', None)
		if attrs is None: return []
		return list(attrs.keys())

	def hasattr(self, name):
		return self.node.hasAttribute(name)

	def __len__(self): return len(self.children)

	def __str__(self): return self.__repr__()

	def __repr__(self):
		if self.text.strip(): return self.text
		return 'node %s' % self.name

	def keys(self):
		return list(self._dict.keys())

	def get(self, name, default):
		return self.child

	def __iter__(self):
		for child in self.children:
			yield child
		raise StopIteration


def ParseFile(file):
	"""Generate a DomTree from an open file handle"""
	return DomTree(parse(file))


def ParseString(text):
	"""Generate a DomTree from a string of XML data"""
	return DomTree(parseString(text))

