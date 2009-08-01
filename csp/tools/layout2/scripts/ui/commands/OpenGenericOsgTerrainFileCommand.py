#!/usr/bin/env python
import os.path
import wx
from csp.tools.layout2.scripts.document.TerrainDocument import TerrainDocument
from XmlFileCommand import XmlFileCommand

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
		
		# Create a document and add the root node to it. This will in
		# turn signal the document added signal that is caught in the gui.
		# This will create a 3D view.
		fileName = self.GetFileName()
		document = TerrainDocument( os.path.basename(fileName) )
		document.SetFileName( fileName )
		application = wx.GetApp()
		application.GetDocumentRegistry().Add(document)
		application.GetDocumentRegistry().SetCurrentDocument(document)
