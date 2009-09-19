#!/usr/bin/env python
import wx

from AutoFitTextCtrl import AutoFitTextCtrl
from ...data import XmlNode

class ItemUpdater(object):
	def __init__(self, propertiesPane, *args, **kwargs):
		self.propertiesPane = propertiesPane
	
	def UpdateItem(self, item):
		pass
	
	def ItemExpanding(self, item):
		pass
	
	def GetErrorMessage(self, node):
		return '\n'.join( "- %s" % error for error in node.errors.itervalues() )
	
	def SetItemImage(self, item, image):
		self.propertiesPane.tree.SetItemImage(item, self.propertiesPane.TreeImages()[image])
	
	def SetItemWindow(self, item, itemWindow):
		self.propertiesPane.tree.DeleteItemWindow(item)
		errorMessage = self.GetErrorMessage(item.xmlNode)
		if errorMessage or itemWindow is not None:
			itemWindow = ItemWindowWithError(self.propertiesPane.tree, errorMessage, itemWindow)
			self.propertiesPane.tree.SetItemWindow(item, itemWindow)


class ItemUpdaterWithoutChildren(ItemUpdater):
	def UpdateItem(self, item):
		if item.xmlChangeCount != item.xmlNode.changeCount:
			self.UpdateLocalChanges(item)
			item.xmlChangeCount = item.xmlNode.changeCount
	
	def UpdateLocalChanges(self, item):
		pass


class ItemUpdaterData(ItemUpdaterWithoutChildren):
	def UpdateLocalChanges(self, item):
		if self.propertiesPane.tree.GetItemImage(item) < 0:
			self.SetItemImage( item, self.GetItemImage() )
			self.propertiesPane.tree.SetItemText( item, self.GetItemText() )
		
		itemWindow = AutoFitTextCtrl(self.propertiesPane.tree, item.xmlNode.GetText(), style = wx.TE_READONLY)
		self.SetItemWindow(item, itemWindow)
	
	def GetItemImage(self):
		return ''
	
	def GetItemText(self):
		return ''


class ItemUpdaterText(ItemUpdaterData):
	NodeClass = XmlNode.XmlNodeText
	
	def GetItemImage(self):
		return 'text'
	
	def GetItemText(self):
		return 'Text'


class ItemUpdaterComment(ItemUpdaterData):
	NodeClass = XmlNode.XmlNodeComment
	
	def GetItemImage(self):
		return 'comment'
	
	def GetItemText(self):
		return 'Comment'


class ItemUpdaterAttribute(ItemUpdaterWithoutChildren):
	NodeClass = XmlNode.XmlNodeAttribute
	
	def UpdateLocalChanges(self, item):
		if self.propertiesPane.tree.GetItemImage(item) < 0:
			self.SetItemImage(item, 'attribute')
		
		self.propertiesPane.tree.SetItemText( item, item.xmlNode.GetName() )
		
		itemWindow = AutoFitTextCtrl(self.propertiesPane.tree, item.xmlNode.GetValue(), style = wx.TE_READONLY)
		self.SetItemWindow(item, itemWindow)


