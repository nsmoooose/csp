import wx

from ActionHistoryCommand import ActionHistoryCommand

class UndoCommand(ActionHistoryCommand):
	"""Undo the last action."""
	
	def __init__(self):
		ActionHistoryCommand.__init__(self)
	
	def GetCaption(self):
		return "Undo"
	
	def GetToolTipText(self):
		return "Undo the last action"
	
	def GetToolBarImageName(self):
		return "edit-undo"
	
	def on_ActionHistoryChanged(self, actionHistory):
		if actionHistory.CanUndo():
			self.Enable(True)
		else:
			self.Enable(False)
	
	def Execute(self):
		self.actionHistory.Undo()
