from csp.base.signals import Signal

class BadUniqueIdError(Exception):
	pass

class DocumentRegistry:
	"""This class represents all opened documents. It can be
	any loaded document like a text file or an hierchy of 
	xml files for the layout editor.
	
	To retreive the instance of this class you should call
	GetDocumentRegistry() method on the wx application object"""
	
	def __init__(self):
		self.documents = {}
		self.currentDocument = None
		self.currentDocumentChangedSignal = Signal()

	def GetCurrentDocumentChangedSignal(self):
		"""Whenever the current document is replaced by another
		this signal is emitted to all listeners. The document
		is attached to the signal as the first argument."""
		return self.currentDocumentChangedSignal

	def GetOrCreateDocument(self, documentFactory):
		"""Get a document based on its uniqueId.
		If the document is not found, create it and add it to the list of opened documents.
		Increment the refCount of the document before returning it.
		The returned document must be released by a corresponded call to ReleaseDocument."""
		
		uniqueId = documentFactory.GetUniqueId()
		document = self.documents.get(uniqueId)
		
		if document is None:
			document = documentFactory.CreateDocument()
			if document.GetUniqueId() != uniqueId:
				raise BadUniqueIdError()
			self.documents[uniqueId] = document
		
		document.incrementRefCount()
		return document

	def ReferenceDocument(self, document):
		"""Increment the refCount of the document."""
		document.incrementRefCount()

	def ReleaseDocument(self, document):
		"""Decrement the refCount of the document.
		If the document is no more referenced, remove it from the list of opened documents,
		and dispose of it."""
		
		if document.decrementRefCount() == 0:
			del self.documents[ document.GetUniqueId() ]
			document.Dispose()
	
	def SetCurrentDocument(self, document):
		"""Changes the current document to the specified one."""

		# Make sure that there is a document change at all. Compare 
		# with the existing document.
		if document is self.currentDocument:
			return

		self.currentDocument = document
		self.currentDocumentChangedSignal.Emit(document)

	def GetCurrentDocument(self):
		return self.currentDocument

	def GetByUniqueId(self, uniqueId):
		"""Returns the document with the specified uniqueId. If it
		doesn't exist None is returned."""
		
		return self.documents.get(uniqueId)

	def GetDocuments(self):
		return self.documents
