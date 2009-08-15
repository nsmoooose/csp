#!/usr/bin/env python

from FileDocument import FileDocument

class SceneDocument(FileDocument):
	"""This document represents a scene of 3D objects."""

	def __init__(self, name):
		FileDocument.__init__(self, name)
		self.node = None
		
	def SetRootNode(self, node):
		self.node = node
		self.GetChangedSignal().Emit(self)
		
	def GetRootNode(self):
		return self.node

	def Clear(self):
		self.node = None
		self.GetChangedSignal().Emit(self)
