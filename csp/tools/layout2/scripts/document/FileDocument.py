#!/usr/bin/env python

from Document import Document

class FileDocument(Document):
	"""Base class for all documents representing a single file."""

	@staticmethod
	def MakeUniqueId(fileName):
		return "FileDocument:" + fileName

	def GetUniqueId(self):
		"""Returns a unique Id identifying the document in the DocumentRegistry.
		Inheriting classes should implement this method"""
		return FileDocument.MakeUniqueId( self.GetFileName() )

	def SetFileName(self, fileName):
		self._fileName = fileName

	def GetFileName(self):
		return self._fileName
