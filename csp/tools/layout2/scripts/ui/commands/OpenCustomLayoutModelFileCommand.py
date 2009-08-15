#!/usr/bin/env python
import os.path
import wx
from XmlFileCommand import XmlFileCommand
from ...document.SceneDocument import SceneDocument
from ..controls.SceneWindow import SceneWindow
from ..controls.DocumentNotebook import DocumentNotebook

class SceneDocumentFactory():
	def __init__(self, fileName, node):
		self.fileName = fileName
		self.node = node
	
	def GetUniqueId(self):
		"""Returns a unique Id identifying the document in the DocumentRegistry."""
		return SceneDocument.MakeUniqueId(self.fileName)
	
	def CreateDocument(self):
		"""Returns a new document that will be added in the DocumentRegistry."""
		document = SceneDocument(os.path.basename(self.fileName))
		document.SetFileName(self.fileName)
		document.SetRootNode(self.node)
		return document

class OpenCustomLayoutModelFileCommand(XmlFileCommand):
	
	def GetCaption(self):
		return "Open CustomLayoutModel file"
	
	def GetToolTipText(self):
		return "Opens an existing layout file"
	
	def GetToolBarImageName(self):
		return "document-open.png"
	
	def Execute(self):
		"""Load a feature group or feature model from the specified file.  The existing
		graph, if any, will be discarded.  If file represents a feature model, a
		default feature group is created at the root of the new graph.
		"""
		
		# Get the application object. This object is used to retreive the
		# configuration object and the top window for this application.
		application = wx.GetApp()
		
		graph = self.GetGraph()
		if graph is None:
			return
		
		node_map = graph.realize()
		self._node_map = node_map
		node = node_map.getRoot()
		assert(node is not None and node.isGroup())
		
		# Get the document from the DocumentRegistry
		documentRegistry = application.GetDocumentRegistry()
		document = documentRegistry.GetOrCreateDocument( SceneDocumentFactory(self.GetFileName(), node) )

		# Create a SceneWindow for the document and add it to the DocumentNotebook
		DocumentNotebook.Instance.AddDocumentPage(SceneWindow, document)
		
		node.thisown = 0
