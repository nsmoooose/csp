#!/usr/bin/env python
import os.path
import wx
from XmlFileCommand import XmlFileCommand
from ...document.TerrainDocument import TerrainDocument
from ..controls.TerrainWindow import TerrainWindow
from ..controls.DocumentNotebook import DocumentNotebook

class TerrainDocumentFactory():
	def __init__(self, fileName):
		self.fileName = fileName
	
	def GetUniqueId(self):
		"""Returns a unique Id identifying the document in the DocumentRegistry."""
		return TerrainDocument.MakeUniqueId(self.fileName)
	
	def CreateDocument(self):
		"""Returns a new document that will be added in the DocumentRegistry."""
		document = TerrainDocument(os.path.basename(self.fileName))
		document.SetFileName(self.fileName)
		return document

class OpenGenericOsgTerrainFileCommand(XmlFileCommand):
	
	def GetCaption(self):
		return "Open GenericOsgTerrain file"
	
	def GetToolTipText(self):
		return "Opens an existing generic osg terrain file"
	
	def GetToolBarImageName(self):
		return "document-open.png"
	
	def Execute(self):
		"""Load a generic osg terrain model from the specified file."""
		
		graph = self.GetGraph()
		if graph is None:
			return
		
		# Get the document from the DocumentRegistry
		application = wx.GetApp()
		documentRegistry = application.GetDocumentRegistry()
		document = documentRegistry.GetOrCreateDocument( TerrainDocumentFactory( self.GetFileName() ) )

		# Create a TerrainWindow for the document and add it to the DocumentNotebook
		DocumentNotebook.Instance.AddDocumentPage(TerrainWindow, document)
