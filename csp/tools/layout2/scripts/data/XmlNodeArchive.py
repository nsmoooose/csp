#!/usr/bin/env python
from XmlNode import XmlNodeFactory
from XmlNode import XmlNodeDocument
from XmlNode import XmlNodeElement
from XmlNode import XmlNodeText
from XmlNode import XmlNodeComment
from XmlNode import XmlNodeChild
from csp.tools.layout2 import layout_module
from csp import csplib

class XmlNodeArchiveFactory(XmlNodeFactory):
	def CreateXmlNode(self, parent, domNode):
		if domNode.nodeType == domNode.ELEMENT_NODE:
			for NodeClass in [
				XmlNodeString,
				XmlNodeBool,
				XmlNodeInt,
				XmlNodeFloat,
				XmlNodeReal,
				XmlNodeECEF,
				XmlNodeLLA,
				XmlNodeUTM,
				XmlNodeVector,
				XmlNodeMatrix,
				XmlNodeQuat,
				XmlNodeDate,
				XmlNodeEnum,
				XmlNodeExternal,
				XmlNodeKey,
				XmlNodePath,
				XmlNodeList,
				XmlNodeObject,
				]:
				if domNode.tagName == NodeClass.tag:
					return NodeClass(parent, self.documentOwner)
		
		return super(XmlNodeArchiveFactory, self).CreateXmlNode(parent, domNode)

class XmlNodeObjectDocument(XmlNodeDocument):
	def CheckErrors(self):
		super(XmlNodeObjectDocument, self).CheckErrors()
		
		if isinstance(self.rootElement, XmlNodeObject):
			self.rootElement.SetError("XmlNodeObjectDocument", None)
		else:
			self.rootElement.SetError("XmlNodeObjectDocument", "Root element should be <Object>")


class XmlNodeArchive(XmlNodeElement):
	def GetText(self):
		texts = []
		for child in self.childNodes:
			if isinstance(child, XmlNodeText):
				texts.append( child.GetText() )
		return '\n'.join(texts)


class XmlNodeSimple(XmlNodeArchive):
	def CheckErrors(self):
		super(XmlNodeSimple, self).CheckErrors()
		
		for child in self.childNodes:
			if isinstance( child, (XmlNodeText, XmlNodeComment) ):
				child.SetError("XmlNodeSimple", None)
			else:
				child.SetError("XmlNodeSimple", "Only text and comments are allowed in <%s>" % self.tag)


class XmlNodeString(layout_module.XmlNodeString, XmlNodeSimple):
	tag = 'String'
	variableTypes = ['string']
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeString.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeBool(layout_module.XmlNodeBool, XmlNodeSimple):
	tag = 'Bool'
	variableTypes = ['bool']
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeBool.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)
	
	def CheckErrors(self):
		super(XmlNodeBool, self).CheckErrors()
		
		if self.GetText().lower() in ['true', 'false']:
			self.SetError( "parseXML", None )
		else:
			self.SetError( "parseXML", "Only true or false are allowed in <%s>" % self.tag )


class XmlNodeInt(layout_module.XmlNodeInt, XmlNodeSimple):
	tag = 'Int'
	variableTypes = ['int8', 'uint8', 'int16', 'uint16', 'int', 'uint']
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeInt.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeFloat(layout_module.XmlNodeFloat, XmlNodeSimple):
	tag = 'Float'
	variableTypes = ['float', 'double']
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeFloat.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeReal(layout_module.XmlNodeReal, XmlNodeSimple):
	tag = 'Real'
	variableType = 'Real'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeReal.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeECEF(layout_module.XmlNodeECEF, XmlNodeSimple):
	tag = 'ECEF'
	variableType = 'ECEF'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeECEF.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeLLA(layout_module.XmlNodeLLA, XmlNodeSimple):
	tag = 'LLA'
	variableType = 'LLA'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeLLA.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeUTM(layout_module.XmlNodeUTM, XmlNodeSimple):
	tag = 'UTM'
	variableType = 'UTM'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeUTM.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeVector(layout_module.XmlNodeVector, XmlNodeSimple):
	tag = 'Vector'
	variableType = 'Vector3'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeVector.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)
	
	def CheckErrors(self):
		super(XmlNodeVector, self).CheckErrors()
		
		vector = csplib.Vector3()
		try:
			vector.parseXML( self.GetText() )
			self.SetError( "parseXML", None )
		except csplib.ParseException, error:
			self.SetError( "parseXML", error.getMessage() )
	
	def GetStringValues(self):
		values = self.GetText().split(None, 2)
		for index in range(len(values), 3):
			values.append('')
		return values


