#!/usr/bin/env python
import wx
import wx.glcanvas

from csp.tools.layout2.layout_module import *

class OutputWindow(wx.Window):
	"""This window holds a multi line text box that displays
	a log of events from any process that is sending message
	signals. For example when we are compiling a new sim.dar
	file."""

	def __init__(self, parent, id):
		wx.Window.__init__(self, parent, id)
		self.textCtrl = wx.TextCtrl(self, wx.ID_ANY, style = wx.TE_MULTILINE|wx.TE_READONLY|wx.TE_DONTWRAP)
		self.Bind(wx.EVT_SIZE, self.on_Size)

	def ConnectToDocument(self, document):
		if document is None:
			return
		document.GetChangedSignal().Connect(self.on_DocumentChanged)

	def on_Size(self, event):
		width, height = self.GetClientSizeTuple()
		self.textCtrl.SetSize(wx.Size(width, height))

	def on_DocumentChanged(self, document):
		self.textCtrl.SetValue(document.GetText())
		self.textCtrl.SetInsertionPointEnd()