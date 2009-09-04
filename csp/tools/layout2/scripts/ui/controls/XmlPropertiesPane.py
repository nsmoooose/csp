#!/usr/bin/env python
import wx

from FilePropertiesPane import FilePropertiesPane
from AutoFitTextCtrl import AutoFitTextCtrl
from ...data.XmlNode import XmlNodeDocument
from ...data.XmlNode import XmlNodeElement
from ...data.XmlNode import XmlNodeText
from ...data.XmlNode import XmlNodeComment
from ...data.XmlNode import XmlNodeAttribute
from ...data.XmlNodeArchive import XmlNodeObject

class XmlPropertiesPane(FilePropertiesPane):
	imageList = None
	imageListItems = {}
	
	def __init__(self, parent, document, *args, **kwargs):
		FilePropertiesPane.__init__(self, parent, document, rootLabel = "XML document", *args, **kwargs)
		
		wx.GetApp().GetDocumentRegistry().ReferenceDocument(self.document)
		self.document.GetChangedSignal().Connect(self.on_DocumentChanged)
		
		self.InitItemForXmlNode( self.root, self.document.GetXmlNodeDocument() )
		
		self.itemUpdaterDocument = ItemUpdaterDocument(self)
		self.itemUpdaterElement = ItemUpdaterElement(self)
		self.itemUpdaterText = ItemUpdaterText(self)
		self.itemUpdaterComment = ItemUpdaterComment(self)
		self.itemUpdaterAttribute = ItemUpdaterAttribute(self)
		
		self.itemUpdaterObject = ItemUpdaterObject(self)
		
		self.on_DocumentChanged(self.document)
	
	def Dispose(self):
		self.document.GetChangedSignal().Disconnect(self.on_DocumentChanged)
		wx.GetApp().GetDocumentRegistry().ReleaseDocument(self.document)
		self.document = None
	
	def SetImageList(self, imageList):
		XmlPropertiesPane.imageList = imageList
	
	def GetImageList(self):
		return XmlPropertiesPane.imageList
	
	def TreeImages(self):
		return XmlPropertiesPane.imageListItems
	
	def FillImageList(self):
		return {
			'root': 'page_white_code',
			'text': 'text_align_left',
			'comment': 'comment',
			'attribute': 'tag_blue',
			'element': 'tag',
			'object': 'brick',
			'string': 'xml-node-string'
		}
	
	def on_DocumentChanged(self, document):
		nodeDocument = self.document.GetXmlNodeDocument()
		self.UpdateItem(self.root, nodeDocument)
	
	def UpdateItem(self, item, node):
		if isinstance(node, XmlNodeDocument):
			self.itemUpdaterDocument.UpdateItem(item, node)
		elif isinstance(node, XmlNodeObject):
			self.itemUpdaterObject.UpdateItem(item, node)
		elif isinstance(node, XmlNodeElement):
			self.itemUpdaterElement.UpdateItem(item, node)
		elif isinstance(node, XmlNodeText):
			self.itemUpdaterText.UpdateItem(item, node)
		elif isinstance(node, XmlNodeComment):
			self.itemUpdaterComment.UpdateItem(item, node)
		elif isinstance(node, XmlNodeAttribute):
			self.itemUpdaterAttribute.UpdateItem(item, node)
	
	def InitItemForXmlNode(self, item, xmlNode):
		item.xmlNode = xmlNode
		item.xmlChangeCount = -1
		item.xmlChildrenChangeCount = -1


class ItemUpdater(object):
	def __init__(self, propertiesPane, *args, **kwargs):
		self.propertiesPane = propertiesPane
	
	def UpdateItem(self, item, node):
		pass
	
	def GetErrorMessage(self, item, node):
		return '\n'.join( "- %s" % error for error in node.errors.itervalues() )


