# CSPEdit - Copyright (C) 2003 Mark Rose
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, US

import CSPEdit

class Core:

	class SelectionVistior:
		def __init__(self, search):
			self.search = map(lambda x: x.this, search)
			self.selection = []
		def __call__(self, node):
			if node.this in self.search:
				self.selection.append(node)
		def getSelection(self):
			return self.selection

	def __init__(self, view):
		self._view = view
		self._root = view.getRoot()
		self._root.children = []
		self._selection = []

	def getRoot(self):
		return self._root

	def load(self, fn):
		model = CSPEdit.FeatureModel()
		if not model.load(fn): return None
		model.parent = self._root
		model.children = []
		self._view.add(model)
		self._root.children.append(model)
		return model

	def walk(self, method, group=None):
		if group is None: group = self._root.children
		for child in group:
			method(child)
			self.walk(method, child.children)

	def accept(self, visitor):
		self.walk(visitor)

	def updateSelection(self):
		selection = self._view.getSelection()
		visitor = Core.SelectionVisitor(selection)
		self.walk(visitor)
		self._selection = visitor.getSelection()

	def move(self, x, y):
		self.updateSelection()
		for node in self._selection:
			node.move(x,y)
