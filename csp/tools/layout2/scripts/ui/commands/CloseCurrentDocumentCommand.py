#!/usr/bin/env python
import wx

from Command import Command

class CloseCurrentDocumentCommand(Command):
	"""The purpose of this command is to close the currently
	edited document."""

	def GetCaption(self):
		return "Close"

	def GetToolTipText(self):
		return "Close current document"

	def GetToolBarImageName(self):
		return "generic.png"

	def Execute(self):
		application = wx.GetApp()

		# Get the registry of documents. This class holds a
		# reference to the current document that has focus.
		documentRegistry = application.GetDocumentRegistry()

		# Retreive the current document. There may not be any
		# opened documents. In that case we cannot continue.
		currentDocument = documentRegistry.GetCurrentDocument()
		if currentDocument is not None:
			documentRegistry.Close(currentDocument)
