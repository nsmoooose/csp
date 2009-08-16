#!/usr/bin/env python
import wx
import wx.lib.agw.customtreectrl

class FilePropertiesPane(wx.Panel):
	def __init__(self, parent, document, rootLabel = "File document", *args, **kwargs):
		wx.Panel.__init__(self, parent, *args, **kwargs)
		
		sizer = wx.BoxSizer(wx.VERTICAL)
		self.SetSizer(sizer)
		
		self.tree = wx.lib.agw.customtreectrl.CustomTreeCtrl(self, style = wx.lib.agw.customtreectrl.TR_DEFAULT_STYLE | wx.lib.agw.customtreectrl.TR_HAS_VARIABLE_ROW_HEIGHT)
		sizer.Add(self.tree, proportion = 1, flag = wx.EXPAND)
		
		self.root = self.tree.AddRoot(rootLabel)
		
		fileName_text = wx.TextCtrl(self.tree, value = document.GetFileName(), size = (500, -1), style = wx.TE_READONLY)
		fileName_item = self.tree.AppendItem(self.root, "File name", wnd = fileName_text)
		
		self.tree.Expand(self.root)
	
	def Dispose(self):
		pass
