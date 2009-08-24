#!/usr/bin/env python
from __future__ import with_statement
import xml.dom.minidom

from csp.tools.layout2 import layout_module

class UnknownXmlNode(Exception):
	pass

class InconsistencyInXmlNodeTree(Exception):
	pass

class XmlNodeFactory:
	def __init__(self, documentOwner):
		self.documentOwner = documentOwner
	
	def CreateXmlNode(self, parent, domNode):
		if domNode.nodeType == domNode.ELEMENT_NODE:
			return XmlNodeElement(parent, self.documentOwner)
		elif domNode.nodeType == domNode.TEXT_NODE or domNode.nodeType == domNode.CDATA_SECTION_NODE:
			return XmlNodeText(parent)
		elif domNode.nodeType == domNode.COMMENT_NODE:
			return XmlNodeComment(parent)
		else:
			raise UnknownXmlNode()

class XmlNodeDocument(layout_module.XmlNodeDocument):
	def __init__(self, documentOwner, *args, **kwargs):
		layout_module.XmlNodeDocument.__init__(self, *args, **kwargs)
		
		# The XmlObjectDocument that own this node
		self.documentOwner = documentOwner
		
		# The xml.dom.Document
		self.domNode = None
	
		# The XmlNode children
		self.nodesBeforeRootElement = []
		self.rootElement = None
		self.nodesAfterRootElement = []
		
		# Incremented each time a change is made on this node
		self.changeCount = 0
		
		# Incremented each time a change is made on a children
		self.childrenChangeCount = 0
		
		# Errors detected on this node
		self.errors = {}
	
	def Dispose(self):
		for child in self.nodesBeforeRootElement:
			child.Dispose()
		self.nodesBeforeRootElement = []
		
		if self.rootElement is not None:
			self.rootElement.Dispose()
			self.rootElement = None
		
		for child in self.nodesAfterRootElement:
			child.Dispose()
		self.nodesAfterRootElement = []
		
		if self.domNode is not None:
			self.domNode.unlink()
			self.domNode = None
	
	def IncrementChangeCount(self):
		self.changeCount += 1
	
	def IncrementChildrenChangeCount(self):
		self.childrenChangeCount += 1
	
	def New(self, rootElementName):
		self.Dispose()
		
		domImplementation = xml.dom.minidom.getDOMImplementation()
		self.domNode = domImplementation.createDocument(None, rootElementName, None)
		
		self.nodesBeforeRootElement = []
		self.rootElement = self.documentOwner.NodeFactory().CreateXmlNode(self, self.domNode.documentElement)
		self.nodesAfterRootElement = []
		
		self.CheckErrors()
		self.IncrementChangeCount()
	
	def Load(self, fileName):
		self.Dispose()
		
		self.domNode = xml.dom.minidom.parse( fileName )
		
		currentList = self.nodesBeforeRootElement
		
		for domChild in self.domNode.childNodes:
			if domChild.nodeType == domChild.ELEMENT_NODE:
				# We found the root element
				currentList = self.nodesAfterRootElement
				self.rootElement = self.documentOwner.NodeFactory().CreateXmlNode(self, domChild)
				self.rootElement.Load(domChild)
			else:
				child = self.documentOwner.NodeFactory().CreateXmlNode(self, domChild)
				currentList.append(child)
				child.Load(domChild)
		
		self.CheckErrors()
		self.IncrementChangeCount()
	
	def CheckErrors(self):
		pass
	
	def Save(self, fileName):
		if self.domNode is not None:
			with open(fileName, 'w+') as file:
				file.write( self.domNode.toprettyxml() )
	
	def GetRootElement(self):
		return self.rootElement
	
	def ReplaceChildElement(self, oldChild, newChild):
		if oldChild is not self.rootElement:
			raise InconsistencyInXmlNodeTree
		
		oldChild.Dispose()
		self.rootElement = newChild
		self.CheckErrors()
		self.IncrementChangeCount()
	
	def GetChildren(self):
		for child in self.nodesBeforeRootElement:
			yield child
		
		if self.rootElement is not None:
			yield self.rootElement
		
		for child in self.nodesAfterRootElement:
			yield child


