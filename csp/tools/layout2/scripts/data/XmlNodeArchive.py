#!/usr/bin/env python
from XmlNode import XmlNodeFactory
from XmlNode import XmlNodeDocument
from XmlNode import XmlNodeElement
from XmlNode import XmlNodeText
from XmlNode import XmlNodeComment
from csp.tools.layout2 import layout_module
from csp import csplib

class XmlNodeArchiveFactory(XmlNodeFactory):
	def CreateXmlNode(self, parent, domNode):
		if domNode.nodeType == domNode.ELEMENT_NODE:
			if domNode.tagName == "Object":
				return XmlNodeObject(parent, self.documentOwner)
			elif domNode.tagName == "String":
				return XmlNodeString(parent, self.documentOwner)
		
		return XmlNodeFactory.CreateXmlNode(self, parent, domNode)

class XmlNodeObjectDocument(XmlNodeDocument):
	def CheckErrors(self):
		if isinstance(self.rootElement, XmlNodeObject):
			self.rootElement.SetError("XmlNodeObjectDocument", None)
		else:
			self.rootElement.SetError("XmlNodeObjectDocument", "Root element should be <Object>")


class XmlNodeObject(layout_module.XmlNodeObject, XmlNodeElement):
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeObject.__init__(self, *args, **kwargs)
		XmlNodeElement.__init__(self, parent, documentOwner, *args, **kwargs)
	
	def CheckErrors(self):
		if 'class' in self.attributes:
			self.SetError("MissingClassAttribute", None)
		else:
			self.SetError("MissingClassAttribute", "<Object> must have a class attribute")
			return
		
		interfaceRegistry = csplib.InterfaceRegistry.getInterfaceRegistry()
		interface = interfaceRegistry.getInterface( self.attributes['class'].GetValue() )
		
		if interface is None:
			self.SetError("UnknownClassAttribute", "<Object> has an unknown class attribute")
		else:
			self.SetError("UnknownClassAttribute", None)


class XmlNodeString(layout_module.XmlNodeString, XmlNodeElement):
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeString.__init__(self, *args, **kwargs)
		XmlNodeElement.__init__(self, parent, documentOwner, *args, **kwargs)
	
	def CheckErrors(self):
		for child in self.childNodes:
			if isinstance( child, (XmlNodeText, XmlNodeComment) ):
				child.SetError("XmlNodeString", None)
			else:
				child.SetError("XmlNodeString", "Only text and comments are allowed in <String>")