class ItemUpdaterNoChild(ItemUpdater):
	def UpdateItem(self, item, node):
		if item.xmlChangeCount != node.changeCount:
			self.UpdateLocalChanges(item, node)
			item.xmlChangeCount = node.changeCount
	
	def UpdateLocalChanges(self, item, node):
		pass


class ItemUpdaterData(ItemUpdaterNoChild):
	def UpdateLocalChanges(self, item, node):
		self.propertiesPane.tree.DeleteItemWindow(item)
		itemWindow = AutoFitTextCtrl(self.propertiesPane.tree, node.GetText(), style = wx.TE_READONLY)
		errorMessage = self.GetErrorMessage(item, node)
		if errorMessage:
			itemWindowWithError = ItemWindowWithError(self.propertiesPane.tree, errorMessage, itemWindow)
			self.propertiesPane.tree.SetItemWindow(item, itemWindowWithError)
		else:
			self.propertiesPane.tree.SetItemWindow(item, itemWindow)


class ItemUpdaterText(ItemUpdaterData):
	def UpdateLocalChanges(self, item, node):
		if not self.propertiesPane.tree.GetItemText(item):
			self.propertiesPane.tree.SetItemText(item, "Text")
			self.propertiesPane.tree.SetItemImage(item, self.propertiesPane.TreeImages()['text'])
		
		super(ItemUpdaterText, self).UpdateLocalChanges(item, node)


class ItemUpdaterComment(ItemUpdaterData):
	def UpdateLocalChanges(self, item, node):
		if not self.propertiesPane.tree.GetItemText(item):
			self.propertiesPane.tree.SetItemText(item, "Comment")
			self.propertiesPane.tree.SetItemImage(item, self.propertiesPane.TreeImages()['comment'])
		
		super(ItemUpdaterComment, self).UpdateLocalChanges(item, node)


class ItemUpdaterAttribute(ItemUpdaterNoChild):
	def UpdateLocalChanges(self, item, node):
		itemText = node.GetName()
		if itemText:
			self.propertiesPane.tree.SetItemText(item, itemText)
		else:
			self.propertiesPane.tree.SetItemText(item, "Attribute")
		
		self.propertiesPane.tree.SetItemImage(item, self.propertiesPane.TreeImages()['attribute'])
		
		self.propertiesPane.tree.DeleteItemWindow(item)
		itemWindow = AutoFitTextCtrl(self.propertiesPane.tree, node.GetValue(), style = wx.TE_READONLY)
		errorMessage = self.GetErrorMessage(item, node)
		if errorMessage:
			itemWindowWithError = ItemWindowWithError(self.propertiesPane.tree, errorMessage, itemWindow)
			self.propertiesPane.tree.SetItemWindow(item, itemWindowWithError)
		else:
			self.propertiesPane.tree.SetItemWindow(item, itemWindow)


