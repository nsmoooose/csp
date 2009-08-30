#!/usr/bin/env python
import wx
from FileCommand import FileCommand
from ...document.XmlObjectDocument import XmlObjectDocumentFactory
from ..controls.DocumentNotebook import DocumentNotebook

class OpenObjectXmlFileCommand(FileCommand):
	
	def __init__(self, PageClass, *args, **kwargs):
		self.PageClass = PageClass
	
	def GetCaption(self):
		return "Open unknown XML file"
	
	def GetToolTipText(self):
		return "Opens an existing unknown XML file"
	
	def GetToolBarImageName(self):
		return "document-open"
	
	def Execute(self):
		# Get the document from the DocumentRegistry
		application = wx.GetApp()
		documentRegistry = application.GetDocumentRegistry()
		document = documentRegistry.GetOrCreateDocument( XmlObjectDocumentFactory( self.GetFileName() ) )

		# Create a Page for the document and add it to the DocumentNotebook
		DocumentNotebook.Instance.AddDocumentPage(self.PageClass, document)
