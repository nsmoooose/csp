#!/usr/bin/env python

from Command import Command

class FileCommand(Command):
	"""Base class for all command objects that involves files."""

	def SetFileName(self, fileName):
		self.fileName = fileName

	def GetFileName(self):
		return self.fileName
