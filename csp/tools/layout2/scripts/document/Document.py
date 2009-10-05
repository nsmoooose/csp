from csp.base.signals import Signal

class Document:
	"""Base class for all documents that can be loaded
	within this editor."""

	def __init__(self, name):
		self.name = name
		self.changedSignal = Signal()
		
		# refCount indicates how many objects are referencing this document.
		# It is managed by the DocumentRegistry
		self.refCount = 0
		
		# List all documents that refer to this document.
		# It is managed by the DocumentRegistry
		self.referrers = []

	def Dispose(self):
		self.changedSignal.Dispose()
		self.changedSignal = None
		self.referrers = []

	def GetName(self):
		"""Returns the name of the document."""
		return self.name

	def GetUniqueId(self):
		"""Returns a unique Id identifying the document in the DocumentRegistry.
		Inheriting classes should implement this method"""
		return "Document:" + self.GetName()
	
	def GetRefCount(self):
		"""This methode should not be used to modify self.refCount."""
		return self.refCount
	
	def incrementRefCount(self):
		"""This methode should only be called by DocumentRegistry."""
		self.refCount += 1
	
	def decrementRefCount(self):
		"""This methode should only be called by DocumentRegistry."""
		self.refCount -= 1
		return self.refCount

	def GetReferrers(self):
		"""This methode should not be used to modify self.referrers."""
		return self.referrers

	def GetAllReferrers( self, allReferrers = None ):
		if allReferrers is None:
			allReferrers = set()
		for referrer in self.referrers:
			if referrer not in allReferrers:
				allReferrers.add( referrer )
				referrer.GetAllReferrers( allReferrers )
		return allReferrers
	
	def GetChangedSignal(self):
		return self.changedSignal
	
	def EmitChangedSignal(self):
		self.changedSignal.Emit(self)
	
	def IsReadOnly(self):
		"""Tests to see if the current document is read only or not."""
		return True

	def PostCreate(self):
		"""Called by DocumentRegistry after the document has been created and added to DocumentRegistry.
		Inheriting classes should implement this method if they need to create dependent sub-documents."""
		pass
	
	def Save(self):
		"""Saves the document. Inheriting classes should implement this
		method."""
		pass
