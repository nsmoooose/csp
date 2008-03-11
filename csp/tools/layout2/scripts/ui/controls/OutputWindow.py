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
		self.document = None

	def SetDocument(self, document):
		if document is None:
			return

		# Store a reference to the document for later use.
		self.document = document
		self.document.GetChangedSignal().Connect(self.on_DocumentChanged)
		
		# Call the changed method in order to display the content
		# from the document.
		self.on_DocumentChanged(document)
	
	def GetDocument(self):
		return self.document

	def on_Size(self, event):
		width, height = self.GetClientSizeTuple()
		self.textCtrl.SetSize(wx.Size(width, height))

	def on_DocumentChanged(self, document):
		self.textCtrl.SetValue(document.GetText())
		self.textCtrl.SetInsertionPointEnd()