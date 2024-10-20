import weakref
import wx

from .XmlPropertiesPaneItem import ItemUpdaterWithoutChildren
from .XmlPropertiesPaneItem import ItemUpdaterElement
from .XmlPropertiesPaneItem import ItemWindowSimple
from ...data.XmlNode import XmlNodeText
from ...data.XmlNode import XmlNodeAttribute
from ...data import XmlNodeArchive
from ..commands.ModifyXmlAction import ModifyXmlDataAction

class ItemUpdaterListTextItem(ItemUpdaterWithoutChildren):
    def UpdateLocalChanges(self, item):
        if self.propertiesPane.tree.GetItemImage(item) < 0:
            self.SetItemImage( item, self.GetItemImage() )
            self.propertiesPane.tree.SetItemText( item, self.GetItemText(item.xmlNode) )
        
        self.SetItemWindow( item, self.GetItemWindow(item), self.GetModifyWindow(item.xmlNode) )
    
    def GetItemImage(self):
        return ''
    
    def GetItemText(self, node):
        return '<' + node.tag + '>'
    
    def GetItemWindow(self, item):
        return ItemWindowSimple(self.propertiesPane.tree, item, value = item.xmlNode.GetText(), style = wx.TE_READONLY)
    
    def GetModifyWindow(self, node):
        return self.CreateModifyWindow
    
    def CreateModifyWindow(self, parent, node):
        return ModifyWindowSimple( parent, node, self.GetItemText(node), self.GetActualImageName( self.GetItemImage() ) )


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
    def GetItemWindow(self, item):
        return ItemWindowSimple(self.propertiesPane.tree, item, value = item.xmlNode.GetText(), style = wx.TE_READONLY)
    
    def GetModifyWindow(self, node):
        return self.CreateModifyWindow
    
    def CreateModifyWindow(self, parent, node):
        return ModifyWindowSimple( parent, node, self.GetItemText(node), self.GetActualImageName( self.GetItemImage() ) )
    
    def GetNodeChildren(self, item):
        for child in super(ItemUpdaterSimple, self).GetNodeChildren(item):
            if isinstance(child, XmlNodeText):
                continue
            yield child


class ModifyWindowSimple(wx.TextCtrl):
    def __init__(self, parent, node, nodeName, imageName):
        wx.TextCtrl.__init__(self, parent, value = node.GetText(), style = wx.TE_MULTILINE | wx.TE_DONTWRAP)
        self.node = node
        self.nodeName = nodeName
        self.imageName = imageName
    
    def GetAction(self):
        return ModifyXmlDataAction( self.nodeName, self.imageName, self.node, self.GetValue() )


class ItemUpdaterString(ItemUpdaterSimple):
    NodeClass = XmlNodeArchive.XmlNodeString
    
    def GetItemImage(self):
        return 'string'


class ItemUpdaterBool(ItemUpdaterSimple):
    NodeClass = XmlNodeArchive.XmlNodeBool
    
    def GetItemImage(self):
        return 'bool'
    
    def GetItemWindow(self, item):
        choices = ['true', 'false']
        text = item.xmlNode.GetText()
        value = text.lower()
        if value not in choices:
            value = text
            choices.append( value )
        itemWindow = ItemWindowComboBox(
            self.propertiesPane.tree,
            item,
            value = value,
            choices = choices,
            nodeName = self.GetItemText(item.xmlNode),
            imageName = self.GetActualImageName( self.GetItemImage() ),
            style = wx.CB_READONLY)
        return itemWindow
    
    def GetModifyWindow(self, node):
        return None


class ItemWindowComboBox(wx.ComboBox):
    def __init__(self, tree, item, value, choices, nodeName, imageName, style = 0, *args, **kwargs):
        wx.ComboBox.__init__(self, tree, value = value, choices = choices, style = style, *args, **kwargs)
        self.Bind(wx.EVT_SET_FOCUS, self.on_SetFocus)
        self.Bind(wx.EVT_COMBOBOX, self.on_Combobox)
        
        self.tree = weakref.ref(tree)
        self.item = weakref.ref(item)
        self.nodeName = nodeName
        self.imageName = imageName
    
    def on_SetFocus(self, event):
        event.Skip()
        tree = self.tree()
        if tree is None:
            return
        item = self.item()
        if item is None:
            return
        tree.SelectItem(item)
    
    def on_Combobox(self, event):
        item = self.item()
        if item is None:
            return
        action = ModifyXmlDataAction( self.nodeName, self.imageName, item.xmlNode, event.GetString() )
        wx.CallLater(1, action.Execute)


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
    
    def GetItemWindow(self, item):
        return ItemWindowMatrix(self.propertiesPane.tree, item, 1, 3)
    
    def GetModifyWindow(self, node):
        return self.CreateModifyWindow
    
    def CreateModifyWindow(self, parent, node):
        return ModifyWindowMatrix( parent, node, 1, 3, self.GetItemText(node), self.GetActualImageName( self.GetItemImage() ) )