class ItemUpdaterChildren(ItemUpdater):
	def UpdateItem(self, item, node):
		if item.xmlChangeCount != node.changeCount:
			self.UpdateLocalChanges(item, node)
			
			if self.AddRemoveChildren(item, node):
				# New nodes where added, force an update
				item.xmlChildrenChangeCount -= 1
			
			item.xmlChangeCount = node.changeCount
		
		if item.xmlChildrenChangeCount != node.childrenChangeCount:
			self.UpdateChildren(item, node)
			
			item.xmlChildrenChangeCount = node.childrenChangeCount
	
	def GetNodeChildren(self, node):
		for child in node.GetChildren():
			if isinstance(child, XmlNodeAttribute):
				if child.GetName() == 'name':
					continue
			yield child
	
	def AddRemoveChildren(self, item, node):
		nodeChildren = [ nodeChild for nodeChild in self.GetNodeChildren(node) ]
		
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
		
		if newChildren:
			self.propertiesPane.tree.Expand(item)
		
		return newChildren
	
	def AppendItemForXmlNode(self, parentItem, xmlNode):
		item = self.propertiesPane.tree.AppendItem(parentItem, "")
		self.propertiesPane.InitItemForXmlNode(item, xmlNode)
		return item
	
	def InsertItemForXmlNodeBefore(self, parentItem, nextItem, xmlNode):
		previousItem = self.propertiesPane.tree.GetPrevSibling(nextItem)
		item = self.propertiesPane.tree.InsertItem(parentItem, previousItem, "")
		self.propertiesPane.InitItemForXmlNode(item, xmlNode)
		return item
	
	def GetItemChildren(self, item):
		child, unused = self.propertiesPane.tree.GetFirstChild(item)
		while child is not None:
			yield child
			child = self.propertiesPane.tree.GetNextSibling(child)
	
	def UpdateChildren(self, item, node):
		itemChild, unused = self.propertiesPane.tree.GetFirstChild(item)
		for nodeChild in self.GetNodeChildren(node):
			self.propertiesPane.UpdateItem(itemChild, nodeChild)
			itemChild = self.propertiesPane.tree.GetNextSibling(itemChild)
	
	def GetErrorMessage(self, item, node):
		message = super(ItemUpdaterChildren, self).GetErrorMessage(item, node)
		childrenErrorCount = node.childrenErrorCount
		if childrenErrorCount:
			childrenMessage = "- There are %d errors in children nodes" % node.childrenErrorCount
			if message:
				return '\n'.join( (message, childrenMessage) )
			else:
				return childrenMessage
		else:
			return message


class ItemUpdaterDocument(ItemUpdaterChildren):
	def UpdateLocalChanges(self, item, node):
		self.propertiesPane.tree.DeleteItemWindow(item)
		rootWindow = self.propertiesPane.CreateRootWindow()
		errorMessage = self.GetErrorMessage(item, node)
		if errorMessage:
			itemWindowWithError = ItemWindowWithError(self.propertiesPane.tree, errorMessage, rootWindow)
			self.propertiesPane.tree.SetItemWindow(item, itemWindowWithError)
		else:
			self.propertiesPane.tree.SetItemWindow(item, rootWindow)


class ItemUpdaterElement(ItemUpdaterChildren):
	def UpdateLocalChanges(self, item, node):
		itemText = node.GetAttributeValue('name')
		if itemText:
			self.propertiesPane.tree.SetItemText(item, itemText)
		else:
			self.propertiesPane.tree.SetItemText(item, "<" + node.domNode.tagName + ">")
		
		self.propertiesPane.tree.SetItemImage(item, self.propertiesPane.TreeImages()['element'])
		
		self.propertiesPane.tree.DeleteItemWindow(item)
		errorMessage = self.GetErrorMessage(item, node)
		if errorMessage:
			itemErrorImage = ItemErrorImage(self.propertiesPane.tree, errorMessage)
			self.propertiesPane.tree.SetItemWindow(item, itemErrorImage)


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


class ItemErrorImage(wx.StaticBitmap):
	def __init__(self, parent, errorMessage, *args, **kwargs):
		warningBitmap = wx.ArtProvider.GetBitmap(wx.ART_ERROR, size = (16, 16))
		wx.StaticBitmap.__init__(self, parent, bitmap = warningBitmap, *args, **kwargs)
		self.SetToolTip( wx.ToolTip( errorMessage ) )


class ItemWindowWithError(wx.Panel):
	def __init__(self, parent, errorMessage, itemWindow, *args, **kwargs):
		wx.Panel.__init__(self, parent, *args, **kwargs)
		
		sizer = wx.BoxSizer(wx.HORIZONTAL)
		self.SetSizer(sizer)
		
		itemErrorImage = ItemErrorImage(self, errorMessage)
		sizer.AddF( itemErrorImage, wx.SizerFlags().Center() )
		
		sizer.AddSpacer(5)
		
		itemWindow.Reparent(self)
		sizer.AddF( itemWindow, wx.SizerFlags().Center() )
		
		self.Fit()
