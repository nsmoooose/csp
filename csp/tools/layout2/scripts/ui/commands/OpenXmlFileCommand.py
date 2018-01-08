import wx
import xml.dom.pulldom
import xml.dom.minidom

from .FileCommand import FileCommand
from .OpenUnknownXmlFileCommand import OpenUnknownXmlFileCommand
from .OpenObjectXmlFileCommand import OpenObjectXmlFileCommand
from .OpenCustomLayoutModelFileCommand import OpenCustomLayoutModelFileCommand
from ..controls.TerrainWindow import TerrainWindow
from ..controls.XmlWindow import XmlWindow

class OpenXmlFileCommand(FileCommand):
	"""Opens the selected XML file.
	When this command is executed we are trying
	to identify the type of file XML we are loading and then
	executing the correct XmlFileCommand."""

	caption = "Open XML file"
	tooltip = "Opens an existing XML file"
	toolbarimage = "document-open"

	def Execute(self):
		"""Load the dom from the specified file.
		Execute the correct XmlFileCommand based on the root element in the dom."""
		
		# Identify the type of XmlFileCommand to execute
		command = None
		
		try:
			rootObjectName = self.GetRootObjectName( self.GetFileName() )
		except Exception as error:
			loadError = str(error)

			# The error message from xml.dom.minidom is far better than from xml.dom.pulldom
			try:
				xml.dom.minidom.parse( self.GetFileName() )
			except Exception as error:
				loadError = str(error)

			wx.MessageDialog(wx.GetApp().GetTopWindow(),
				"Cannot open xml file.\n" + loadError,
				"Error loading XML file",
				style = wx.OK | wx.ICON_ERROR).ShowModal()
			return

		if rootObjectName is None:
			command = OpenUnknownXmlFileCommand()
		elif rootObjectName == "GenericOsgTerrain":
			command = OpenTerrainObjectXmlFileCommand()
		elif rootObjectName == "CustomLayoutModel":
			command = OpenCustomLayoutModelFileCommand()
		else:
			command = OpenObjectXmlFileCommand()

		command.SetFileName( self.GetFileName() )
		command.Execute()
	
	def GetRootObjectName(self, fileName):
		events = xml.dom.pulldom.parse(fileName)
		for (event, node) in events:
			if event == xml.dom.pulldom.START_ELEMENT:
				if node.tagName == "Object":
					return node.getAttribute("class")
				else:
					return None
		return None