class ItemUpdaterLLA(ItemUpdaterSimple):
    NodeClass = XmlNodeArchive.XmlNodeLLA
    
    def GetItemImage(self):
        return 'lla'


class ItemUpdaterUTM(ItemUpdaterSimple):
    NodeClass = XmlNodeArchive.XmlNodeUTM
    
    def GetItemImage(self):
        return 'utm'


class ItemUpdaterVector2(ItemUpdaterSimple):
    NodeClass = XmlNodeArchive.XmlNodeVector2
    
    def GetItemImage(self):
        return 'vector'
    
    def GetItemWindow(self, item):
        return ItemWindowMatrix(self.propertiesPane.tree, item, 1, 2)
    
    def GetModifyWindow(self, node):
        return self.CreateModifyWindow
    
    def CreateModifyWindow(self, parent, node):
        return ModifyWindowMatrix( parent, node, 1, 2, self.GetItemText(node), self.GetActualImageName( self.GetItemImage() ) )

class ItemUpdaterVector3(ItemUpdaterSimple):
    NodeClass = XmlNodeArchive.XmlNodeVector3
    
    def GetItemImage(self):
        return 'vector'
    
    def GetItemWindow(self, item):
        return ItemWindowMatrix(self.propertiesPane.tree, item, 1, 3)
    
    def GetModifyWindow(self, node):
        return self.CreateModifyWindow
    
    def CreateModifyWindow(self, parent, node):
        return ModifyWindowMatrix( parent, node, 1, 3, self.GetItemText(node), self.GetActualImageName( self.GetItemImage() ) )


class ItemUpdaterMatrix(ItemUpdaterSimple):
    NodeClass = XmlNodeArchive.XmlNodeMatrix
    
    def GetItemImage(self):
        return 'matrix'
    
    def GetItemWindow(self, item):
        return ItemWindowMatrix(self.propertiesPane.tree, item, 3, 3)
    
    def GetModifyWindow(self, node):
        return self.CreateModifyWindow
    
    def CreateModifyWindow(self, parent, node):
        return ModifyWindowMatrix( parent, node, 3, 3, self.GetItemText(node), self.GetActualImageName( self.GetItemImage() ) )


class ItemWindowMatrix(wx.Panel):
    def __init__(self, tree, item, rows, cols, *args, **kwargs):
        wx.Panel.__init__(self, tree, *args, **kwargs)
        
        self.SetOwnBackgroundColour( tree.GetBackgroundColour() )
        
        sizer = wx.GridSizer(rows = rows, cols = cols)
        self.SetSizer(sizer)
        
        for value in item.xmlNode.GetStringValues():
            textCtrl = ItemWindowSimple(self, item, tree = tree, value = value, style = wx.TE_READONLY)
            sizer.AddF( textCtrl, wx.SizerFlags().Expand() )
        
        self.Fit()


class ModifyWindowMatrix(wx.Panel):
    def __init__(self, parent, node, rows, cols, nodeName, imageName):
        wx.Panel.__init__(self, parent)
        self.node = node
        self.rows = rows
        self.cols = cols
        self.nodeName = nodeName
        self.imageName = imageName
        
        sizer = wx.GridSizer( rows = self.rows, cols = self.cols )
        self.SetSizer(sizer)
        
        self.textCtrl = []
        
        for value in node.GetStringValues():
            textCtrl = wx.TextCtrl( self, value = value )
            sizer.AddF( textCtrl, wx.SizerFlags().Expand() )
            self.textCtrl.append( textCtrl )
    
    def GetAction(self):
        textRows = []
        for row in range( self.rows ):
            textCols = []
            for col in range( self.cols ):
                textCols.append( self.textCtrl[ row * self.cols + col ].GetValue() )
            textRows.append( ' '.join(textCols) )
        return ModifyXmlDataAction( self.nodeName, self.imageName, self.node, '\n'.join(textRows) )


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
    
    def GetItemWindow(self, item):
        choices = list(item.xmlNode.allowedValues)
        value = item.xmlNode.GetText()
        if value not in choices:
            choices.append( value )
        itemWindow = ItemWindowComboBox(
            self.propertiesPane.tree,
            item,
            value = value,
            choices = choices,
            nodeName = self.GetItemText(item.xmlNode),
            imageName = self.GetActualImageName( self.GetItemImage() ),
            style = wx.CB_READONLY)
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
    
    def GetItemWindow(self, item):
        if item.xmlNode.hasTextItems:
            return ItemWindowSimple(self.propertiesPane.tree, item, value = item.xmlNode.GetAttributeValue('type'), style = wx.TE_READONLY)
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
    
    def GetItemWindow(self, item):
        classAttribute = item.xmlNode.GetAttributeValue('class')
        if classAttribute:
            return ItemWindowSimple(self.propertiesPane.tree, item, value = classAttribute, style = wx.TE_READONLY)
        else:
            return None
    
    def GetNodeChildren(self, item):
        for child in super(ItemUpdaterObject, self).GetNodeChildren(item):
            if isinstance(child, XmlNodeAttribute):
                if child.GetName() == 'class':
                    continue
            yield child
