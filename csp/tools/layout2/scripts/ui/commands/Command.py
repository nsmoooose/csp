#!/usr/bin/env python

class Command:
	"""Base class for all command objects. Implement
	apropriate methods in your derived class to execute
	this command."""
	
	def __init__(self):
		self.outputDocument = None

	def GetCaption(self):
		"""The name of the command to be displayed in a menu
		for example."""
		return ""

	def GetToolTipText(self):
		"""Returns a text to use when the mouse is hoovering 
		over the command button in a toolbar."""
		return ""

	def GetToolBarImageName(self):
		"""Returns the name of the image file for this command.
		The name will be used to load the file into a
		toolbar button."""
		return None

	def Execute(self):
		pass

	def SetOutputDocument(self, outputDocument):
		self.outputDocument = outputDocument
		
	def GetOutputDocument(self):
		return self.outputDocument