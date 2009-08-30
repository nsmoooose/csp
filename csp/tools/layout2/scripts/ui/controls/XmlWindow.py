#!/usr/bin/env python
import wx
import wx.stc

class XmlWindow(wx.stc.StyledTextCtrl):
	"""This window holds a read only StyledTextCtrl. It is used to display
	an xml document."""
	
	def __init__(self, *args, **kwargs):
		wx.stc.StyledTextCtrl.__init__(self, *args, **kwargs)
		
		self.document = None
		self.SetReadOnly(1)
	
	def Dispose(self):
		self.document.GetChangedSignal().Disconnect(self.on_DocumentChanged)
		documentRegistry = wx.GetApp().GetDocumentRegistry()
		documentRegistry.ReleaseDocument(self.document)
		self.document = None
	
	def SetDocument(self, document):
		# Store a reference to the document for later use.
		self.document = document
		self.document.GetChangedSignal().Connect(self.on_DocumentChanged)
		
		# Call the changed method in order to display the content
		# from the document.
		self.on_DocumentChanged(document)
	
	def GetDocument(self):
		return self.document
	
	def on_DocumentChanged(self, document):
		fullText = document.xmlNodeDocument.domNode.toprettyxml()
		
		# Remove empty lines
		lines = []
		for line in fullText.splitlines():
			if line.strip():
				lines.append( line )
		fullText = "\n".join( lines )
		
		self.SetReadOnly(0)
		self.SetText(fullText)
		self.SetReadOnly(1)
