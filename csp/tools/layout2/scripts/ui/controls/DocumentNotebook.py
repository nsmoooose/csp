#!/usr/bin/env python
import wx
import wx.aui

from csp.tools.layout2.scripts.document.ImageDocument import ImageDocument
from csp.tools.layout2.scripts.document.OutputDocument import OutputDocument
from csp.tools.layout2.scripts.document.ModelDocument import ModelDocument
from csp.tools.layout2.scripts.document.SceneDocument import SceneDocument
from csp.tools.layout2.scripts.document.TerrainDocument import TerrainDocument
from ImageWindow import ImageWindow
from ModelWindow import ModelWindow
from OutputWindow import OutputWindow
from SceneWindow import SceneWindow
from TerrainWindow import TerrainWindow

class DocumentNotebook(wx.aui.AuiNotebook):
	"""This class handles the UI of all opened documents.
	
	Opened documents is displayed in a notebook (tab control)
	where each page is a single document.
	
	This class is also monitoring the following events in 
	order to keep the UI consistent:
		DocumentAdded, DocumentClosed"""

	Instance = None

	def __init__(self, *args, **kwargs):
		wx.aui.AuiNotebook.__init__(self, style = wx.NO_BORDER | wx.aui.AUI_NB_DEFAULT_STYLE | wx.aui.AUI_NB_WINDOWLIST_BUTTON, *args, **kwargs)
		
		self.Bind(wx.aui.EVT_AUINOTEBOOK_PAGE_CHANGED, self.on_PageChanged)
		self.Bind(wx.aui.EVT_AUINOTEBOOK_PAGE_CLOSE, self.on_PageClose)
		
		self.pendingPageClose = False
		
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
		application = wx.GetApp()
		documentRegistry = application.GetDocumentRegistry()
		
		newSelection = event.GetSelection()
		newPage = self.GetPage(newSelection)
		documentRegistry.SetCurrentDocument(newPage.GetDocument())
	
	def on_PageClose(self, event):
		application = wx.GetApp()
		documentRegistry = application.GetDocumentRegistry()
		
		self.pendingPageClose = True
		currentSelection = event.GetSelection()
		currentPage = self.GetPage(currentSelection)
		currentPage.Dispose()
		documentRegistry.Close(currentPage.GetDocument())
		self.pendingPageClose = False
	
	def documentAdded_Signal(self, document):
		# Depending on instance type of the document we choose
		# different control to display the document.
		
		if isinstance(document, SceneDocument):
			# Add the scene control that is responsible for the 3D
			# scene.
			page = SceneWindow(self, style = wx.BORDER_NONE)
			page.SetDocument(document)
			self.AddPage(page, document.GetName())
		elif isinstance(document, OutputDocument):
			# Add simple text output window to handle any kind of 
			# tool output.
			page = OutputWindow(self, style = wx.BORDER_NONE)
			page.SetDocument(document)
			self.AddPage(page, document.GetName())
		elif isinstance(document, ModelDocument):
			page = ModelWindow(self, style = wx.BORDER_NONE)
			page.SetDocument(document)
			self.AddPage(page, document.GetName())
		elif isinstance(document, ImageDocument):
			page = ImageWindow(self, style = wx.BORDER_NONE)
			page.SetDocument(document)
			self.AddPage(page, document.GetName())
		elif isinstance(document, TerrainDocument):
			page = TerrainWindow(self, style = wx.BORDER_NONE)
			page.SetDocument(document)
			self.AddPage(page, document.GetName())
	
	def documentClosed_Signal(self, document):
		# We need to remove the page that is responsible for
		# the closed document.
		if self.pendingPageClose:
			return
		
		pageCount = self.GetPageCount()
		
		# Make sure that we closes the page that is assigned the
		# document.
		for pageIndex in range(pageCount):
			page = self.GetPage(pageIndex)
			if page.GetDocument() == document:
				page.Dispose()
				self.DeletePage(pageIndex)
				break
	
	def currentDocumentChanged_Signal(self, document):
		# A new document is set to be the current active document.
		# Lets change current active page.
		if self.pendingPageClose:
			return
		
		pageCount = self.GetPageCount()
		
		# Make sure that we changes the page that is assigned the
		# document.
		for pageIndex in range(pageCount):
			page = self.GetPage(pageIndex)
			if page.GetDocument() == document:
				self.SetSelection(pageIndex)
				break
