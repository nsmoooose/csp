#!/usr/bin/env python

class FileCommand:
	"""Base class for all command objects that involves files."""

	def SetFileName(self, fileName):
		self.fileName = fileName
		
	def GetFileName(self):
		return self.fileName
