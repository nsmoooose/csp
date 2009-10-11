import wx

from ActionHistoryCommand import ActionHistoryCommand

class RedoCommand(ActionHistoryCommand):
	"""Redo the last action."""
	
	def __init__(self):
		ActionHistoryCommand.__init__(self)
	
	def GetCaption(self):
		return "Redo"
	
	def GetToolTipText(self):
		return "Redo the last action"
	
	def GetToolBarImageName(self):
		return "edit-redo"
	
	def on_ActionHistoryChanged(self, actionHistory):
		if actionHistory.CanRedo():
			self.Enable(True)
		else:
			self.Enable(False)
	
	def Execute(self):
		self.actionHistory.Redo()
