#!/usr/bin/env python
import os.path
import wx
from csp.tools.layout2.scripts.data import DataTree
from FileCommand import FileCommand

class OpenCustomLayoutModelFileCommand(FileCommand):

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

		# Get the top window for this application. The top window shall be the 
		# parent for the open file dialog.
		topWindow = application.GetTopWindow()
		if topWindow == None:
			return

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
				'Cannot open features that are outside the XML path.  To change the '
				'XML path, rerun the program with the --xml option.\n\n'
				'The current path is "%s"' % xmlPath,
				'Error loading feature', style = wx.OK|wx.ICON_ERROR)
			messageDialog.ShowModal()
			return

		# Parse all the xml file and all its child files used.
		graph = DataTree.Node.ParseFile(fileName, xmlPath)

		# Make sure that it is the correct type of data the user
		# has tried to load.
		if not graph.containsNode(DataTree.CustomLayoutModel):
			wx.MessageDialog(topWindow, "Cannot open leaf node FeatureModels; use insert instead.", "Error loading feature", style = wx.OK|wx.ICON_ERROR).ShowModal()
			return 0

		node_map = graph.realize()
		self._node_map = node_map
		node = node_map.getRoot()
		assert(node is not None and node.isGroup())
		
		sceneWindow = topWindow.GetSceneWindow()
		sceneWindow.graphicsWindow.graph().setRoot(node)

		node.thisown = 0
		self.UpdateTree(node)
		return 1