class XmlNodeMatrix(layout_module.XmlNodeMatrix, XmlNodeSimple):
	tag = 'Matrix'
	variableType = 'Matrix3'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeMatrix.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)
	
	def CheckErrors(self):
		super(XmlNodeMatrix, self).CheckErrors()
		
		matrix = csplib.Matrix3()
		try:
			matrix.parseXML( self.GetText() )
			self.SetError( "parseXML", None )
		except csplib.ParseException, error:
			self.SetError( "parseXML", error.getMessage() )
	
	def GetStringValues(self):
		values = self.GetText().split(None, 8)
		for index in range(len(values), 9):
			values.append('')
		return values


class XmlNodeQuat(layout_module.XmlNodeQuat, XmlNodeSimple):
	tag = 'Quat'
	variableType = 'Quat'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeQuat.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeDate(layout_module.XmlNodeDate, XmlNodeSimple):
	tag = 'Date'
	variableType = 'SimDate'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeDate.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeEnum(layout_module.XmlNodeEnum, XmlNodeSimple):
	tag = 'Enum'
	variableType = 'Enum'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeEnum.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeExternal(layout_module.XmlNodeExternal, XmlNodeSimple):
	tag = 'External'
	variableType = 'External'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeExternal.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeKey(layout_module.XmlNodeKey, XmlNodeSimple):
	tag = 'Key'
	variableType = 'Key'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeKey.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodePath(layout_module.XmlNodePath, XmlNodeSimple):
	tag = 'Path'
	variableType = 'Path'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodePath.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeList(layout_module.XmlNodeList, XmlNodeArchive):
	tag = 'List'
	variableType = 'vector'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeList.__init__(self, *args, **kwargs)
		XmlNodeArchive.__init__(self, parent, documentOwner, *args, **kwargs)
		
		# Contains the items of the list when it has a type attribute
		self.textItems = []
	
	def Dispose(self):
		super(XmlNodeList, self).Dispose()
		self.textItems = []
	
	def PostLoad(self):
		typeAttribute = self.attributes.get('type')
		if typeAttribute:
			typeAttributeValue = typeAttribute.GetValue()
			for ItemClass in [
				XmlNodeListTextItemInt,
				XmlNodeListTextItemFloat,
				XmlNodeListTextItemReal,
				XmlNodeListTextItemExternal,
				XmlNodeListTextItemKey,
				XmlNodeListTextItemPath,
				]:
				if typeAttributeValue == ItemClass.type:
					for value in self.GetText().split():
						self.textItems.append( ItemClass(self, value) )
					break
	
	def GetChildren(self):
		for textItem in self.textItems:
			yield textItem
		
		for child in super(XmlNodeList, self).GetChildren():
			yield child
	
	# TODO: CheckErrors


class XmlNodeListTextItem(XmlNodeChild):
	def __init__(self, parent, value):
		XmlNodeChild.__init__(self, parent)
		self.value = value
	
	def GetChildren(self):
		return []
	
	def GetText(self):
		return self.value


class XmlNodeListTextItemInt(XmlNodeListTextItem):
	tag = 'Int'
	type = 'int'


class XmlNodeListTextItemFloat(XmlNodeListTextItem):
	tag = 'Float'
	type = 'float'


class XmlNodeListTextItemReal(XmlNodeListTextItem):
	tag = 'Real'
	type = 'real'


class XmlNodeListTextItemExternal(XmlNodeListTextItem):
	tag = 'External'
	type = 'external'


class XmlNodeListTextItemKey(XmlNodeListTextItem):
	tag = 'Key'
	type = 'key'


class XmlNodeListTextItemPath(XmlNodeListTextItem):
	tag = 'Path'
	type = 'path'


