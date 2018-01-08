import os.path

from .Document import Document

class FileDocument(Document):
	"""Base class for all documents representing a single file."""

	def __init__(self, fileName, name = None):
		if name is None:
			name = os.path.basename(fileName)
		Document.__init__(self, name)
		
		self._fileName = fileName

	@staticmethod
	def MakeUniqueId(fileName):
		return "FileDocument:" + fileName

	def GetUniqueId(self):
		"""Returns a unique Id identifying the document in the DocumentRegistry.
		Inheriting classes should implement this method"""
		return FileDocument.MakeUniqueId( self.GetFileName() )

	def GetFileName(self):
		return self._fileName
