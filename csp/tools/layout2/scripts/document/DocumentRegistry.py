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
		# The list of opened documents
		self.documents = {}

		# The current document (the document that is managed by the selected page)
		self.currentDocument = None
		self.currentDocumentChangedSignal = Signal()

		# The active document (the actual document that is currently edited.
		# This can be a sub-document of the current document, or the current document itself)
		self.activeDocument = None
		self.activeDocumentChangedSignal = Signal()

		self.documentAddedSignal = Signal()
		self.documentRemovedSignal = Signal()

		# Protection to avoid recursion when removing a document member of a
		# dependency cycle
		self.releasesInProgress = set()

	def GetCurrentDocumentChangedSignal(self):
		"""Whenever the current document is replaced by another
		this signal is emitted to all listeners. The document
		is attached to the signal as the first argument."""
		return self.currentDocumentChangedSignal

	def GetActiveDocumentChangedSignal(self):
		"""Whenever the active document is replaced by another
		this signal is emitted to all listeners. The document
		is attached to the signal as the first argument."""
		return self.activeDocumentChangedSignal

	def GetDocumentRemovedSignal(self):
		return self.documentRemovedSignal

	def GetDocumentAddedSignal(self):
		return self.documentAddedSignal

	def GetOrCreateDocument(self, documentFactory, documentReferrer = None):
		"""Get a document based on its uniqueId.
		If the document is not found, create it and add it to the list of opened documents.
		Increment the refCount of the document before returning it.
		The returned document must be released by a corresponded call to ReleaseDocument.
		If the owner of the returned document is another document, documentReferrer
		must be this other document, otherwise documentReferrer must be None."""

		uniqueId = documentFactory.GetUniqueId()
		document = self.documents.get(uniqueId)

		doPostCreate = False
		if document is None:
			document = documentFactory.CreateDocument()
			if document.GetUniqueId() != uniqueId:
				raise BadUniqueIdError()
			self.documents[uniqueId] = document
			self.documentAddedSignal.Emit(document)
			doPostCreate = True

		document.incrementRefCount()
		if documentReferrer is not None:
			document.referrers.append( documentReferrer )

		if doPostCreate:
			document.PostCreate()

		return document

	def ReferenceDocument(self, document, documentReferrer = None):
		"""Increment the refCount of the document.
		If the owner of the document is another document, documentReferrer
		must be this other document, otherwise documentReferrer must be None."""

		document.incrementRefCount()
		if documentReferrer is not None:
			document.referrers.append( documentReferrer )

	def ReleaseDocument(self, document, documentReferrer = None):
		"""Decrement the refCount of the document.
		If the document is no more referenced, remove it from the list of opened documents,
		and dispose of it.
		If the owner of the document is another document, documentReferrer
		must be this other document, otherwise documentReferrer must be None."""

		if document in self.releasesInProgress:
			# Protect from recursion when removing a document member of a 
			# dependency cycle
			return

		refCount = document.decrementRefCount()
		if documentReferrer is not None:
			document.referrers.remove( documentReferrer )

		# Find all orphans documents
		orphanDocuments = []

		if refCount == 0:
			orphanDocuments.append( document )
		else:
			dependencyCycle, orphan = self.GetDependencyCycle( document )
			if orphan:
				orphanDocuments.extend( dependencyCycle )

		# Dispose of all orphans documents
		if orphanDocuments:
			self.releasesInProgress.update( orphanDocuments )

			for documentToDispose in orphanDocuments:
				self.documentRemovedSignal.Emit(documentToDispose)
				del self.documents[ documentToDispose.GetUniqueId() ]
				documentToDispose.Dispose()

			self.releasesInProgress.difference_update( orphanDocuments )

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
	
	def SetActiveDocument(self, document):
		"""Changes the active document to the specified one."""

		# Make sure that there is a document change at all. Compare 
		# with the existing document.
		if document is self.activeDocument:
			return

		self.activeDocument = document
		self.activeDocumentChangedSignal.Emit(document)

	def GetActiveDocument(self):
		return self.activeDocument

	def GetByUniqueId(self, uniqueId):
		"""Returns the document with the specified uniqueId. If it
		doesn't exist None is returned."""

		return self.documents.get(uniqueId)

	def GetDocuments(self):
		return self.documents

	def GetDependencyCycle(self, document):
		documentStack = []
		visitedDocuments = {}
		dependencyCycle = self.TarjanAlgorithm(document, 0, documentStack, visitedDocuments)

		orphan = True
		visitedDocuments = {}
		for dependency in dependencyCycle:
			visitedDocuments[dependency] = 0

			for referrer in dependency.referrers:
				if referrer in dependencyCycle:
					visitedDocuments[dependency] += 1

			if visitedDocuments[dependency] != dependency.GetRefCount():
				orphan = False
				break

		return dependencyCycle, orphan

	def TarjanAlgorithm(self, document, currentIndex, documentStack, visitedDocuments):
		"""Apply the Tarjan algorithm on document.
		See http://en.wikipedia.org/wiki/Tarjan%E2%80%99s_strongly_connected_components_algorithm

		Return the list of documents that form the dependency cycle that contains document."""
		
		dependencyCycle = []
		
		visitedDocumentData = {'index': currentIndex, 'lowlink': currentIndex}
		visitedDocuments[document] = visitedDocumentData
		currentIndex += 1
		documentStack.append( document )
		
		for referrer in document.referrers:
			if referrer not in visitedDocuments:
				self.TarjanAlgorithm(referrer, currentIndex, documentStack, visitedDocuments)
				visitedDocumentData['lowlink'] = min( visitedDocumentData['lowlink'], visitedDocuments[referrer]['lowlink'] )
			elif referrer in documentStack:
				visitedDocumentData['lowlink'] = min( visitedDocumentData['lowlink'], visitedDocuments[referrer]['index'] )
		
		if visitedDocumentData['lowlink'] == visitedDocumentData['index']:
			while True:
				documentInCycle = documentStack.pop()
				dependencyCycle.append( documentInCycle )
				if documentInCycle is document:
					break
		
		return dependencyCycle
