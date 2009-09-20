#!/usr/bin/env python
import wx

from XmlPropertiesPaneItem import ItemUpdaterWithoutChildren
from XmlPropertiesPaneItem import ItemUpdaterElement
from AutoFitTextCtrl import AutoFitTextCtrl
from ...data.XmlNode import XmlNodeText
from ...data.XmlNode import XmlNodeAttribute
from ...data import XmlNodeArchive

class ItemUpdaterListTextItem(ItemUpdaterWithoutChildren):
	def UpdateLocalChanges(self, item):
		if self.propertiesPane.tree.GetItemImage(item) < 0:
			self.SetItemImage( item, self.GetItemImage() )
			self.propertiesPane.tree.SetItemText( item, self.GetItemText(item.xmlNode) )
		
		self.SetItemWindow( item, self.GetItemWindow(item.xmlNode) )
	
	def GetItemImage(self):
		return ''
	
	def GetItemText(self, node):
		return '<' + node.tag + '>'
	
	def GetItemWindow(self, node):
		return AutoFitTextCtrl(self.propertiesPane.tree, node.GetText(), style = wx.TE_READONLY)


class ItemUpdaterNodeArchive(ItemUpdaterElement):
	def GetItemText(self, node):
		if isinstance(node.parent, XmlNodeArchive.XmlNodeObject):
			itemText = node.GetAttributeValue('name')
			if itemText:
				return itemText
		return super(ItemUpdaterNodeArchive, self).GetItemText(node)
	
	def GetNodeChildren(self, node):
		if isinstance(node.parent, XmlNodeArchive.XmlNodeObject):
			return self.GetNodeChildrenWithoutNameAttribute(node)
		else:
			return super(ItemUpdaterNodeArchive, self).GetNodeChildren(node)
	
	def GetNodeChildrenWithoutNameAttribute(self, node):
		for child in super(ItemUpdaterNodeArchive, self).GetNodeChildren(node):
			if isinstance(child, XmlNodeAttribute):
				if child.GetName() == 'name':
					continue
			yield child


class ItemUpdaterSimple(ItemUpdaterNodeArchive):
	def GetItemWindow(self, node):
		return AutoFitTextCtrl(self.propertiesPane.tree, node.GetText(), style = wx.TE_READONLY)
	
	def GetNodeChildren(self, node):
		for child in super(ItemUpdaterSimple, self).GetNodeChildren(node):
			if isinstance(child, XmlNodeText):
				continue
			yield child


