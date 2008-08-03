#!/usr/bin/env python

from Document import Document

class ImageDocument(Document):
	"""Document representing a single Image. """

	def SetFileName(self, fileName):
		self._fileName = fileName

	def GetFileName(self):
		return self._fileName
