#!/usr/bin/env python
import wx

from FilePropertiesPane import FilePropertiesPane
from ...data.XmlNode import XmlNodeDocument
from ...data.XmlNode import XmlNodeElement
from ...data.XmlNode import XmlNodeText
from ...data.XmlNode import XmlNodeComment

class XmlPropertiesPane(FilePropertiesPane):
	def __init__(self, parent, document, *args, **kwargs):
		FilePropertiesPane.__init__(self, parent, document, rootLabel = "XML document", *args, **kwargs)
		
		self.document = document
		wx.GetApp().GetDocumentRegistry().ReferenceDocument(self.document)
		self.document.GetChangedSignal().Connect(self.on_DocumentChanged)
		
		self.nodeDocument_item = self.AppendItem( self.root, self.document.GetXmlNodeDocument() )
		
		self.itemUpdaterDocument = ItemUpdaterDocument(self)
		self.itemUpdaterElement = ItemUpdaterElement(self)
		self.itemUpdaterText = ItemUpdaterText(self)
		self.itemUpdaterComment = ItemUpdaterComment(self)
		
		self.on_DocumentChanged(document)
	
	def Dispose(self):
		self.document.GetChangedSignal().Disconnect(self.on_DocumentChanged)
		wx.GetApp().GetDocumentRegistry().ReleaseDocument(self.document)
		self.document = None
	
	def on_DocumentChanged(self, document):
		nodeDocument = self.document.GetXmlNodeDocument()
		self.UpdateItem(self.nodeDocument_item, nodeDocument)
	
	def UpdateItem(self, item, node):
		if isinstance(node, XmlNodeDocument):
			self.itemUpdaterDocument.UpdateItem(item, node)
		elif isinstance(node, XmlNodeElement):
			self.itemUpdaterElement.UpdateItem(item, node)
		elif isinstance(node, XmlNodeText):
			self.itemUpdaterText.UpdateItem(item, node)
		elif isinstance(node, XmlNodeComment):
			self.itemUpdaterComment.UpdateItem(item, node)
	
	def AppendItem(self, parentItem, xmlNode):
		item = self.tree.AppendItem(parentItem, "")
		item.xmlNode = xmlNode
		item.xmlChangeCount = -1
		item.xmlChildrenChangeCount = -1
		return item
	
	def InsertItemBefore(self, parentItem, nextItem, xmlNode):
		previousItem = self.tree.GetPrevSibling(nextItem)
		item = self.tree.InsertItem(parentItem, previousItem, "")
		item.xmlNode = xmlNode
		item.xmlChangeCount = -1
		item.xmlChildrenChangeCount = -1
		return item
	
	def GetItemChildren(self, item):
		child, unused = self.tree.GetFirstChild(item)
		
		while child is not None:
			yield child
			child = self.tree.GetNextSibling(child)


class ItemUpdater:
	def __init__(self, propertiesPane, *args, **kwargs):
		self.propertiesPane = propertiesPane
	
	def UpdateItem(self, item, node):
		pass


class ItemUpdaterNoChild(ItemUpdater):
	def UpdateItem(self, item, node):
		if item.xmlChangeCount != node.changeCount:
			self.UpdateLocalChanges(item, node)
			item.xmlChangeCount = node.changeCount
	
	def UpdateLocalChanges(self, item, node):
		pass


class ItemUpdaterText(ItemUpdaterNoChild):
	def UpdateLocalChanges(self, item, node):
		if not self.propertiesPane.tree.GetItemText(item):
			self.propertiesPane.tree.SetItemText(item, "Text")
		
		itemWindow = self.propertiesPane.tree.GetItemWindow(item)
		text = node.GetText()
		
		if itemWindow is None or itemWindow.GetLabel() != text:
			itemWindow = wx.StaticText(self.propertiesPane.tree, label = text, style = wx.BORDER_SIMPLE)
			self.propertiesPane.tree.SetItemWindow(item, itemWindow)


class ItemUpdaterComment(ItemUpdaterNoChild):
	def UpdateLocalChanges(self, item, node):
		if not self.propertiesPane.tree.GetItemText(item):
			self.propertiesPane.tree.SetItemText(item, "Comment")
		
		itemWindow = self.propertiesPane.tree.GetItemWindow(item)
		text = node.GetText()
		
		if itemWindow is None or itemWindow.GetLabel() != text:
			itemWindow = wx.StaticText(self.propertiesPane.tree, label = text, style = wx.BORDER_SIMPLE)
			self.propertiesPane.tree.SetItemWindow(item, itemWindow)


class ItemUpdaterChildren(ItemUpdater):
	def UpdateItem(self, item, node):
		if item.xmlChangeCount != node.changeCount:
			self.UpdateLocalChanges(item, node)
			
			if self.AddRemoveChildren( item, self.GetNodeChildren(node) ):
				# New nodes where added, force an update
				item.xmlChildrenChangeCount -= 1
			
			item.xmlChangeCount = node.changeCount
		
		if item.xmlChildrenChangeCount != node.childrenChangeCount:
			self.UpdateChildren( item, self.GetNodeChildren(node) )
			
			item.xmlChildrenChangeCount = node.childrenChangeCount
	
	def GetNodeChildren(self, node):
		return node.GetChildren()
	
	def AddRemoveChildren(self, item, nodeChildren):
		# Remove unused items
		itemsToRemove = []
		for itemChild in self.propertiesPane.GetItemChildren(item):
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
				self.propertiesPane.AppendItem(item, nodeChild)
				newChildren = True
			elif itemChild.xmlNode is not nodeChild:
				# Missing item
				self.propertiesPane.tree.InsertItemBefore(item, itemChild, nodeChild)
				newChildren = True
			else:
				# Existing item
				itemChild = self.propertiesPane.tree.GetNextSibling(itemChild)
		
		if newChildren:
			self.propertiesPane.tree.Expand(item)
		
		return newChildren
	
	def UpdateChildren(self, item, nodeChildren):
		itemChild, unused = self.propertiesPane.tree.GetFirstChild(item)
		for nodeChild in nodeChildren:
			self.propertiesPane.UpdateItem(itemChild, nodeChild)
			itemChild = self.propertiesPane.tree.GetNextSibling(itemChild)


class ItemUpdaterDocument(ItemUpdaterChildren):
	def UpdateLocalChanges(self, item, node):
		if not self.propertiesPane.tree.GetItemText(item):
			self.propertiesPane.tree.SetItemText(item, "Document Node")


class ItemUpdaterElement(ItemUpdaterChildren):
	def UpdateLocalChanges(self, item, node):
		if not self.propertiesPane.tree.GetItemText(item):
			self.propertiesPane.tree.SetItemText(item, "<" + node.domNode.tagName + ">")
