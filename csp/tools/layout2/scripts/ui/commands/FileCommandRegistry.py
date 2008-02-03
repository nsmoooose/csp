#!/usr/bin/env python
import os

from OpenCustomLayoutModelFileCommand import OpenCustomLayoutModelFileCommand

class FileCommandRegistry:
	"""This is responsible for knowing what Command to execute
	on different files. It is also responsible for identification
	of files in order to choose the correct Command."""

	def GetCommandForFile(self, fileName):
		"""Returns a Command to use for the file. If no command
		can be found for that file type we return None."""
		if os.path.isfile(fileName) == False:
			return None

		# Get the file name extension in lowercase.
		command = None
		extension = os.path.splitext(fileName)[1].lower()
		if extension == '.xml':
			command = OpenCustomLayoutModelFileCommand()

		# If we have found a command we set the fileName on it
		# and returns it.
		if command is not None:
			command.SetFileName(fileName)
			return command

		return None

