#!/usr/bin/env python
import wx

from XmlPropertiesPaneItem import ItemUpdaterWithoutChildren
from XmlPropertiesPaneItem import ItemUpdaterElement
from AutoFitTextCtrl import AutoFitTextCtrl
from ...data.XmlNode import XmlNodeText
from ...data.XmlNode import XmlNodeAttribute
from ...data import XmlNodeArchive
from ..commands.ModifyXmlCommand import ModifyXmlDataCommand

class ItemUpdaterListTextItem(ItemUpdaterWithoutChildren):
	def UpdateLocalChanges(self, item):
		if self.propertiesPane.tree.GetItemImage(item) < 0:
			self.SetItemImage( item, self.GetItemImage() )
			self.propertiesPane.tree.SetItemText( item, self.GetItemText(item.xmlNode) )
		
		self.SetItemWindow( item, self.GetItemWindow(item.xmlNode), self.GetModifyWindow(item.xmlNode) )
	
	def GetItemImage(self):
		return ''
	
	def GetItemText(self, node):
		return '<' + node.tag + '>'
	
	def GetItemWindow(self, node):
		return AutoFitTextCtrl(self.propertiesPane.tree, node.GetText(), style = wx.TE_READONLY)
	
	def GetModifyWindow(self, node):
		return ModifyWindowSimple


class ItemUpdaterNodeArchive(ItemUpdaterElement):
	def GetItemText(self, node):
		if isinstance(node.parent, XmlNodeArchive.XmlNodeObject):
			itemText = node.GetAttributeValue('name')
			if itemText:
				return itemText
		return super(ItemUpdaterNodeArchive, self).GetItemText(node)
	
	def GetNodeChildren(self, item):
		if isinstance(item.xmlNode.parent, XmlNodeArchive.XmlNodeObject):
			return self.GetNodeChildrenWithoutNameAttribute(item)
		else:
			return super(ItemUpdaterNodeArchive, self).GetNodeChildren(item)
	
	def GetNodeChildrenWithoutNameAttribute(self, item):
		for child in super(ItemUpdaterNodeArchive, self).GetNodeChildren(item):
			if isinstance(child, XmlNodeAttribute):
				if child.GetName() == 'name':
					continue
			yield child


class ItemUpdaterSimple(ItemUpdaterNodeArchive):
	def GetItemWindow(self, node):
		return AutoFitTextCtrl(self.propertiesPane.tree, node.GetText(), style = wx.TE_READONLY)
	
	def GetModifyWindow(self, node):
		return ModifyWindowSimple
	
	def GetNodeChildren(self, item):
		for child in super(ItemUpdaterSimple, self).GetNodeChildren(item):
			if isinstance(child, XmlNodeText):
				continue
			yield child


class ModifyWindowSimple(wx.TextCtrl):
	def __init__(self, parent, node):
		wx.TextCtrl.__init__(self, parent, value = node.GetText(), style = wx.TE_MULTILINE | wx.TE_DONTWRAP)
		self.node = node
	
	def GetCommand(self):
		return ModifyXmlDataCommand( self.node, self.GetValue() )


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
		itemWindow = wx.ComboBox(self.propertiesPane.tree, value = value, choices = choices, style = wx.CB_READONLY)
		itemWindow.Bind(wx.EVT_COMBOBOX, ComboBoxHandler(node).onCombobox)
		return itemWindow
	
	def GetModifyWindow(self, node):
		return None


class ComboBoxHandler(object):
	def __init__(self, node):
		self.node = node
	
	def onCombobox(self, event):
		command = ModifyXmlDataCommand( self.node, event.GetString() )
		wx.CallLater(1, command.Execute)


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
		return ItemWindowMatrix(self.propertiesPane.tree, node, 1, 3)
	
	def GetModifyWindow(self, node):
		return ModifyWindowVector


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
		return ItemWindowMatrix(self.propertiesPane.tree, node, 1, 3)
	
	def GetModifyWindow(self, node):
		return ModifyWindowVector


