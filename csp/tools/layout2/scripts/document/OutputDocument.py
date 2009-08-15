from Document import Document

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
		self.GetChangedSignal().Emit(self)

	def GetLines(self):
		return self.lines

	def Clear(self):
		self.lines = []
		self.GetChangedSignal().Emit(self)