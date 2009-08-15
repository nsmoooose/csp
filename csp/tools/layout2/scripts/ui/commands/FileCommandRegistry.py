#!/usr/bin/env python
import os

from OpenXmlFileCommand import OpenXmlFileCommand
from OpenImageFileCommand import OpenImageFileCommand
from OpenOsgModelFileCommand import OpenOsgModelFileCommand

class FileCommandRegistry:
	"""This singleton is responsible for knowing what Command to execute
	on different files. It is also responsible for identification
	of files in order to choose the correct Command."""
	
	Instance = None
	
	def __init__(self):
		self.fileCommands = {}
		self.RegisterCommandForFileExtensions( '.xml', OpenXmlFileCommand() )
		self.RegisterCommandForFileExtensions( ('.png', '.jpg', '.tga'), OpenImageFileCommand() )
		self.RegisterCommandForFileExtensions( ('.osg', '.ive'), OpenOsgModelFileCommand() )

	def RegisterCommandForFileExtensions(self, extensions, fileCommand):
		"""Called by FileCommands to register themselves"""
		
		if isinstance(extensions, str):
			extensions = [extensions]
			
		for extension in extensions:
			self.fileCommands[extension] = fileCommand
			print "RegisterCommandForFileExtensions( %s )" % extension
	
	def GetCommandForFile(self, fileName):
		"""Returns a Command to use for the file. If no command
		can be found for that file type we return None."""
		if os.path.isfile(fileName) == False:
			return None
		
		# Get the file name extension in lowercase.
		extension = os.path.splitext(fileName)[1].lower()
		command = self.fileCommands.get(extension)
		
		# If we have found a command we set the fileName on it
		# and returns it.
		if command is not None:
			command.SetFileName(fileName)
			return command
		
		return None

FileCommandRegistry.Instance = FileCommandRegistry()
