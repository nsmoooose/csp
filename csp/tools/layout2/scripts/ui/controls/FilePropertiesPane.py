import wx
from wx.lib import customtreectrl

from .AutoFitTextCtrl import AutoFitTextCtrl

class FilePropertiesPane(wx.Panel):
	imageListSize = (16, 16)
	imageList = None
	imageListItems = {}
	imageListItemNames = {
		"root": "page_white",
		}
	
	def __init__(self, parent, document, rootLabel = "File document", *args, **kwargs):
		wx.Panel.__init__(self, parent, *args, **kwargs)
		
		self.document = document
		
		sizer = wx.BoxSizer(wx.VERTICAL)
		self.SetSizer(sizer)
		
		self.tree = customtreectrl.CustomTreeCtrl(self, agwStyle = customtreectrl.TR_DEFAULT_STYLE | customtreectrl.TR_HAS_VARIABLE_ROW_HEIGHT)
		sizer.Add(self.tree, proportion = 1, flag = wx.EXPAND)
		
		if self.GetImageList() is None:
			width, height = FilePropertiesPane.imageListSize
			self.SetImageList( wx.ImageList(width, height) )
			for imageListItemKey, imageListItemName in self.ImageListItemNames().items():
				bitmap = wx.ArtProvider.GetBitmap(imageListItemName, size = FilePropertiesPane.imageListSize)
				self.TreeImages()[imageListItemKey] = self.GetImageList().Add(bitmap)
		
		self.tree.SetImageList( self.GetImageList() )
		
		self.root = self.tree.AddRoot( rootLabel, image = self.TreeImages()['root'], wnd = self.CreateRootWindow() )
		
		self.tree.AppendItem(self.root, "Expand empty root hack")
		self.tree.Expand(self.root)
		self.tree.DeleteChildren(self.root)
	
	def Dispose(self):
		pass
	
	def SetImageList(self, imageList):
		FilePropertiesPane.imageList = imageList
	
	def GetImageList(self):
		return FilePropertiesPane.imageList
	
	def TreeImages(self):
		return FilePropertiesPane.imageListItems
	
	def ImageListItemNames(self):
		return self.imageListItemNames
	
	def CreateRootWindow(self):
		return AutoFitTextCtrl(self.tree, self.document.GetFileName(), style = wx.TE_READONLY)
