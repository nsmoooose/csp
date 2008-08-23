#!/usr/bin/env python
import wx

from csp.tools.layout2.scripts.document.ImageDocument import ImageDocument
from csp.tools.layout2.scripts.document.OutputDocument import OutputDocument
from csp.tools.layout2.scripts.document.ModelDocument import ModelDocument
from csp.tools.layout2.scripts.document.SceneDocument import SceneDocument
from ImageWindow import ImageWindow
from ModelWindow import ModelWindow
from OutputWindow import OutputWindow
from SceneWindow import SceneWindow

class DocumentNotebook(wx.Notebook):
	"""This class handles the UI of all opened documents.
	
	Opened documents is displayed in a notebook (tab control)
	where each page is a single document.
	
	This class is also monitoring the following events in 
	order to keep the UI consistent:
		DocumentAdded, DocumentClosed"""

	def __init__(self, parent, id):
		wx.Notebook.__init__(self, parent, id)
		
		self.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGED, self.on_PageChanged)
		
		application = wx.GetApp()
		
		# Attach ourselves to the document registry and all the 
		# events that it provides.
		documentRegistry = application.GetDocumentRegistry()
		documentRegistry.GetDocumentAddedSignal().Connect(self.documentAdded_Signal)
		documentRegistry.GetDocumentClosedSignal().Connect(self.documentClosed_Signal)
		documentRegistry.GetCurrentDocumentChangedSignal().Connect(self.currentDocumentChanged_Signal)
		
		# For all existing documents that has already been loaded we
		# need to create visible pages for them.
		for document in documentRegistry.GetDocuments():
			self.documentAdded_Signal(document)
			
	def on_PageChanged(self, event):
		pass
		
	def documentAdded_Signal(self, document):
		# Depending on instance type of the document we choose
		# different control to display the document.
	
		if isinstance(document, SceneDocument):
			# Add the scene control that is responsible for the 3D
			# scene.
			page = SceneWindow(self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.BORDER_NONE, "GLCanvas", 0, wx.NullPalette)
			page.SetDocument(document)
			self.AddPage(page, document.GetName(), True)
		elif isinstance(document, OutputDocument):
			# Add simple text output window to handle any kind of 
			# tool output.
			page = OutputWindow(self, wx.ID_ANY)
			page.SetDocument(document)
			self.AddPage(page, document.GetName(), True)
		elif isinstance(document, ModelDocument):
			page = ModelWindow(self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.BORDER_NONE, "GLCanvas", 0, wx.NullPalette)
			page.SetDocument(document)
			self.AddPage(page, document.GetName(), True)
		elif isinstance(document, ImageDocument):
			page = ImageWindow(self, wx.ID_ANY)
			page.SetDocument(document)
			self.AddPage(page, document.GetName(), True)
		
	def documentClosed_Signal(self, document):
		# We need to remove the page that is responsible for
		# the closed document.
		pageCount = self.GetPageCount()
		
		# Make sure that we closes the page that is assigned the
		# document.
		for pageIndex in range(pageCount):
			page = self.GetPage(pageIndex)
			if page.GetDocument() == document:
				self.RemovePage(pageIndex)
				break
		
	def currentDocumentChanged_Signal(self, document):
		# A new document is set to be the current active document.
		# Lets change current active page.
		pageCount = self.GetPageCount()
		
		# Make sure that we closes the page that is assigned the
		# document.
		for pageIndex in range(pageCount):
			page = self.GetPage(pageIndex)
			if page.GetDocument() == document:
				self.ChangeSelection(pageIndex)
				break
