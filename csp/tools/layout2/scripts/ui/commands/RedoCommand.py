import wx

from .Command import Command

class RedoCommand(Command):
    """Redo the last action."""

    caption = "Redo"
    tooltip = "Redo the last action"
    toolbarimage = "edit-redo"

    def Execute(self):
        documentRegistry = wx.GetApp().GetDocumentRegistry()
        currentDocument = documentRegistry.GetCurrentDocument()
        currentDocument.actionHistory.Redo()

    @staticmethod
    def Enabled():
        documentRegistry = wx.GetApp().GetDocumentRegistry()
        currentDocument = documentRegistry.GetCurrentDocument()
        if currentDocument is None:
            return False

        return True if currentDocument.actionHistory.CanRedo() else False

