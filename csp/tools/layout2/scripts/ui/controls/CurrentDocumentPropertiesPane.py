import wx

from ...document.FileDocument import FileDocument
from ...document.XmlDocument import XmlDocument
from ...document.ImageDocument import ImageDocument
from NoPropertiesPane import NoPropertiesPane
from XmlPropertiesPane import XmlPropertiesPane
from ImagePropertiesPane import ImagePropertiesPane
from FilePropertiesPane import FilePropertiesPane

class CurrentDocumentPropertiesPane(wx.Panel):
	"""This window display various properties related to the current document."""
	
	Instance = None
	
	def __init__(self, parent, *args, **kwargs):
		wx.Panel.__init__(self, parent, *args, **kwargs)
		
		documentRegistry = wx.GetApp().GetDocumentRegistry()
		documentRegistry.GetCurrentDocumentChangedSignal().Connect(self.on_CurrentDocumentChanged)
		
		self.sizer = wx.BoxSizer(wx.VERTICAL)
		self.SetSizer(self.sizer)
		
		self.propertiesPane = NoPropertiesPane(self)
		self.sizer.Add(self.propertiesPane, proportion = 1, flag = wx.EXPAND)
	
	def on_CurrentDocumentChanged(self, document):
		self.propertiesPane.Dispose()
		
		newPropertiesPane = self.CreatePropertiesPaneForDocument(document)
		
		self.sizer.Replace(self.propertiesPane, newPropertiesPane)
		self.sizer.Layout()
		
		self.propertiesPane.Destroy()
		self.propertiesPane = newPropertiesPane
	
	def CreatePropertiesPaneForDocument(self, document):
		if document is None:
			return NoPropertiesPane(self)
		else:
			if isinstance(document, FileDocument):
				if isinstance(document, XmlDocument):
					return XmlPropertiesPane(self, document = document)
				elif isinstance(document, ImageDocument):
					return ImagePropertiesPane(self, document = document)
				else:
					return FilePropertiesPane(self, document = document)
			else:
				return NoPropertiesPane(self, label = "No properties")
