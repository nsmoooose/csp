from .FileDocument import FileDocument

class ImageDocument(FileDocument):
	"""Document representing a single Image. """


class ImageDocumentFactory():
	def __init__(self, fileName):
		self.fileName = fileName
	
	def GetUniqueId(self):
		"""Returns a unique Id identifying the document in the DocumentRegistry."""
		return ImageDocument.MakeUniqueId(self.fileName)
	
	def CreateDocument(self):
		"""Returns a new document that will be added in the DocumentRegistry."""
		return ImageDocument(self.fileName)
