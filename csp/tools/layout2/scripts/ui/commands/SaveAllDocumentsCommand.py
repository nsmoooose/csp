#!/usr/bin/env python
import wx

from Command import Command

class SaveAllDocumentsCommand(Command):
    """Saves all opened documents."""

    def GetCaption(self):
        return "Save all files"

    def GetToolTipText(self):
        return "Save all files"

    def GetToolBarImageName(self):
        return "document-save.png"

    def Execute(self):
	application = wx.GetApp()

	# Get the registry of documents. This class holds a
	# reference to the current document that has focus.
	documentRegistry = application.GetDocumentRegistry()

	# Iterate each document and try to save every document
	# that isn't read only.
	for document in documentRegistry.GetDocuments():
		if document.IsReadOnly():
			continue
		document.Save()

