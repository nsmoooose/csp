#!/usr/bin/env python
import os.path
import wx
from csp.tools.layout2.scripts.document.SceneDocument import SceneDocument
from XmlFileCommand import XmlFileCommand

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
		
		# Create a document and add the root node to it. This will in
		# turn signal the document added signal that is caught in the gui.
		# This will create a 3D view.
		document = SceneDocument('Scene')
		document.SetRootNode(node)
		application.GetDocumentRegistry().Add(document)
		application.GetDocumentRegistry().SetCurrentDocument(document)
		
		node.thisown = 0
		return 1
