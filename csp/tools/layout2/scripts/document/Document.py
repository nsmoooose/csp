from csp.base.signals import Signal

class Document:
	"""Base class for all documents that can be loaded
	within this editor."""

	def __init__(self, name):
		self.name = name
		self.changedSignal = Signal()
		
		# refCount indicates how many views are referencing this document.
		# It is managed by the DocumentRegistry
		self.refCount = 0

	def GetName(self):
		"""Returns the name of the document."""
		return self.name

	def GetUniqueId(self):
		"""Returns a unique Id identifying the document in the DocumentRegistry.
		Inheriting classes should implement this method"""
		return "Document:" + self.GetName()
	
	def incrementRefCount(self):
		"""This methode should only be called by DocumentRegistry."""
		self.refCount = self.refCount + 1
	
	def decrementRefCount(self):
		"""This methode should only be called by DocumentRegistry."""
		self.refCount = self.refCount - 1
		return self.refCount
	
	def GetChangedSignal(self):
		return self.changedSignal

	def Dispose(self):
		self.changedSignal.Dispose()
		self.changedSignal = None

	def IsReadOnly(self):
		"""Tests to see if the current document is read only or not."""
		return True

	def Save(self):
		"""Saves the document. Inheriting classes should implement this
		method."""
		pass
