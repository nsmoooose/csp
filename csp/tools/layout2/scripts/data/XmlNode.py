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
		elif domNode.nodeType == domNode.ATTRIBUTE_NODE:
			return XmlNodeAttribute(parent)
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
		
		# Nb errors detected on children
		self.childrenErrorCount = 0
	
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
	
	def SetError(self, id, text):
		if text is None:
			if id in self.errors:
				del self.errors[id]
				self.IncrementChangeCount()
		else:
			if id in self.errors:
				if self.errors[id] != text:
					self.errors[id] = text
					self.IncrementChangeCount()
			else:
				self.errors[id] = text
				self.IncrementChangeCount()
	
	def ChangeChildrenErrorCount(self, childErrorCount):
		self.childrenErrorCount += childErrorCount
	
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


class XmlNodeChild(object):
	def __init__(self, parent):
		# The XmlNode parent
		self.parent = parent
		
		# Incremented each time a change is made on this node
		self.changeCount = 0
		
		# Errors detected on this node
		self.errors = {}
	
	def TakeFrom(self, xmlNodeChild):
		"""Copy all the data from xmlNodeChild."""
		
		self.changeCount = xmlNodeElement.changeCount
		self.errors = xmlNodeElement.errors
	
	def IncrementChangeCount(self):
		self.changeCount += 1
		self.parent.IncrementChildrenChangeCount()
	
	def SetError(self, id, text):
		errorCountDelta = 0
		
		if text is None:
			if id in self.errors:
				del self.errors[id]
				errorCountDelta = -1
				self.IncrementChangeCount()
		else:
			if id in self.errors:
				if self.errors[id] != text:
					self.errors[id] = text
					self.IncrementChangeCount()
			else:
				self.errors[id] = text
				errorCountDelta = 1
				self.IncrementChangeCount()
		
		self.parent.ChangeChildrenErrorCount(errorCountDelta)


class XmlNodeElement(layout_module.XmlNodeElement, XmlNodeChild):
	def __init__(self, parent, documentOwner, *args, **kwargs):
		if ( type(self) is XmlNodeElement ):
			layout_module.XmlNodeElement.__init__(self, *args, **kwargs)
		XmlNodeChild.__init__(self, parent)
		
		# The XmlObjectDocument that own this node
		self.documentOwner = documentOwner
		
		# The xml.dom.Element
		self.domNode = None
		
		# The XmlNode attributes
		self.attributes = {}
		
		# The XmlNode children
		self.childNodes = []
		
		# Incremented each time a change is made on a children
		self.childrenChangeCount = 0
		
		# Nb errors detected on children
		self.childrenErrorCount = 0
	
	def TakeFrom(self, xmlNodeElement):
		"""Copy all the data from xmlNodeElement.
		Re-parent xmlNodeElement children to self.
		Clean xmlNodeElement children list to ensure that Dispose will not
		be propagated to its children.
		Should be inherited by specialisation to ensure that all data are copied.
		"""
		
		self.Dispose()
		
		# Take the xml.dom.Element
		self.domNode = xmlNodeElement.domNode
		xmlNodeElement.domNode = None
		
		# Clone the lists
		self.attributes = xmlNodeElement.attributes
		self.childNodes = xmlNodeElement.childNodes
		
		# Re-parent the children
		for attribute in self.attributes.itervalues():
			attribute.parent = self
		
		for child in self.childNodes:
			child.parent = self
		
		# Clean the source element
		xmlNodeElement.attributes = {}
		xmlNodeElement.childNodes = []
		
		# Copy all remaining member data
		XmlNodeChild.TakeFrom(self, xmlNodeElement)
		self.childrenChangeCount = xmlNodeElement.childrenChangeCount
		self.childrenErrorCount = xmlNodeElement.childrenErrorCount
	
	def Dispose(self):
		for attribute in self.attributes.itervalues():
			attribute.Dispose()
		self.attributes = {}
		
		for child in self.childNodes:
			child.Dispose()
		self.childNodes = []
	
	def IncrementChildrenChangeCount(self):
		self.childrenChangeCount += 1
		self.parent.IncrementChildrenChangeCount()
	
	def Load(self, domNode):
		self.Dispose()
		
		self.domNode = domNode
		self.domNode.normalize()
		
		domAttributes = self.domNode.attributes
		for attributeIndex in range(0, domAttributes.length):
			domAttribute = domAttributes.item(attributeIndex)
			child = self.documentOwner.NodeFactory().CreateXmlNode(self, domAttribute)
			self.attributes[domAttribute.name] = child
			child.Load(domAttribute)
		
		for domChild in self.domNode.childNodes:
			child = self.documentOwner.NodeFactory().CreateXmlNode(self, domChild)
			self.childNodes.append(child)
			child.Load(domChild)
		
		self.CombineTextNodes()
		self.CheckErrors()
	
	def CombineTextNodes(self):
		lastNodeText = None
		nodeTextToRemove = []
		for child in self.childNodes:
			if isinstance(child, XmlNodeText):
				if child.GetText() == '':
					nodeTextToRemove.append(child)
				elif lastNodeText is None:
					lastNodeText = child
				else:
					lastNodeText.CombineWith(child)
					nodeTextToRemove.append(child)
			else:
				lastNodeText = None
		
		for child in nodeTextToRemove:
			del self.childNodes[ self.childNodes.index(child) ]
			self.domNode.removeChild( child.domNode )
			child.Dispose()
	
	def CheckErrors(self):
		pass
	
	def ChangeChildrenErrorCount(self, childErrorCount):
		self.childrenErrorCount += childErrorCount
		self.parent.ChangeChildrenErrorCount(childErrorCount)
	
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
	
	def GetAttributeValue(self, name):
		attribute = self.attributes.get(name)
		if attribute is None:
			return ""
		else:
			return attribute.GetValue()
	
	def GetChildren(self):
		for attribute in self.attributes.itervalues():
			yield attribute
		
		for child in self.childNodes:
			yield child


class XmlNodeData(layout_module.XmlNode, XmlNodeChild):
	def __init__(self, parent, *args, **kwargs):
		layout_module.XmlNode.__init__(self, *args, **kwargs)
		XmlNodeChild.__init__(self, parent)
		
		# The xml.dom.Text or xml.dom.Comment
		self.domNode =  None
		
		# Trimmed text
		self.text = ""
	
	def Dispose(self):
		pass
	
	def Load(self, domNode):
		self.domNode = domNode
		self.StripText()
	
	def StripText(self):
		lines = str( self.domNode.data ).strip().splitlines()
		self.text = '\n'.join( line.strip() for line in lines )
	
	def GetText(self):
		return self.text


class XmlNodeText(XmlNodeData):
	def __init__(self, *args, **kwargs):
		XmlNodeData.__init__(self, *args, **kwargs)
	
	def CombineWith(self, nodeText):
		self.domNode.data += nodeText.domNode.data
		self.StripText()


class XmlNodeComment(XmlNodeData):
	def __init__(self, *args, **kwargs):
		XmlNodeData.__init__(self, *args, **kwargs)


class XmlNodeAttribute(layout_module.XmlNode, XmlNodeChild):
	def __init__(self, parent, *args, **kwargs):
		layout_module.XmlNode.__init__(self, *args, **kwargs)
		XmlNodeChild.__init__(self, parent)
		
		# The xml.dom.Attr
		self.domNode =  None
	
	def Dispose(self):
		pass
	
	def Load(self, domNode):
		self.domNode = domNode
	
	def GetName(self):
		return str(self.domNode.name)
	
	def GetValue(self):
		return str(self.domNode.nodeValue)
