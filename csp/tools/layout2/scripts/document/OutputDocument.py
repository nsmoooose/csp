from Document import Document

class OutputDocument(Document):
	"""This document represents a console like output
	document where simple text lines is written. This
	document is usually displayed in a read only text 
	ctrl."""

	def __init__(self, name):
		Document.__init__(self, name)
		self.lines = []

	def WriteLine(self, text):
		self.lines.append(text)
		self.GetChangedSignal().Emit(self)

	def GetLines(self):
		return self.lines

	def Clear(self):
		self.lines = []
		self.GetChangedSignal().Emit(self)