class XmlNodeObject(layout_module.XmlNodeObject, XmlNodeArchive):
	tag = 'Object'
	variableType = 'Link'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeObject.__init__(self, *args, **kwargs)
		XmlNodeArchive.__init__(self, parent, documentOwner, *args, **kwargs)
		
		# The XmlNode children with a name attribute.
		# If more than one child has the same name attribute,
		# only the first is referenced by namedChildNodes.
		self.namedChildNodes = {}
	
	def Dispose(self):
		super(XmlNodeObject, self).Dispose()
		self.namedChildNodes = {}
	
	def CheckErrors(self):
		super(XmlNodeObject, self).CheckErrors()
		
		childErrors = ["XmlNodeObject.unknownName"]
		
		self.namedChildNodes = {}
		for child in self.childNodes:
			if isinstance(child, XmlNodeArchive):
				nameAttribute = child.GetAttributeValue('name')
				if nameAttribute:
					if nameAttribute in self.namedChildNodes:
						child.SetError("XmlNodeObject.duplicatedName", "The variable %s is already defined" % nameAttribute)
						continue
					else:
						self.namedChildNodes[nameAttribute] = child
			child.SetError("XmlNodeObject.duplicatedName", None)
		
		classAttribute = self.attributes.get('class')
		if classAttribute is None:
			self.SetError("MissingClassAttribute", "<%s> must have a class attribute" % self.tag)
			self.CleanChildErrors(childErrors)
			return
		else:
			self.SetError("MissingClassAttribute", None)
		
		classAttributeValue = classAttribute.GetValue()
		interfaceRegistry = csplib.InterfaceRegistry.getInterfaceRegistry()
		interface = interfaceRegistry.getInterface(classAttributeValue)
		
		if interface is None:
			self.SetError("UnknownClassAttribute", "<%s> has an unknown class attribute" % self.tag)
			self.CleanChildErrors(childErrors)
			return
		else:
			self.SetError("UnknownClassAttribute", None)
		
		for requiredName in interface.getRequiredNames():
			if requiredName in self.namedChildNodes:
				self.SetError("requiredName_%s" % requiredName, None)
			else:
				self.SetError("requiredName_%s" % requiredName, "Required variable %s is missing" % requiredName)
		
		for child in self.childNodes:
			if not isinstance(child, XmlNodeArchive):
				child.SetError("XmlNodeObject.unknownName", None)
				child.SetError("XmlNodeObject.badType", None)
				continue
			
			nameAttribute = child.GetAttributeValue('name')
			if not nameAttribute:
				child.SetError("XmlNodeObject.unknownName", None)
				child.SetError("XmlNodeObject.badType", None)
				continue
			
			if interface.variableExists(nameAttribute):
				child.SetError("XmlNodeObject.unknownName", None)
			else:
				child.SetError( "XmlNodeObject.unknownName", "%s is not a variable of %s" % (nameAttribute, classAttributeValue) )
				child.SetError( "XmlNodeObject.badType", None )
				continue
			
			variableType = interface.variableType(nameAttribute).split('::')
			
			if variableType[0] == 'builtin':
				variableType = variableType[1]
				found = False
				for NodeClass in [
					XmlNodeString,
					XmlNodeBool,
					XmlNodeInt,
					XmlNodeFloat,
					]:
					if variableType in NodeClass.variableTypes:
						found = True
						self.CheckChildType(child, nameAttribute, NodeClass)
				
				if not found:
					child.SetError( "XmlNodeObject.badType", None )
					print "BUG: unknown archive builtin type"
			elif variableType[0] == 'type':
				variableType = variableType[1]
				found = False
				for NodeClass in [
					XmlNodeReal,
					XmlNodeECEF,
					XmlNodeLLA,
					XmlNodeUTM,
					XmlNodeVector,
					XmlNodeMatrix,
					XmlNodeQuat,
					XmlNodeDate,
					XmlNodeEnum,
					XmlNodeExternal,
					XmlNodeKey,
					XmlNodePath,
					]:
					if variableType in NodeClass.variableType:
						found = True
						self.CheckChildType(child, nameAttribute, NodeClass)
				
				if not found:
					if variableType == XmlNodeObject.variableType:
						found = True
						if isinstance(child, XmlNodeObject):
							child.SetError( "XmlNodeObject.badType", None )
						elif isinstance(child, XmlNodePath):
							child.SetError( "XmlNodeObject.badType", None )
							# TODO: check <Object> referenced by <Path>
						else:
							child.SetError( "XmlNodeObject.badType", "Variable %s must be of type <%s> or <%s>" % (nameAttribute, XmlNodeObject.tag, XmlNodePath.tag) )
				
				if not found:
					child.SetError( "XmlNodeObject.badType", None )
					print "BUG: unknown archive simple type"
			elif variableType[0] == XmlNodeList.variableType:
				self.CheckChildType(child, nameAttribute, XmlNodeList)
	
	def CleanChildErrors(self, errors):
		for child in self.childNodes:
			for error in errors:
				child.SetError(error, None)
	
	def CheckChildType(self, child, nameAttribute, NodeClass):
		if isinstance(child, NodeClass):
			child.SetError( "XmlNodeObject.badType", None )
		else:
			child.SetError( "XmlNodeObject.badType", "Variable %s must be of type <%s>" % (nameAttribute, NodeClass.tag) )