class XmlNodeElement(layout_module.XmlNodeElement):
	def __init__(self, parent, documentOwner, *args, **kwargs):
		if ( type(self) is XmlNodeElement ):
			layout_module.XmlNodeElement.__init__(self, *args, **kwargs)
		
		# The XmlNode parent
		self.parent = parent
		
		# The XmlObjectDocument that own this node
		self.documentOwner = documentOwner
		
		# The xml.dom.Element
		self.domNode = None
		
		# The XmlNode children
		self.childNodes = []
		
		# Incremented each time a change is made on this node
		self.changeCount = 0
		
		# Incremented each time a change is made on a children
		self.childrenChangeCount = 0
		
		# Errors detected on this node
		self.errors = {}
	
	def TakeFrom(self, xmlNodeElement):
		"""Copy all the data from xmlNodeElement.
		Re-parent xmlNodeElement children to self.
		Clean xmlNodeElement children list to ensure that Dispose will not
		be propagated to its children.
		Should be inherited by specialisation to ensure that all data are copied.
		"""
		
		self.Dispose()
		
		self.domNode = xmlNodeElement.domNode
		
		# Clone the list
		self.childNodes = xmlNodeElement.childNodes[:]
		
		# Re-parent the children
		for child in self.childNodes:
			child.parent = self
		
		# Clean the source element
		xmlNodeElement.childNodes = []
		
		# Copy all remaining member data
		self.changeCount = xmlNodeElement.changeCount
		self.childrenChangeCount = xmlNodeElement.childrenChangeCount
		self.errors = xmlNodeElement.errors
	
	def Dispose(self):
		for child in self.childNodes:
			child.Dispose()
		self.childNodes = []
	
	def IncrementChangeCount(self):
		self.changeCount += 1
		self.parent.IncrementChildrenChangeCount()
	
	def IncrementChildrenChangeCount(self):
		self.childrenChangeCount += 1
		self.parent.IncrementChildrenChangeCount()
	
	def Load(self, domNode):
		self.Dispose()
		
		self.domNode = domNode
		
		for domChild in self.domNode.childNodes:
			child = self.documentOwner.NodeFactory().CreateXmlNode(self, domChild)
			self.childNodes.append(child)
			child.Load(domChild)
		
		self.CheckErrors()
	
	def CheckErrors(self):
		pass
	
	def Rename(self, newName):
		if self.domNode.tagName == newName:
			return
		
		self.domNode.tagName = newName
		
		# Now, we must replace ourself by an object which manage the new tagName
		newElement = self.documentOwner.NodeFactory().CreateXmlNode(self.parent, self.domNode)
		newElement.TakeFrom(self)
		self.parent.ReplaceChildElement(self, newElement)
		newElement.CheckErrors()
		newElement.IncrementChangeCount()
	
	def ReplaceChildElement(self, oldChild, newChild):
		if oldChild not in self.childNodes:
			raise InconsistencyInXmlNodeTree
		
		oldChild.Dispose()
		childIndex = self.childNodes.index(oldChild)
		self.childNodes[childIndex] = newChild
		self.CheckErrors()
		self.IncrementChangeCount()
	
	def GetChildren(self):
		return self.childNodes


class XmlNodeText(layout_module.XmlNode):
	def __init__(self, parent, *args, **kwargs):
		layout_module.XmlNode.__init__(self, *args, **kwargs)
		
		# The XmlNode parent
		self.parent = parent
		
		# The xml.dom.Text
		self.domNode =  None
		
		# Incremented each time a change is made on this node
		self.changeCount = 0
		
		# Errors detected on this node
		self.errors = {}
	
	def Dispose(self):
		pass
	
	def IncrementChangeCount(self):
		self.changeCount += 1
		self.parent.IncrementChildrenChangeCount()
	
	def Load(self, domNode):
		self.domNode = domNode
	
	def GetText(self):
		return str(self.domNode.data)


class XmlNodeComment(layout_module.XmlNode):
	def __init__(self, parent, *args, **kwargs):
		layout_module.XmlNode.__init__(self, *args, **kwargs)
		
		# The XmlNode parent
		self.parent = parent
		
		# The xml.dom.Comment
		self.domNode =  None
		
		# Incremented each time a change is made on this node
		self.changeCount = 0
		
		# Errors detected on this node
		self.errors = {}
	
	def Dispose(self):
		pass
	
	def IncrementChangeCount(self):
		self.changeCount += 1
		self.parent.IncrementChildrenChangeCount()
	
	def Load(self, domNode):
		self.domNode = domNode
	
	def GetText(self):
		return str(self.domNode.data)
