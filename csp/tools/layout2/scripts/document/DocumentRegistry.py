from csp.base.signals import Signal

class DocumentRegistry:
	"""This class represents all opened documents. It can be
	any loaded document like a text file or an hierchy of 
	xml files for the layout editor.
	
	To retreive the instance of this class you should call
	GetDocumentRegistry() method on the wx application object"""
	
	def __init__(self):
		self.documents = []
		self.currentDocument = None
		self.documentAddedSignal = Signal()
		self.documentClosedSignal = Signal()
		self.currentDocumentChangedSignal = Signal()

	def GetDocumentAddedSignal(self):
		"""Whenever a document is added this signal is emitted
		to all listeners. The document is attached to the signal
		as the first argument."""
		return self.documentAddedSignal

	def GetDocumentClosedSignal(self):
		"""Whenever a document is closed this signal is emitted
		to all listeners. The document is attached to the signal
		as the first argument."""
		return self.documentClosedSignal
		
	def GetCurrentDocumentChangedSignal(self):
		return self.currentDocumentChangedSignal

	def Add(self, document):
		"""Adds a document to the list of opened documents."""
		self.documents.append(document)
		self.documentAddedSignal.Emit(document)
		
		# If this is the first document we are adding. Lets make it 
		# the default one too.
		if len(self.documents) == 1:
			self.SetCurrentDocument(document)

	def Close(self, document):
		"""Closes the specified document and removes it from
		the list of opened documents."""
		self.documents.remove(document)
		self.documentClosedSignal.Emit(document)
		
		if len(self.documents) == 0:
			self.SetCurrentDocument(None)
		elif document == self.currentDocument:
			self.SetCurrentDocument(self.documents[0])
			
		# No further use of this document since it has been closed.
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

	def GetByName(self, name):
		"""Returns the document with the specified name. If it
		doesn't exist None is returned."""
		for document in self.documents:
			if document.GetName() == name:
				return document
		return None

	def GetDocuments(self):
		return self.documents
