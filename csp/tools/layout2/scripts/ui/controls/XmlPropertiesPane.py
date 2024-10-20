import wx

from .DocumentNotebook import DocumentNotebook
from .FilePropertiesPane import FilePropertiesPane
from . import XmlPropertiesPaneItem
from . import XmlPropertiesPaneItemArchive

class XmlPropertiesPane(FilePropertiesPane):
    imageList = None
    imageListItems = {}
    imageListItemNames = {
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
    
    def __init__(self, parent, document, *args, **kwargs):
        FilePropertiesPane.__init__(self, parent, document, rootLabel = "XML document", *args, **kwargs)
        
        DocumentNotebook.Instance.GetPageUnselectedSignal().Connect(self.on_PageUnselectedSignal)
        DocumentNotebook.Instance.GetPageSelectedSignal().Connect(self.on_PageSelectedSignal)
        
        wx.GetApp().GetDocumentRegistry().ReferenceDocument(self.document)
        self.document.GetChangedSignal().Connect(self.on_DocumentChanged)
        
        self.InitItemForXmlNode(self.root, self.document.GetXmlNodeDocument(), 0)
        self.tree.Bind(wx.EVT_TREE_ITEM_EXPANDING, self.on_TreeItemExpanding)
        self.tree.Bind(wx.EVT_TREE_SEL_CHANGED, self.on_TreeSelChanged)
        
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
            XmlPropertiesPaneItemArchive.ItemUpdaterVector2(self),
            XmlPropertiesPaneItemArchive.ItemUpdaterVector3(self),
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
    
    def Dispose(self):
        DocumentNotebook.Instance.GetPageUnselectedSignal().Disconnect(self.on_PageUnselectedSignal)
        DocumentNotebook.Instance.GetPageSelectedSignal().Disconnect(self.on_PageSelectedSignal)
        
        self.document.GetChangedSignal().Disconnect(self.on_DocumentChanged)
        wx.GetApp().GetDocumentRegistry().ReleaseDocument(self.document)
        self.document = None
    
    def SetImageList(self, imageList):
        XmlPropertiesPane.imageList = imageList
    
    def GetImageList(self):
        return XmlPropertiesPane.imageList
    
    def TreeImages(self):
        return XmlPropertiesPane.imageListItems
    
    def ImageListItemNames(self):
        return self.imageListItemNames
    
    def on_PageUnselectedSignal(self, page):
        page.externalData['XmlPropertiesPane'] = self.SaveTreeItemsState(self.root)
    
    def on_PageSelectedSignal(self, page):
        self.RestoreTreeItemsState( self.root, page.externalData.get('XmlPropertiesPane') )
    
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
    
    def on_TreeSelChanged(self, event):
        item = event.GetItem()
        node = item.xmlNode
        documentRegistry = wx.GetApp().GetDocumentRegistry()
        documentRegistry.SetActiveDocument( node.documentOwner )
    
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
        itemState = ItemState()
        if self.tree.IsSelected(item):
            itemState.isSelected = True
        if self.tree.IsExpanded(item):
            itemState.isExpanded = True
            for child in self.GetItemChildren(item):
                childItemState = self.SaveTreeItemsState(child)
                if childItemState.HasNonDefaultValue():
                    itemState.childrenState[child.xmlNode] = childItemState
        return itemState
    
    def RestoreTreeItemsState(self, item, itemState):
        if itemState is None:
            return
        if itemState.isSelected:
            self.SelectItem(item)
        if itemState.isExpanded:
            self.tree.Expand(item)
            defaultItemState = ItemState()
            for child in self.GetItemChildren(item):
                childItemState = itemState.childrenState.get(child.xmlNode, defaultItemState)
                self.RestoreTreeItemsState(child, childItemState)
        else:
            self.tree.Collapse(item)
    
    def GetItemChildren(self, item):
        child, unused = self.tree.GetFirstChild(item)
        while child is not None:
            yield child
            child = self.tree.GetNextSibling(child)
    
    def SelectItem(self, item):
        # Workaround for a refresh bug in wx.lib.agw.customtreectrl.CustomTreeCtrl.EnsureVisible
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
