import os.path
import wx
from FileCommand import FileCommand
from ...data import DataTree
from ...document.SceneDocument import SceneDocumentFactory
from ..controls.SceneWindow import SceneWindow
from ..controls.DocumentNotebook import DocumentNotebook

class OpenCustomLayoutModelFileCommand(FileCommand):

	caption = "Open CustomLayoutModel file"
	tooltip = "Opens an existing layout file"
	toolbarimage = "document-open"

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
		
	def GetGraph(self):
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
			return DataTree.Node.ParseFile(fileName, xmlPath)
		except Exception, e:
			wx.MessageDialog(topWindow, str(e), "Error loading XML file", style = wx.OK|wx.ICON_ERROR).ShowModal()
			return None
