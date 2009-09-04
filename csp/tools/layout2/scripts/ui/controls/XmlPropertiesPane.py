#!/usr/bin/env python
import wx

from FilePropertiesPane import FilePropertiesPane
from XmlPropertiesPaneItem import ItemUpdaterDocument
from XmlPropertiesPaneItem import ItemUpdaterElement
from XmlPropertiesPaneItem import ItemUpdaterText
from XmlPropertiesPaneItem import ItemUpdaterComment
from XmlPropertiesPaneItem import ItemUpdaterAttribute
from XmlArchivePropertiesPaneItem import ItemUpdaterObject
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
