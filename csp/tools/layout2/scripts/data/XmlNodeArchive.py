#!/usr/bin/env python
from XmlNode import XmlNodeFactory
from XmlNode import XmlNodeDocument
from XmlNode import XmlNodeElement
from csp.tools.layout2 import layout_module

class XmlNodeArchiveFactory(XmlNodeFactory):
	def CreateXmlNode(self, documentOwner, domNode):
		if domNode.nodeType == domNode.ELEMENT_NODE:
			if domNode.tagName == "Object":
				return XmlNodeObject(documentOwner, domNode, self)
			elif domNode.tagName == "String":
				return XmlNodeString(documentOwner, domNode, self)
		
		return XmlNodeFactory.CreateXmlNode(self, documentOwner, domNode)

class XmlNodeObjectDocument(XmlNodeDocument):
	def __init__(self, *args, **kwargs):
		XmlNodeDocument.__init__(self, nodeFactory = XmlNodeArchiveFactory(), *args, **kwargs)
		
		if not isinstance(self.rootElement, XmlNodeObject):
			# TODO: Add type error to self.rootElement
			pass


class XmlNodeObject(layout_module.XmlNodeObject, XmlNodeElement):
	def __init__(self, documentOwner, domNode, nodeFactory, *args, **kwargs):
		layout_module.XmlNodeObject.__init__(self, *args, **kwargs)
		XmlNodeElement.__init__(self, documentOwner, domNode, nodeFactory, *args, **kwargs)


class XmlNodeString(layout_module.XmlNodeString, XmlNodeElement):
	def __init__(self, documentOwner, domNode, nodeFactory, *args, **kwargs):
		layout_module.XmlNodeString.__init__(self, *args, **kwargs)
		XmlNodeElement.__init__(self, documentOwner, domNode, nodeFactory, *args, **kwargs)
