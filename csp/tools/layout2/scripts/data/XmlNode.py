#!/usr/bin/env python
from __future__ import with_statement
import xml.dom.minidom

from csp.tools.layout2 import layout_module

class XmlNodeFactory:
	def CreateXmlNode(self, documentOwner, domNode):
		if domNode.nodeType == domNode.ELEMENT_NODE:
			return XmlNodeElement(documentOwner, domNode, self)
		elif domNode.nodeType == domNode.TEXT_NODE or domNode.nodeType == domNode.CDATA_SECTION_NODE:
			return XmlNodeText(domNode)
		elif domNode.nodeType == domNode.COMMENT_NODE:
			return XmlNodeComment(domNode)

class XmlNodeDocument(layout_module.XmlNodeDocument):
	def __init__(self, documentOwner, fileName, nodeFactory = XmlNodeFactory(), *args, **kwargs):
		layout_module.XmlNodeDocument.__init__(self, *args, **kwargs)
		
		# The XmlObjectDocument that own this node
		self.documentOwner = documentOwner
		
		# The xml.dom.Document
		self.domNode = xml.dom.minidom.parse( fileName )
		
		self.nodesBeforeRootElement = []
		self.nodesAfterRootElement = []
		
		currentList = self.nodesBeforeRootElement
		
		for child in self.domNode.childNodes:
			if child.nodeType == child.COMMENT_NODE:
				currentList.append( XmlNodeComment(child) )
			
			elif child.nodeType == child.ELEMENT_NODE:
				# We found the root element
				currentList = self.nodesAfterRootElement
				self.rootElement = nodeFactory.CreateXmlNode(self.documentOwner, child)
	
	def Dispose(self):
		self.domNode.unlink()
		self.domNode = None
	
	def SaveTo(self, fileName):
		with open(fileName, 'w+') as file:
			file.write( self.domNode.toprettyxml() )
	
	def GetRootElement(self):
		return self.rootElement


class XmlNodeElement(layout_module.XmlNodeElement):
	def __init__(self, documentOwner, domNode, nodeFactory, *args, **kwargs):
		if ( type(self) is XmlNodeElement ):
			layout_module.XmlNodeElement.__init__(self, *args, **kwargs)
		
		# The XmlObjectDocument that own this node
		self.documentOwner = documentOwner
		
		# The xml.dom.Document
		self.domNode = domNode
		
		self.childNodes = []
		
		for child in self.domNode.childNodes:
			self.childNodes.append( nodeFactory.CreateXmlNode(self.documentOwner, child) )


class XmlNodeText(layout_module.XmlNode):
	def __init__(self, domNode, *args, **kwargs):
		layout_module.XmlNode.__init__(self, *args, **kwargs)
		
		# The xml.dom.Document
		self.domNode = domNode


class XmlNodeComment(layout_module.XmlNode):
	def __init__(self, domNode, *args, **kwargs):
		layout_module.XmlNode.__init__(self, *args, **kwargs)
		
		# The xml.dom.Document
		self.domNode = domNode
