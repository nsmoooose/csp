from .Document import Document

class OutputDocument(Document):
	"""This document represents a console like output
	document where simple text lines is written. This
	document is usually displayed in a read only text 
	ctrl."""

	def __init__(self, name):
		Document.__init__(self, name)
		self.lines = []

	@staticmethod
	def MakeUniqueId(name):
		return "OutputDocument:" + name
	
	def GetUniqueId(self):
		"""Returns a unique Id identifying the document in the DocumentRegistry.
		Inheriting classes should implement this method"""
		return OutputDocument.MakeUniqueId( self.GetName() )

	def WriteLine(self, text):
		self.lines.append(text)
		self.EmitChangedSignal()

	def GetLines(self):
		return self.lines

	def Clear(self):
		self.lines = []
		self.EmitChangedSignal()


class OutputDocumentFactory():
	def __init__(self, documentName):
		self.documentName = documentName
	
	def GetUniqueId(self):
		"""Returns a unique Id identifying the document in the DocumentRegistry."""
		return OutputDocument.MakeUniqueId(self.documentName)
	
	def CreateDocument(self):
		"""Returns a new document that will be added in the DocumentRegistry."""
		return OutputDocument(self.documentName)
