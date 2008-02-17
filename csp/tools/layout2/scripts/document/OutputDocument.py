from Document import Document

class OutputDocument(Document):
	"""This document represents a console like output
	document where simple text lines is written. This
	document is usually displayed in a read only text 
	ctrl."""

	def __init__(self, name):
		Document.__init__(self, name)
		self.text = ''

	def WriteLine(self, text):
		self.text += '\n' + text
		self.GetChangedSignal().Emit(self)

	def GetText(self):
		return self.text

	def Clear(self):
		self.text = ''
		self.GetChangedSignal().Emit(self)