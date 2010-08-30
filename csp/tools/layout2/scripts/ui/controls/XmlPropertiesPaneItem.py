import weakref
import wx

from AutoFitTextCtrl import AutoFitTextCtrl
from ...data import XmlNode
from ..commands.ModifyXmlAction import ModifyXmlDataAction
from ..commands.ModifyXmlAction import ModifyXmlAttributeAction

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
	
	def SetItemWindow(self, item, itemWindow, modifyWindowFactory = None):
		self.propertiesPane.tree.DeleteItemWindow(item)
		errorMessage = self.GetErrorMessage(item.xmlNode)
		if errorMessage or itemWindow is not None:
			itemWindow = ItemWindowWithError(self.propertiesPane.tree, errorMessage, itemWindow, modifyWindowFactory, item.xmlNode)
			self.propertiesPane.tree.SetItemWindow(item, itemWindow)
	
	def GetActualImageName(self, itemImageName):
		return self.propertiesPane.ImageListItemNames()[ itemImageName ]


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
		
		itemWindow = ItemWindowSimple(self.propertiesPane.tree, item, value = item.xmlNode.GetText(), style = wx.TE_READONLY)
		self.SetItemWindow(item, itemWindow, self.CreateModifyWindow)
	
	def GetItemImage(self):
		return ''
	
	def GetItemText(self):
		return ''
	
	def CreateModifyWindow(self, parent, node):
		return ModifyWindowData( parent, node, self.GetItemText(), self.GetActualImageName( self.GetItemImage() ) )


class ModifyWindowData(wx.TextCtrl):
	def __init__(self, parent, node, nodeName, imageName):
		wx.TextCtrl.__init__(self, parent, value = node.GetText(), style = wx.TE_MULTILINE | wx.TE_DONTWRAP)
		self.node = node
		self.nodeName = nodeName
		self.imageName = imageName
	
	def GetAction(self):
		return ModifyXmlDataAction( self.nodeName, self.imageName, self.node, self.GetValue() )


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
		
		itemWindow = ItemWindowSimple(self.propertiesPane.tree, item, value = item.xmlNode.GetValue(), style = wx.TE_READONLY)
		self.SetItemWindow(item, itemWindow, self.CreateModifyWindow)
	
	def CreateModifyWindow(self, parent, node):
		return ModifyWindowAttribute( parent, node, self.GetActualImageName('attribute') )


class ModifyWindowAttribute(wx.TextCtrl):
	def __init__(self, parent, node, imageName):
		wx.TextCtrl.__init__( self, parent, value = node.GetValue() )
		self.node = node
		self.imageName = imageName
	
	def GetAction(self):
		return ModifyXmlAttributeAction( self.imageName, self.node, self.GetValue() )


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
				if len(list( self.GetNodeChildren(item) )):
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
	
	def GetNodeChildren(self, item):
		return item.xmlNode.GetChildren()
	
	def AddRemoveChildren(self, item):
		nodeChildren = list( self.GetNodeChildren(item) )
		
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
	
	def UpdateChildren(self, item):
		for itemChild in self.propertiesPane.GetItemChildren(item):
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
			
			itemWindow = ItemWindowSimple(self.propertiesPane.tree, item, value = item.xmlNode.documentOwner.GetFileName(), style = wx.TE_READONLY)
			self.SetItemWindow(item, itemWindow)


class ItemUpdaterElement(ItemUpdaterWithChildren):
	NodeClass = XmlNode.XmlNodeElement
	
	def UpdateLocalChanges(self, item):
		if self.propertiesPane.tree.GetItemImage(item) < 0:
			self.SetItemImage( item, self.GetItemImage() )
		
		self.propertiesPane.tree.SetItemText( item, self.GetItemText(item.xmlNode) )
		self.SetItemWindow( item, self.GetItemWindow(item), self.GetModifyWindow(item.xmlNode) )
	
	def GetItemImage(self):
		return 'element'
	
	def GetItemText(self, node):
		return '<' + node.domNode.tagName + '>'
	
	def GetItemWindow(self, item):
		return None
	
	def GetModifyWindow(self, node):
		return None


class ItemWindowSimple(AutoFitTextCtrl):
	def __init__(self, parent, item, tree = None, value = wx.EmptyString, style = 0, *args, **kwargs):
		AutoFitTextCtrl.__init__(self, parent, value = value, style = style, *args, **kwargs)
		self.Bind(wx.EVT_SET_FOCUS, self.on_SetFocus)
		
		if tree is None:
			tree = parent
		self.tree = weakref.ref(tree)
		self.item = weakref.ref(item)
	
	def on_SetFocus(self, event):
		event.Skip()
		tree = self.tree()
		if tree is None:
			return
		item = self.item()
		if item is None:
			return
		tree.SelectItem(item)


class ItemErrorImage(wx.StaticBitmap):
	def __init__(self, parent, errorMessage, *args, **kwargs):
		warningBitmap = wx.ArtProvider.GetBitmap(wx.ART_ERROR, size = (16, 16))
		wx.StaticBitmap.__init__(self, parent, bitmap = warningBitmap, *args, **kwargs)
		self.SetToolTip( wx.ToolTip( errorMessage ) )


class ItemWindowWithError(wx.Panel):
	def __init__(self, parent, errorMessage, itemWindow, modifyWindowFactory, node, *args, **kwargs):
		wx.Panel.__init__(self, parent, *args, **kwargs)
		
		self.modifyWindowFactory = modifyWindowFactory
		self.node = node
		
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
			
			if modifyWindowFactory is not None:
				sizer.AddSpacer(5)
				editBitmap = wx.ArtProvider.GetBitmap('pencil', size = (16, 16))
				editButton = wx.BitmapButton(self, bitmap = editBitmap)
				editButton.Bind(wx.EVT_BUTTON, self.on_EditButton)
				sizer.AddF( editButton, wx.SizerFlags().Center() )
		
		self.Fit()
	
	def on_EditButton(self, event):
		action = None
		modifyDialog = ModifyDialog(self, self.modifyWindowFactory, self.node)
		if modifyDialog.ShowModal() == wx.ID_OK:
			action = modifyDialog.modifyWindow.GetAction()
			wx.CallLater(1, action.Execute)
		modifyDialog.Destroy()


class ModifyDialog(wx.Dialog):
	def __init__(self, parent, modifyWindowFactory, node, *args, **kwargs):
		wx.Dialog.__init__(self, parent, style = wx.DEFAULT_DIALOG_STYLE | wx.RESIZE_BORDER, *args, **kwargs)
		
		sizer = wx.BoxSizer(wx.VERTICAL)
		self.SetSizer(sizer)
		
		self.modifyWindow = modifyWindowFactory(self, node)
		sizer.AddF( self.modifyWindow, wx.SizerFlags().Expand().Proportion(1).Border(wx.ALL, 5) )
		
		sizer.Add( self.CreateButtonSizer(wx.OK | wx.CANCEL) )
		
		self.Fit()
