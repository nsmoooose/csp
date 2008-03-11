from csp.base.signals import Signal

class Document:
	"""Base class for all documents that can be loaded
	within this editor."""

	def __init__(self, name):
		self.name = name
		self.changedSignal = Signal()

	def GetName(self):
		"""Returns the name of the document."""
		return self.name

	def GetChangedSignal(self):
		return self.changedSignal

	def Dispose(self):
		self.changedSignal.Dispose()
		self.changedSignal = None