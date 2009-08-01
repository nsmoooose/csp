#!/usr/bin/env python
import os
import wx

from csp.tools.layout2.scripts.data import DataTree
from FileCommand import FileCommand

class XmlFileCommand(FileCommand):
	"""Base class for all command objects that involves xml files."""
	
	def __init__(self):
		self.graph = None

	def SetGraph(self, graph):
		self.graph = graph
		
	def GetGraph(self):
		if self.graph is None:
			# Get the application object. This object is used to retreive the
			# configuration object and the top window for this application.
			application = wx.GetApp()
			
			# Get the top window for this application. The top window shall be the 
			# parent for the open file dialog.
			topWindow = application.GetTopWindow()
			if topWindow == None:
				return None
			
			# Retreive the filename. It is set by the parent class.
			fileName = self.GetFileName()
			
			# Get the current xml path settings. We must check that the file we
			# are loading is actually below this directory.
			xmlPath = application.Configuration.get('LayoutApplication.XmlPath', '.')
			
			# Do a safety check that the file exists under the current project directory.
			# This is mandatory since we have specified all paths to data files
			# used by the renderer.
			if not os.path.abspath(fileName).startswith(os.path.abspath(xmlPath)):
				messageDialog = wx.MessageDialog(topWindow, 
					'Cannot open xml files that are outside the XML path. To change the '
					'XML path, rerun the program with the --xml option.\n\n'
					'The current path is "%s".\n\n'
					'The file to open is "%s".' % (xmlPath, fileName),
					'Error loading XML file', style = wx.OK|wx.ICON_ERROR)
				messageDialog.ShowModal()
				return None
			
			# Parse all the xml file and all its child files used.
			try:
				self.graph = DataTree.Node.ParseFile(fileName, xmlPath)
			except Exception, e:
				wx.MessageDialog(topWindow, str(e), "Error loading XML file", style = wx.OK|wx.ICON_ERROR).ShowModal()
				return None
			
		return self.graph
