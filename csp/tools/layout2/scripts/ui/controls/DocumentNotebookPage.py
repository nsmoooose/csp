import wx


class DocumentNotebookPage(object):
    """Base class for all windows added to the DocumentNotebook."""

    def GetDocument(self):
        return None

    def on_PageUnselected(self):
        pass

    def on_PageSelected(self):
        documentRegistry = wx.GetApp().GetDocumentRegistry()
        documentRegistry.SetCurrentDocument(self.GetDocument())
        documentRegistry.SetActiveDocument(self.GetDocument())
