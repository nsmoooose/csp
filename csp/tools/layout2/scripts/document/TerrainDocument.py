#!/usr/bin/env python

from Document import Document

class TerrainDocument(Document):
	"""Document representing a terrain."""

	def SetFileName(self, fileName):
		self._fileName = fileName

	def GetFileName(self):
		return self._fileName
