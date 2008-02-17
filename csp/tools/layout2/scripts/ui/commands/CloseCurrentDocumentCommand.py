#!/usr/bin/env python
import wx

from Command import Command

class CloseCurrentDocumentCommand(Command):

	def GetCaption(self):
		return "Close"

	def GetToolTipText(self):
		return "Close current document"

	def GetToolBarImageName(self):
		return "quit.png"

	def Execute(self):
		application = wx.GetApp()
		topWindow = application.GetTopWindow()
		if topWindow != None:
			topWindow.Close()


