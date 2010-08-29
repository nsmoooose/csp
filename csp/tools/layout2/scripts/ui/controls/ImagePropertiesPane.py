#!/usr/bin/env python
import wx

from FilePropertiesPane import FilePropertiesPane

class ImagePropertiesPane(FilePropertiesPane):
	imageList = None
	imageListItems = {}
	imageListItemNames = {
		'root': 'page_white_picture',
		'imageSize': 'shape_handles',
		'width': 'tag_blue',
		'height': 'tag_blue',
		}
	
	def __init__(self, parent, document, *args, **kwargs):
		FilePropertiesPane.__init__(self, parent, document, rootLabel = "Image document", *args, **kwargs)
		
		image = wx.Image( document.GetFileName() )
		if image.IsOk():
			width_value = str( image.GetWidth() )
			height_value = str( image.GetHeight() )
			
			imageSize_item = self.tree.AppendItem(self.root, "Image size", image = self.TreeImages()['imageSize'])
			
			width_text = wx.TextCtrl(self.tree, value = width_value, size = (50, -1), style = wx.TE_READONLY)
			width_item = self.tree.AppendItem(imageSize_item, "Width", image = self.TreeImages()['width'], wnd = width_text)
			
			height_text = wx.TextCtrl(self.tree, value = height_value, size = (50, -1), style = wx.TE_READONLY)
			height_item = self.tree.AppendItem(imageSize_item, "Height", image = self.TreeImages()['height'], wnd = height_text)
			
			self.tree.Expand(imageSize_item)
	
	def SetImageList(self, imageList):
		ImagePropertiesPane.imageList = imageList
	
	def GetImageList(self):
		return ImagePropertiesPane.imageList
	
	def TreeImages(self):
		return ImagePropertiesPane.imageListItems
	
	def ImageListItemNames(self):
		return self.imageListItemNames
