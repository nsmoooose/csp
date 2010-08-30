import wx

from Command import Command

class SaveCurrentDocumentCommand(Command):
    """Saves the current document if it allows it."""

    def GetCaption(self):
        return "Save file"

    def GetToolTipText(self):
        return "Save file"

    def GetToolBarImageName(self):
        return "document-save"

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

