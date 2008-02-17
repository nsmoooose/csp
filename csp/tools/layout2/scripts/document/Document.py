
class DocumentSignal:
	def __init__(self):
		self.slots = []
	
	def Connect(self, slot):
		self.slots.append(slot)
		
	def Emit(self, document):
		for slot in self.slots:
			slot(document)

class Document:
	"""Base class for all documents that can be loaded
	within this editor."""

	def __init__(self, name):
		self.name = name
		self.changedSignal = DocumentSignal()

	def GetName(self):
		"""Returns the name of the document."""
		return self.name

	def GetChangedSignal(self):
		return self.changedSignal