class ModifyWindowMatrixGeneric(wx.Panel):
	def __init__(self, parent, node):
		wx.Panel.__init__(self, parent)
		self.node = node
		
		sizer = wx.GridSizer( rows = self.GetRows(), cols = self.GetCols() )
		self.SetSizer(sizer)
		
		self.textCtrl = []
		
		for value in node.GetStringValues():
			textCtrl = wx.TextCtrl( self, value = value )
			sizer.AddF( textCtrl, wx.SizerFlags().Expand() )
			self.textCtrl.append( textCtrl )
	
	def GetCommand(self):
		textRows = []
		for row in range( self.GetRows() ):
			textCols = []
			for cols in range( self.GetCols() ):
				textCols.append( self.textCtrl[ row * self.GetCols() + cols ].GetValue() )
			textRows.append( ' '.join(textCols) )
		return ModifyXmlDataCommand( self.node, '\n'.join(textRows) )


class ModifyWindowVector(ModifyWindowMatrixGeneric):
	def GetRows(self):
		return 1
	
	def GetCols(self):
		return 3


class ItemUpdaterMatrix(ItemUpdaterSimple):
	NodeClass = XmlNodeArchive.XmlNodeMatrix
	
	def GetItemImage(self):
		return 'matrix'
	
	def GetItemWindow(self, node):
		return ItemWindowMatrix(self.propertiesPane.tree, node, 3, 3)
	
	def GetModifyWindow(self, node):
		return ModifyWindowMatrix


class ItemWindowMatrix(wx.Panel):
	def __init__(self, parent, node, rows, cols, *args, **kwargs):
		wx.Panel.__init__(self, parent, *args, **kwargs)
		
		self.SetOwnBackgroundColour( parent.GetBackgroundColour() )
		
		sizer = wx.GridSizer(rows = rows, cols = cols)
		self.SetSizer(sizer)
		
		for value in node.GetStringValues():
			textCtrl = AutoFitTextCtrl(self, value, style = wx.TE_READONLY)
			sizer.AddF( textCtrl, wx.SizerFlags().Expand() )
		
		self.Fit()


class ModifyWindowMatrix(ModifyWindowMatrixGeneric):
	def GetRows(self):
		return 3
	
	def GetCols(self):
		return 3


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
		itemWindow = wx.ComboBox(self.propertiesPane.tree, value = value, choices = choices, style = wx.CB_READONLY)
		itemWindow.Bind(wx.EVT_COMBOBOX, ComboBoxHandler(node).onCombobox)
		return itemWindow
	
	def GetModifyWindow(self, node):
		return None


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
	
	def GetNodeChildren(self, item):
		ancestors = []
		parentItem = item
		while parentItem is not None:
			ancestors.append( parentItem.xmlNode )
			parentItem = self.propertiesPane.tree.GetItemParent(parentItem)
		
		for child in super(ItemUpdaterPath, self).GetNodeChildren(item):
			# Bloc infinite recursion when in a dependency cycle
			if child in ancestors:
				continue
			yield child


class ItemUpdaterList(ItemUpdaterNodeArchive):
	NodeClass = XmlNodeArchive.XmlNodeList
	
	def GetItemImage(self):
		return 'list'
	
	def GetItemWindow(self, node):
		if node.hasTextItems:
			return AutoFitTextCtrl(self.propertiesPane.tree, node.GetAttributeValue('type'), style = wx.TE_READONLY)
		else:
			return None
	
	def GetNodeChildren(self, item):
		for child in super(ItemUpdaterList, self).GetNodeChildren(item):
			if isinstance(child, XmlNodeAttribute):
				if child.GetName() == 'type':
					continue
			elif item.xmlNode.TextItemClass:
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
	
	def GetNodeChildren(self, item):
		for child in super(ItemUpdaterObject, self).GetNodeChildren(item):
			if isinstance(child, XmlNodeAttribute):
				if child.GetName() == 'class':
					continue
			yield child
