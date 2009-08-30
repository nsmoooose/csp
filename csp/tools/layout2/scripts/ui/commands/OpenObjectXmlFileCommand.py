#!/usr/bin/env python
import wx
from FileCommand import FileCommand
from ...document.XmlObjectDocument import XmlObjectDocument
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


class XmlObjectDocumentFactory():
	def __init__(self, fileName):
		self.fileName = fileName
	
	def GetUniqueId(self):
		"""Returns a unique Id identifying the document in the DocumentRegistry."""
		return XmlObjectDocument.MakeUniqueId(self.fileName)
	
	def CreateDocument(self):
		"""Returns a new document that will be added in the DocumentRegistry."""
		document = XmlObjectDocument(self.fileName)
		document.Load()
		return document
