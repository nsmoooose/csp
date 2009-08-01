#!/usr/bin/env python
import os.path
import wx
from csp.tools.layout2.scripts.data import DataTree
from XmlFileCommand import XmlFileCommand
from OpenGenericOsgTerrainFileCommand import OpenGenericOsgTerrainFileCommand
from OpenCustomLayoutModelFileCommand import OpenCustomLayoutModelFileCommand

class OpenXmlFileCommand(XmlFileCommand):
	"""Opens the selected XML file.
	When this command is executed we are trying
	to identify the type of file XML we are loading and then
	executing the correct XmlFileCommand."""
	
	def GetCaption(self):
		return "Open XML file"
	
	def GetToolTipText(self):
		return "Opens an existing XML file"
	
	def GetToolBarImageName(self):
		return "document-open.png"
	
	def Execute(self):
		"""Load the dom from the specified file.
		Execute the correct XmlFileCommand based on the elements in the dom."""
		
		# Identify the type of XmlFileCommand to execute
		command = None
		
		graph = self.GetGraph()
		if graph is None:
			return
		
		if graph.rootNodeIs(DataTree.GenericOsgTerrain):
			command = OpenGenericOsgTerrainFileCommand()
		
		if graph.containsNode(DataTree.CustomLayoutModel):
			command = OpenCustomLayoutModelFileCommand()
			
		if command is None:
			wx.MessageDialog(wx.GetApp().GetTopWindow(), "Cannot open xml file with unknown elements.", "Error loading XML file", style = wx.OK|wx.ICON_ERROR).ShowModal()
			return
		
		command.SetFileName( self.GetFileName() )
		command.SetGraph(graph)
		command.Execute()
