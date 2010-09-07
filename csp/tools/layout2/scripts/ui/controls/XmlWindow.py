import wx
import wx.stc

from DocumentNotebook import DocumentNotebook
from DocumentNotebookPage import DocumentNotebookPage

class XmlWindow(wx.stc.StyledTextCtrl, DocumentNotebookPage):
	"""This window holds a read only StyledTextCtrl. It is used to display
	an xml document."""
	
	def __init__(self, *args, **kwargs):
		wx.stc.StyledTextCtrl.__init__(self, *args, **kwargs)
		
		# The main document opened in this window
		self.document = None
		
		# The active document (the actual document that is currently edited.
		# This can be a sub-document of the main document, or the main document itself)
		self.activeDocument = None
		
		# Can be used by any object to store private data.
		# Each object must use a unique key.
		self.externalData = {}
		
		self.SetReadOnly(1)
		
		documentRegistry = wx.GetApp().GetDocumentRegistry()
		documentRegistry.GetActiveDocumentChangedSignal().Connect(self.on_ActiveDocumentChanged)
	
	def Dispose(self):
		documentRegistry = wx.GetApp().GetDocumentRegistry()
		documentRegistry.GetActiveDocumentChangedSignal().Disconnect(self.on_ActiveDocumentChanged)
		self.document.GetChangedSignal().Disconnect(self.on_DocumentChanged)
		documentRegistry.ReleaseDocument(self.document)
		self.document = None
		self.externalData = {}
	
	def SetDocument(self, document):
		# Store a reference to the document for later use.
		self.document = document
		self.document.GetChangedSignal().Connect(self.on_DocumentChanged)
		
		self.activeDocument = self.document
		
		# Call the changed method in order to display the content
		# from the document.
		self.on_DocumentChanged(document)
	
	def GetDocument(self):
		return self.document
	
	def on_PageSelected(self):
		documentRegistry = wx.GetApp().GetDocumentRegistry()
		documentRegistry.SetCurrentDocument(self.document)
		documentRegistry.SetActiveDocument(self.activeDocument)
	
	def on_ActiveDocumentChanged(self, document):
		if DocumentNotebook.Instance.GetCurrentPage() is self:
			self.activeDocument = document
	
	def on_DocumentChanged(self, document):
		fullText = document.GetXmlNodeDocument().domNode.toprettyxml()
		
		# Remove empty lines
		lines = []
		for line in fullText.splitlines():
			if line.strip():
				lines.append( line )
		fullText = "\n".join( lines )
		
		self.SetReadOnly(0)
		self.SetText(fullText)
		self.SetReadOnly(1)
