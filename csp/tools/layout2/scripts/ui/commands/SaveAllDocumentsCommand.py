import wx

from Command import Command

class SaveAllDocumentsCommand(Command):
    """Saves all opened documents."""

    caption = "Save all files"
    tooltip = "Save all files"
    toolbarimage = "document-save"

    def Execute(self):
	application = wx.GetApp()

	# Get the registry of documents. This class holds a
	# reference to the current document that has focus.
	documentRegistry = application.GetDocumentRegistry()

	# Iterate each document and try to save every document
	# that isn't read only.
	for document in documentRegistry.GetDocuments().itervalues():
		if document.IsReadOnly():
			continue
		document.Save()

    @staticmethod
    def Enabled():
	application = wx.GetApp()
	documentRegistry = application.GetDocumentRegistry()
        return len(documentRegistry.GetDocuments()) > 0

