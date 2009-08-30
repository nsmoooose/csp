#!/usr/bin/env python
import wx
from xml.dom import pulldom

from FileCommand import FileCommand
from OpenUnknownXmlFileCommand import OpenUnknownXmlFileCommand
from OpenObjectXmlFileCommand import OpenObjectXmlFileCommand
from OpenCustomLayoutModelFileCommand import OpenCustomLayoutModelFileCommand
from ..controls.TerrainWindow import TerrainWindow
from ..controls.XmlWindow import XmlWindow

class OpenXmlFileCommand(FileCommand):
	"""Opens the selected XML file.
	When this command is executed we are trying
	to identify the type of file XML we are loading and then
	executing the correct XmlFileCommand."""
	
	def GetCaption(self):
		return "Open XML file"
	
	def GetToolTipText(self):
		return "Opens an existing XML file"
	
	def GetToolBarImageName(self):
		return "document-open"
	
	def Execute(self):
		"""Load the dom from the specified file.
		Execute the correct XmlFileCommand based on the root element in the dom."""
		
		# Identify the type of XmlFileCommand to execute
		command = None
		
		try:
			rootObjectName = self.GetRootObjectName( self.GetFileName() )
		except Exception, error:
			wx.MessageDialog(wx.GetApp().GetTopWindow(),
				"Cannot open xml file.\n" + str(error),
				"Error loading XML file",
				style = wx.OK | wx.ICON_ERROR).ShowModal()
			return
		
		if rootObjectName is None:
			command = OpenUnknownXmlFileCommand()
		elif rootObjectName == "GenericOsgTerrain":
			command = OpenObjectXmlFileCommand( TerrainWindow )
		elif rootObjectName == "CustomLayoutModel":
			command = OpenCustomLayoutModelFileCommand()
		else:
			command = OpenObjectXmlFileCommand( XmlWindow )
		
		command.SetFileName( self.GetFileName() )
		command.Execute()
	
	def GetRootObjectName(self, fileName):
		events = pulldom.parse(fileName)
		for (event, node) in events:
			if event == pulldom.START_ELEMENT:
				if node.tagName == "Object":
					return node.getAttribute("class")
				else:
					return None
		return None
