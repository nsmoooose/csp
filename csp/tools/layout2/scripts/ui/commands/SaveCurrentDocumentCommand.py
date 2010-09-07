import wx

from Command import Command

class SaveCurrentDocumentCommand(Command):
    """Saves the current document if it allows it."""

    caption = "Save file"
    tooltip = "Save file"
    toolbarimage = "document-save"

    def Execute(self):
	application = wx.GetApp()

	# Get the registry of documents. This class holds a
	# reference to the current document that has focus.
	documentRegistry = application.GetDocumentRegistry()

	# Retreive the current document. There may not be any
	# opened documents. In that case we cannot continue.
	currentDocument = documentRegistry.GetCurrentDocument()
	if currentDocument is not None and currentDocument.IsReadOnly() == False:
		currentDocument.Save()

    @staticmethod
    def Enabled():
	application = wx.GetApp()
	documentRegistry = application.GetDocumentRegistry()
	return False if documentRegistry.GetCurrentDocument() is None else True