class ItemUpdaterWithChildren(ItemUpdater):
	def UpdateItem(self, item):
		firstUpdate = item.xmlChangeCount == -1
		
		if item.xmlChangeCount != item.xmlNode.changeCount:
			self.UpdateLocalChanges(item)
			
			if self.propertiesPane.tree.IsExpanded(item):
				if self.AddRemoveChildren(item):
					# New nodes where added, force an update
					item.xmlChildrenChangeCount -= 1
			else:
				if len(list( self.GetNodeChildren(item.xmlNode) )):
					self.propertiesPane.tree.SetItemHasChildren(item, True)
					
					# Force an update when expanding
					if item.xmlChildrenChangeCount == item.xmlNode.childrenChangeCount:
						item.xmlChildrenChangeCount -= 1
				else:
					self.propertiesPane.tree.DeleteChildren(item)
					self.propertiesPane.tree.SetItemHasChildren(item, False)
			
			item.xmlChangeCount = item.xmlNode.changeCount
		
		if self.propertiesPane.tree.IsExpanded(item):
			if item.xmlChildrenChangeCount != item.xmlNode.childrenChangeCount:
				self.UpdateChildren(item)
				
				item.xmlChildrenChangeCount = item.xmlNode.childrenChangeCount
		
		if firstUpdate and item.level == 1:
			# Expand the XML rootElement
			self.propertiesPane.tree.Expand(item)
	
	def ItemExpanding(self, item):
		if item.xmlChildrenChangeCount != item.xmlNode.childrenChangeCount:
			self.AddRemoveChildren(item)
			self.UpdateChildren(item)
			
			item.xmlChildrenChangeCount = item.xmlNode.childrenChangeCount
	
	def UpdateLocalChanges(self, item):
		pass
	
	def GetNodeChildren(self, node):
		return node.GetChildren()
	
	def AddRemoveChildren(self, item):
		nodeChildren = list( self.GetNodeChildren(item.xmlNode) )
		
		# Remove unused items
		itemsToRemove = []
		for itemChild in self.GetItemChildren(item):
			if itemChild.xmlNode not in nodeChildren:
				itemsToRemove.append(itemChild)
		
		for itemChild in itemsToRemove:
			self.propertiesPane.tree.Delete(itemChild)
		itemsToRemove = None
		
		# Add missing items
		newChildren = False
		itemChild, unused = self.propertiesPane.tree.GetFirstChild(item)
		for nodeChild in nodeChildren:
			if itemChild is None:
				# Missing item
				self.AppendItemForXmlNode(item, nodeChild)
				newChildren = True
			elif itemChild.xmlNode is not nodeChild:
				# Missing item
				self.InsertItemForXmlNodeBefore(item, itemChild, nodeChild)
				newChildren = True
			else:
				# Existing item
				itemChild = self.propertiesPane.tree.GetNextSibling(itemChild)
		
		return newChildren
	
	def AppendItemForXmlNode(self, parentItem, xmlNode):
		item = self.propertiesPane.tree.AppendItem(parentItem, "")
		self.propertiesPane.InitItemForXmlNode(item, xmlNode, parentItem.level + 1)
		return item
	
	def InsertItemForXmlNodeBefore(self, parentItem, nextItem, xmlNode):
		previousItem = self.propertiesPane.tree.GetPrevSibling(nextItem)
		item = self.propertiesPane.tree.InsertItem(parentItem, previousItem, "")
		self.propertiesPane.InitItemForXmlNode(item, xmlNode, parentItem.level + 1)
		return item
	
	def GetItemChildren(self, item):
		child, unused = self.propertiesPane.tree.GetFirstChild(item)
		while child is not None:
			yield child
			child = self.propertiesPane.tree.GetNextSibling(child)
	
	def UpdateChildren(self, item):
		for itemChild in self.GetItemChildren(item):
			self.propertiesPane.UpdateItem(itemChild)
	
	def GetErrorMessage(self, node):
		message = super(ItemUpdaterWithChildren, self).GetErrorMessage(node)
		childrenErrorCount = node.childrenErrorCount
		if childrenErrorCount:
			childrenMessage = "- There are %d errors in children nodes" % node.childrenErrorCount
			if message:
				return '\n'.join( (message, childrenMessage) )
			else:
				return childrenMessage
		else:
			return message


class ItemUpdaterDocument(ItemUpdaterWithChildren):
	NodeClass = XmlNode.XmlNodeDocument
	
	def UpdateLocalChanges(self, item):
		if item is self.propertiesPane.root:
			self.SetItemWindow( item, self.propertiesPane.CreateRootWindow() )
		else:
			if self.propertiesPane.tree.GetItemImage(item) < 0:
				self.SetItemImage(item, 'root')
				self.propertiesPane.tree.SetItemText(item, 'XML document')
			
			itemWindow = AutoFitTextCtrl(self.propertiesPane.tree, item.xmlNode.documentOwner.GetFileName(), style = wx.TE_READONLY)
			self.SetItemWindow(item, itemWindow)


class ItemUpdaterElement(ItemUpdaterWithChildren):
	NodeClass = XmlNode.XmlNodeElement
	
	def UpdateLocalChanges(self, item):
		if self.propertiesPane.tree.GetItemImage(item) < 0:
			self.SetItemImage( item, self.GetItemImage() )
		
		self.propertiesPane.tree.SetItemText( item, self.GetItemText(item.xmlNode) )
		self.SetItemWindow( item, self.GetItemWindow(item.xmlNode) )
	
	def GetItemImage(self):
		return 'element'
	
	def GetItemText(self, node):
		return '<' + node.domNode.tagName + '>'
	
	def GetItemWindow(self, node):
		return None


class ItemErrorImage(wx.StaticBitmap):
	def __init__(self, parent, errorMessage, *args, **kwargs):
		warningBitmap = wx.ArtProvider.GetBitmap(wx.ART_ERROR, size = (16, 16))
		wx.StaticBitmap.__init__(self, parent, bitmap = warningBitmap, *args, **kwargs)
		self.SetToolTip( wx.ToolTip( errorMessage ) )


class ItemWindowWithError(wx.Panel):
	def __init__(self, parent, errorMessage, itemWindow, *args, **kwargs):
		wx.Panel.__init__(self, parent, *args, **kwargs)
		
		self.SetOwnBackgroundColour( parent.GetBackgroundColour() )
		
		sizer = wx.BoxSizer(wx.HORIZONTAL)
		self.SetSizer(sizer)
		
		if errorMessage:
			itemErrorImage = ItemErrorImage(self, errorMessage)
			sizer.AddF( itemErrorImage, wx.SizerFlags().Center() )
			sizer.AddSpacer(5)
		
		if itemWindow is not None:
			itemWindow.Reparent(self)
			sizer.AddF( itemWindow, wx.SizerFlags().Center().Border(wx.TOP | wx.BOTTOM, 2) )
		
		self.Fit()
