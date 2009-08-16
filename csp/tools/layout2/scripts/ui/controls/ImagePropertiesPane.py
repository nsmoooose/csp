#!/usr/bin/env python
import wx

from FilePropertiesPane import FilePropertiesPane

class ImagePropertiesPane(FilePropertiesPane):
	def __init__(self, parent, document, *args, **kwargs):
		FilePropertiesPane.__init__(self, parent, document, rootLabel = "Image document", *args, **kwargs)
		
		bitmap = wx.Bitmap( document.GetFileName() )
		width_value = str( bitmap.GetWidth() )
		height_value = str( bitmap.GetHeight() )
		
		imageSize_item = self.tree.AppendItem(self.root, "Image size")
		
		width_text = wx.TextCtrl(self.tree, value = width_value, size = (50, -1), style = wx.TE_READONLY)
		width_item = self.tree.AppendItem(imageSize_item, "Width", wnd = width_text)
		
		height_text = wx.TextCtrl(self.tree, value = height_value, size = (50, -1), style = wx.TE_READONLY)
		height_item = self.tree.AppendItem(imageSize_item, "Height", wnd = height_text)

		self.tree.Expand(imageSize_item)
