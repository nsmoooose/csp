#!/usr/bin/env python
import wx

from FilePropertiesPane import FilePropertiesPane
import XmlPropertiesPaneItem
import XmlPropertiesPaneItemArchive

class XmlPropertiesPane(FilePropertiesPane):
	imageList = None
	imageListItems = {}
	
	def __init__(self, parent, document, *args, **kwargs):
		FilePropertiesPane.__init__(self, parent, document, rootLabel = "XML document", *args, **kwargs)
		
		wx.GetApp().GetDocumentRegistry().ReferenceDocument(self.document)
		self.document.GetChangedSignal().Connect(self.on_DocumentChanged)
		
		self.InitItemForXmlNode(self.root, self.document.GetXmlNodeDocument(), 0)
		self.tree.Bind(wx.EVT_TREE_ITEM_EXPANDING, self.on_TreeItemExpanding)
		
		self.itemUpdaters = [
			XmlPropertiesPaneItemArchive.ItemUpdaterString(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterBool(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterInt(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterListTextItemInt(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterFloat(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterListTextItemFloat(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterReal(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterListTextItemReal(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterECEF(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterLLA(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterUTM(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterVector(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterMatrix(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterQuat(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterDate(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterEnum(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterExternal(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterKey(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterListTextItemKey(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterTable1(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterTable2(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterTable3(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterBreaks0(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterBreaks1(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterBreaks2(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterValues(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterPath(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterList(self),
			XmlPropertiesPaneItemArchive.ItemUpdaterObject(self),
			XmlPropertiesPaneItem.ItemUpdaterDocument(self),
			XmlPropertiesPaneItem.ItemUpdaterElement(self),
			XmlPropertiesPaneItem.ItemUpdaterText(self),
			XmlPropertiesPaneItem.ItemUpdaterComment(self),
			XmlPropertiesPaneItem.ItemUpdaterAttribute(self),
			]
		
		self.on_DocumentChanged(self.document)
		self.RestoreTreeItemsState( self.root, self.document.viewData.get('XmlPropertiesPane', {}) )
	
	def Dispose(self):
		self.document.viewData['XmlPropertiesPane'] = self.SaveTreeItemsState(self.root)
		
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
			'string': 'xml-node-string',
			'bool': 'xml-node-bool',
			'int': 'xml-node-integer',
			'float': 'xml-node-float',
			'real': 'xml-node-real',
			'ecef': 'world',
			'lla': 'world',
			'utm': 'world',
			'vector': 'xml-node-vector3',
			'matrix': 'xml-node-matrix3',
			'quat': 'arrow_rotate_anticlockwise',
			'date': 'date',
			'enum': 'text_list_numbers',
			'external': 'attach',
			'key': 'key',
			'lut': 'chart_curve',
			'path': 'brick_link',
			'list': 'text_list_bullets',
			'object': 'brick',
			'root': 'page_white_code',
			'element': 'tag',
			'text': 'text_align_left',
			'comment': 'comment',
			'attribute': 'tag_blue',
		}
	
	def on_DocumentChanged(self, document):
		self.tree.Freeze()
		self.UpdateItem(self.root)
		self.tree.Thaw()
	
	def on_TreeItemExpanding(self, event):
		self.tree.Freeze()
		item = event.GetItem()
		node = item.xmlNode
		for itemUpdater in self.itemUpdaters:
			if isinstance(node, itemUpdater.NodeClass):
				itemUpdater.ItemExpanding(item)
				break
		self.tree.Thaw()
	
	def UpdateItem(self, item):
		node = item.xmlNode
		for itemUpdater in self.itemUpdaters:
			if isinstance(node, itemUpdater.NodeClass):
				itemUpdater.UpdateItem(item)
				break
	
	def InitItemForXmlNode(self, item, xmlNode, level):
		item.xmlNode = xmlNode
		item.xmlChangeCount = -1
		item.xmlChildrenChangeCount = -1
		item.level = level
	
	def SaveTreeItemsState(self, item):
		childrenState = {}
		for child in self.GetItemChildren(item):
			itemState = ItemState()
			if self.tree.IsSelected(child):
				itemState.isSelected = True
			if self.tree.IsExpanded(child):
				itemState.isExpanded = True
				itemState.childrenState = self.SaveTreeItemsState(child)
			if itemState.HasNonDefaultValue():
				childrenState[child.xmlNode] = itemState
		return childrenState
	
	def RestoreTreeItemsState(self, item, childrenState):
		for child in self.GetItemChildren(item):
			if child.xmlNode in childrenState:
				itemState = childrenState[child.xmlNode]
				if itemState.isSelected:
					self.SelectItem(child)
				if itemState.isExpanded:
					self.tree.Expand(child)
					self.RestoreTreeItemsState(child, itemState.childrenState)
	
	def GetItemChildren(self, item):
		child, unused = self.tree.GetFirstChild(item)
		while child is not None:
			yield child
			child = self.tree.GetNextSibling(child)
	
	def SelectItem(self, item):
		# Workaround for a refresh bug in wx.lib.customtreectrl.CustomTreeCtrl.EnsureVisible
		self.GetSizer().Hide(self.tree)
		
		self.tree.EnsureVisible(item)
		self.tree.SelectItem(item)
		
		self.GetSizer().Show(self.tree)
		self.GetSizer().Layout()

class ItemState(object):
	def __init__(self):
		self.isSelected = False
		self.isExpanded = False
		self.childrenState = {}
	
	def HasNonDefaultValue(self):
		return self.isSelected or self.isExpanded
