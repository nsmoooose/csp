#!/usr/bin/env python
import wx
import wx.glcanvas

from csp.tools.layout2.layout_module import *

class OutputWindow(wx.Window):
	"""This window holds a multi line text box that displays
	a log of events from any process that is sending message
	signals. For example when we are compiling a new sim.dar
	file."""

	def __init__(self, *args, **kwargs):
		wx.Window.__init__(self, *args, **kwargs)
		self.textCtrl = wx.TextCtrl(self, style = wx.BORDER_NONE | wx.TE_MULTILINE | wx.TE_READONLY | wx.TE_DONTWRAP)
		self.Bind(wx.EVT_SIZE, self.on_Size)
		self.outputDocument = None
		self.nbLines = 0

	def Dispose(self):
		self.outputDocument.GetChangedSignal().Disconnect(self.on_DocumentChanged)
	
	def SetDocument(self, outputDocument):
		if outputDocument is None:
			return

		# Store a reference to the outputDocument for later use.
		self.outputDocument = outputDocument
		self.outputDocument.GetChangedSignal().Connect(self.on_DocumentChanged)
		
		# Call the changed method in order to display the content
		# from the outputDocument.
		self.textCtrl.Clear()
		self.nbLines = 0
		self.on_DocumentChanged(outputDocument)
	
	def GetDocument(self):
		return self.outputDocument

	def on_Size(self, event):
		width, height = self.GetClientSizeTuple()
		self.textCtrl.SetSize(wx.Size(width, height))

	def on_DocumentChanged(self, outputDocument):
		lines = outputDocument.GetLines()
		newNbLines = len( lines )
		
		if self.nbLines > newNbLines:
			# the outputDocument has been cleared
			self.textCtrl.Clear()
			self.nbLines = 0
		
		for line in lines[self.nbLines:]:
			self.textCtrl.AppendText(line + '\n')
		
		self.nbLines = newNbLines