class ItemUpdaterString(ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeString
	
	def GetItemImage(self):
		return 'string'


class ItemUpdaterBool(ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeBool
	
	def GetItemImage(self):
		return 'bool'
	
	def GetItemWindow(self, node):
		choices = ['true', 'false']
		text = node.GetText()
		value = text.lower()
		if value not in choices:
			value = text
			choices.append( value )
		return wx.ComboBox(self.propertiesPane.tree, value = value, choices = choices, style = wx.CB_READONLY)


class ItemUpdaterIntData(object):
	def GetItemImage(self):
		return 'int'


class ItemUpdaterInt(ItemUpdaterIntData, ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeInt


class ItemUpdaterListTextItemInt(ItemUpdaterIntData, ItemUpdaterListTextItem):
	NodeClass = XmlNodeArchive.XmlNodeListTextItemInt


class ItemUpdaterFloatData(object):
	def GetItemImage(self):
		return 'float'


class ItemUpdaterFloat(ItemUpdaterFloatData, ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeFloat


class ItemUpdaterListTextItemFloat(ItemUpdaterFloatData, ItemUpdaterListTextItem):
	NodeClass = XmlNodeArchive.XmlNodeListTextItemFloat


class ItemUpdaterRealData(object):
	def GetItemImage(self):
		return 'real'


class ItemUpdaterReal(ItemUpdaterRealData, ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeReal


class ItemUpdaterListTextItemReal(ItemUpdaterRealData, ItemUpdaterListTextItem):
	NodeClass = XmlNodeArchive.XmlNodeListTextItemReal


class ItemUpdaterECEF(ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeECEF
	
	def GetItemImage(self):
		return 'ecef'
	
	def GetItemWindow(self, node):
		return ItemVector(self.propertiesPane.tree, node)


class ItemUpdaterLLA(ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeLLA
	
	def GetItemImage(self):
		return 'lla'


class ItemUpdaterUTM(ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeUTM
	
	def GetItemImage(self):
		return 'utm'


class ItemUpdaterVector(ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeVector
	
	def GetItemImage(self):
		return 'vector'
	
	def GetItemWindow(self, node):
		return ItemVector(self.propertiesPane.tree, node)


class ItemVector(wx.Panel):
	def __init__(self, parent, node, *args, **kwargs):
		wx.Panel.__init__(self, parent, *args, **kwargs)
		
		self.SetOwnBackgroundColour( parent.GetBackgroundColour() )
		
		editSizer = wx.BoxSizer(wx.HORIZONTAL)
		self.SetSizer(editSizer)
		
		editBitmap = wx.ArtProvider.GetBitmap('pencil', size = (16, 16))
		editButton = wx.BitmapButton(self, bitmap = editBitmap)
		editSizer.AddF( editButton, wx.SizerFlags().Center() )
		editSizer.AddSpacer(5)
		
		vectorSizer = wx.GridSizer(rows = 0, cols = 3)
		editSizer.AddF( vectorSizer, wx.SizerFlags().Center() )
		
		for value in node.GetStringValues():
			textCtrl = AutoFitTextCtrl(self, value, style = wx.TE_READONLY)
			vectorSizer.AddF( textCtrl, wx.SizerFlags().Expand() )
		
		self.Fit()


class ItemUpdaterMatrix(ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeMatrix
	
	def GetItemImage(self):
		return 'matrix'
	
	def GetItemWindow(self, node):
		return ItemMatrix(self.propertiesPane.tree, node)


class ItemMatrix(wx.Panel):
	def __init__(self, parent, node, *args, **kwargs):
		wx.Panel.__init__(self, parent, *args, **kwargs)
		
		self.SetOwnBackgroundColour( parent.GetBackgroundColour() )
		
		sizer = wx.GridSizer(rows = 3, cols = 3)
		self.SetSizer(sizer)
		
		for value in node.GetStringValues():
			textCtrl = AutoFitTextCtrl(self, value, style = wx.TE_READONLY)
			sizer.AddF( textCtrl, wx.SizerFlags().Expand() )
		
		self.Fit()


class ItemUpdaterQuat(ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeQuat
	
	def GetItemImage(self):
		return 'quat'


class ItemUpdaterDate(ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeDate
	
	def GetItemImage(self):
		return 'date'


class ItemUpdaterEnum(ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeEnum
	
	def GetItemImage(self):
		return 'enum'
	
	def GetItemWindow(self, node):
		choices = list(node.allowedValues)
		value = node.GetText()
		if value not in choices:
			choices.append( value )
		return wx.ComboBox(self.propertiesPane.tree, value = value, choices = choices, style = wx.CB_READONLY)


class ItemUpdaterExternal(ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeExternal
	
	def GetItemImage(self):
		return 'external'


class ItemUpdaterKeyData(object):
	def GetItemImage(self):
		return 'key'


class ItemUpdaterKey(ItemUpdaterKeyData, ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeKey


class ItemUpdaterListTextItemKey(ItemUpdaterKeyData, ItemUpdaterListTextItem):
	NodeClass = XmlNodeArchive.XmlNodeListTextItemKey


class ItemUpdaterLUT(ItemUpdaterNodeArchive):
	def GetItemImage(self):
		return 'lut'


class ItemUpdaterTable1(ItemUpdaterLUT):
	NodeClass = XmlNodeArchive.XmlNodeTable1


class ItemUpdaterTable2(ItemUpdaterLUT):
	NodeClass = XmlNodeArchive.XmlNodeTable2


class ItemUpdaterTable3(ItemUpdaterLUT):
	NodeClass = XmlNodeArchive.XmlNodeTable3


class ItemUpdaterBreaks(ItemUpdaterNodeArchive):
	def GetItemImage(self):
		return 'list'


class ItemUpdaterBreaks0(ItemUpdaterBreaks):
	NodeClass = XmlNodeArchive.XmlNodeBreaks0


class ItemUpdaterBreaks1(ItemUpdaterBreaks):
	NodeClass = XmlNodeArchive.XmlNodeBreaks1


class ItemUpdaterBreaks2(ItemUpdaterBreaks):
	NodeClass = XmlNodeArchive.XmlNodeBreaks2


class ItemUpdaterValues(ItemUpdaterNodeArchive):
	NodeClass = XmlNodeArchive.XmlNodeValues
	
	def GetItemImage(self):
		return 'list'


class ItemUpdaterPath(ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodePath
	
	def GetItemImage(self):
		return 'path'


class ItemUpdaterList(ItemUpdaterNodeArchive):
	NodeClass = XmlNodeArchive.XmlNodeList
	
	def GetItemImage(self):
		return 'list'
	
	def GetItemWindow(self, node):
		if node.hasTextItems:
			return AutoFitTextCtrl(self.propertiesPane.tree, node.GetAttributeValue('type'), style = wx.TE_READONLY)
		else:
			return None
	
	def GetNodeChildren(self, node):
		for child in super(ItemUpdaterList, self).GetNodeChildren(node):
			if isinstance(child, XmlNodeAttribute):
				if child.GetName() == 'type':
					continue
			elif node.TextItemClass:
				if isinstance(child, XmlNodeText):
					continue
			yield child


class ItemUpdaterObject(ItemUpdaterNodeArchive):
	NodeClass = XmlNodeArchive.XmlNodeObject
	
	def GetItemImage(self):
		return 'object'
	
	def GetItemWindow(self, node):
		classAttribute = node.GetAttributeValue('class')
		if classAttribute:
			return AutoFitTextCtrl(self.propertiesPane.tree, classAttribute, style = wx.TE_READONLY)
		else:
			return None
	
	def GetNodeChildren(self, node):
		for child in super(ItemUpdaterObject, self).GetNodeChildren(node):
			if isinstance(child, XmlNodeAttribute):
				if child.GetName() == 'class':
					continue
			yield child
