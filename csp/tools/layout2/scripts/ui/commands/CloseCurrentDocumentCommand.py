#!/usr/bin/env python
import wx

from Command import Command
from ..controls.DocumentNotebook import DocumentNotebook

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
		DocumentNotebook.Instance.CloseCurrentPage()
