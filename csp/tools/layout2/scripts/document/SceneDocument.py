#!/usr/bin/env python

from Document import Document

class SceneDocument(Document):
	"""This document represents a scene of 3D objects."""

	def __init__(self, name):
		Document.__init__(self, name)
		self.node = None
		
	def SetRootNode(self, node):
		self.node = node
		self.GetChangedSignal().Emit(self)
		
	def GetRootNode(self):
		return self.node

	def Clear(self):
		self.node = None
		self.GetChangedSignal().Emit(self)
