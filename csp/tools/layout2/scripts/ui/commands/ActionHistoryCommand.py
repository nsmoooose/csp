import wx

from Command import Command

class ActionHistoryCommand(Command):
	"""Base class for command that manipulate action history."""
	
	def __init__(self):
		Command.__init__(self)
		
		self.actionHistory = None
		self.Enable(False)
		
		documentRegistry = wx.GetApp().GetDocumentRegistry()
		documentRegistry.GetActiveDocumentChangedSignal().Connect(self.on_ActiveDocumentChanged)
	
	def on_ActiveDocumentChanged(self, document):
		if self.actionHistory is not None:
			if self.actionHistory.GetChangedSignal() is not None:
				self.actionHistory.GetChangedSignal().Disconnect(self.on_ActionHistoryChanged)
		
		if document is None:
			self.actionHistory = None
			self.Enable(False)
		else:
			self.actionHistory = document.actionHistory
			self.actionHistory.GetChangedSignal().Connect(self.on_ActionHistoryChanged)
			self.on_ActionHistoryChanged(self.actionHistory)
	
	def on_ActionHistoryChanged(self, actionHistory):
		pass
