import os.path

class Command(object):
	"""Base class for all command objects. Implement
	apropriate methods in your derived class to execute
	this command."""

	caption = "No caption set"
	tooltip = "No tooltip set"
	toolbarimage = ""

	def Execute(self):
		pass

	@staticmethod
	def Enabled():
		"""Checks if the command can be executed or not. Returns True if it is ok
		to construct and execute a command."""
		return True
