from Command import Command

class DirectoryCommand(Command):
	"""Base class for all command objects that involves a directory."""

	def SetDirectory(self, directory):
		self.directory = directory

	def GetDirectory(self):
		return self.directory
