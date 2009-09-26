#!/usr/bin/env python

from Command import Command

class ReversibleCommand(Command):
	"""Base class for all command objects that can be undone."""
	
	def CanUndo(self):
		return True
	
	def Undo(self):
		pass
