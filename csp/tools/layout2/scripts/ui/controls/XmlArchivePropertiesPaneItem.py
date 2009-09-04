#!/usr/bin/env python
import wx

from XmlPropertiesPaneItem import ItemUpdaterElement
from XmlPropertiesPaneItem import ItemWindowWithError
from AutoFitTextCtrl import AutoFitTextCtrl
from ...data.XmlNode import XmlNodeAttribute

class ItemUpdaterObject(ItemUpdaterElement):
	def UpdateLocalChanges(self, item, node):
		itemText = node.GetAttributeValue('name')
		if itemText:
			self.propertiesPane.tree.SetItemText(item, itemText)
		else:
			self.propertiesPane.tree.SetItemText(item, "<Object>")
		
		self.propertiesPane.tree.SetItemImage(item, self.propertiesPane.TreeImages()['object'])
		
		self.propertiesPane.tree.DeleteItemWindow(item)
		itemWindow = AutoFitTextCtrl(self.propertiesPane.tree, node.GetAttributeValue('class'), style = wx.TE_READONLY)
		errorMessage = self.GetErrorMessage(item, node)
		if errorMessage:
			itemWindowWithError = ItemWindowWithError(self.propertiesPane.tree, errorMessage, itemWindow)
			self.propertiesPane.tree.SetItemWindow(item, itemWindowWithError)
		else:
			self.propertiesPane.tree.SetItemWindow(item, itemWindow)
	
	def GetNodeChildren(self, node):
		for child in super(ItemUpdaterObject, self).GetNodeChildren(node):
			if isinstance(child, XmlNodeAttribute):
				if child.GetName() == 'class':
					continue
			yield child
