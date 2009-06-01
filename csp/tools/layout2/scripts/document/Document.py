from csp.base.signals import Signal

class Document:
	"""Base class for all documents that can be loaded
	within this editor."""

	def __init__(self, name):
		self.name = name
		self.changedSignal = Signal()

	def GetName(self):
		"""Returns the name of the document."""
		return self.name

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
