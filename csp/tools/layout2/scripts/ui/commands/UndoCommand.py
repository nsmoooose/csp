import wx

from Command import Command

class UndoCommand(Command):
	"""Undo the last action."""

	caption = "Undo"
	tooltip = "Undo the last action"
	toolbarimage = "edit-undo"

	def Execute(self):
		documentRegistry = wx.GetApp().GetDocumentRegistry()
		currentDocument = documentRegistry.GetCurrentDocument()
		currentDocument.actionHistory.Undo()

	@staticmethod
	def Enabled():
		documentRegistry = wx.GetApp().GetDocumentRegistry()
		currentDocument = documentRegistry.GetCurrentDocument()
		if currentDocument is None:
			return False

		return True if currentDocument.actionHistory.CanUndo() else False

