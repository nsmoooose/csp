import os
import wx
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
				XmlNodeTable1,
				XmlNodeTable2,
				XmlNodeTable3,
				XmlNodeBreaks0,
				XmlNodeBreaks1,
				XmlNodeBreaks2,
				XmlNodeValues,
				XmlNodePath,
				XmlNodeList,
				XmlNodeObject,
				]:
				if domNode.tagName == NodeClass.tag:
					return NodeClass(parent, self.documentOwner)
		
		return super(XmlNodeArchiveFactory, self).CreateXmlNode(parent, domNode)

class XmlNodeObjectDocument(XmlNodeDocument):
	def LoadDependencies(self):
		self.rootElement.LoadDependencies()
	
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
	
	def SetText(self, newText):
		# Set and keep only the first XmlNodeText
		textNodes = []
		for child in self.childNodes:
			if isinstance(child, XmlNodeText):
				textNodes.append(child)
		
		for child in textNodes[1:]:
			self.childNodes.remove( child )
			self.domNode.removeChild( child.domNode )
			child.Dispose()
		
		if textNodes:
			textNodes[0].SetText(newText)
		else:
			newDomNode = self.documentOwner.GetXmlNodeDocument().domNode.createTextNode(newText)
			self.domNode.appendChild(newDomNode)
			newChild = self.documentOwner.NodeFactory().CreateXmlNode(self, newDomNode)
			self.childNodes.append(newChild)
			newChild.Load(newDomNode)
		
		self.CheckErrors()
		self.IncrementChangeCount()
	
	def GetSelfVariableType(self):
		if isinstance(self.parent, XmlNodeContainer):
			return self.parent.GetChildVariableType(self)
		else:
			return ['']
	
	def GetSelfScalarPrototype(self):
		if isinstance(self.parent, XmlNodeContainer):
			return self.parent.GetChildScalarPrototype(self)
		else:
			return None


class XmlNodeContainer(XmlNodeArchive):
	def CleanChildErrors(self, errors):
		for child in self.childNodes:
			for error in errors:
				child.SetError(error, None)
	
	def CheckVariableType(self, childVariableType, child):
		if childVariableType[0] == 'builtin':
			childVariableType = childVariableType[1]
			found = False
			for NodeClass in [
				XmlNodeString,
				XmlNodeBool,
				XmlNodeInt,
				XmlNodeFloat,
				]:
				if childVariableType in NodeClass.variableTypes:
					found = True
					self.CheckChildType(child, NodeClass)
			
			if not found:
				child.SetError( "%s.badType" % self.__class__.__name__, None )
				print "BUG: unknown archive builtin type"
			
			return True
		
		elif childVariableType[0] == 'type':
			childVariableType = childVariableType[1]
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
				XmlNodeTable1,
				XmlNodeTable2,
				XmlNodeTable3,
				XmlNodeBreaks0,
				XmlNodeBreaks1,
				XmlNodeBreaks2,
				XmlNodeValues,
				XmlNodePath,
				]:
				if childVariableType == NodeClass.variableType:
					found = True
					self.CheckChildType(child, NodeClass)
			
			if not found:
				if childVariableType == XmlNodeObject.variableType:
					found = True
					if isinstance(child, XmlNodeObject):
						child.SetError( "%s.badType" % self.__class__.__name__, None )
					elif isinstance(child, XmlNodePath):
						child.SetError( "%s.badType" % self.__class__.__name__, None )
					else:
						child.SetError( "%s.badType" % self.__class__.__name__, "This element must be of type <%s> or <%s>" % (XmlNodeObject.tag, XmlNodePath.tag) )
			
			if not found:
				child.SetError( "%s.badType" % self.__class__.__name__, None )
				print "BUG: unknown archive base type"
			
			return True
		
		else:
			return False
	
	def CheckChildType(self, child, NodeClass):
		if isinstance(child, NodeClass):
			child.SetError( "%s.badType" % self.__class__.__name__, None )
		else:
			child.SetError( "%s.badType" % self.__class__.__name__, "This element must be of type <%s>" % NodeClass.tag )
	
	def GetChildVariableType(self, child):
		return ['']
	
	def GetChildScalarPrototype(self, child):
		return None


class XmlNodeSimple(XmlNodeArchive):
	def CheckErrors(self):
		super(XmlNodeSimple, self).CheckErrors()
		
		for child in self.childNodes:
			if isinstance( child, (XmlNodeText, XmlNodeComment) ):
				child.SetError("XmlNodeSimple", None)
			else:
				child.SetError("XmlNodeSimple", "This element is not allowed in <%s>" % self.tag)


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


class XmlNodeIntData(object):
	def CheckErrors(self):
		try:
			int( self.GetText(), 0 )
			self.SetError( "parseXML", None )
		except ValueError, error:
			self.SetError( "parseXML", str(error) )


class XmlNodeInt(layout_module.XmlNodeInt, XmlNodeIntData, XmlNodeSimple):
	tag = 'Int'
	variableTypes = ['int8', 'uint8', 'int16', 'uint16', 'int', 'uint']
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeInt.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)
	
	def CheckErrors(self):
		XmlNodeIntData.CheckErrors(self)
		XmlNodeSimple.CheckErrors(self)


class XmlNodeFloatData(object):
	def CheckErrors(self):
		try:
			float( self.GetText() )
			self.SetError( "parseXML", None )
		except ValueError, error:
			self.SetError( "parseXML", str(error) )


class XmlNodeFloat(layout_module.XmlNodeFloat, XmlNodeFloatData, XmlNodeSimple):
	tag = 'Float'
	variableTypes = ['float', 'double']
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeFloat.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)
	
	def CheckErrors(self):
		XmlNodeFloatData.CheckErrors(self)
		XmlNodeSimple.CheckErrors(self)


class XmlNodeRealData(object):
	def CheckErrors(self):
		real = csplib.Real()
		try:
			real.parseXML( self.GetText() )
			self.SetError( "parseXML", None )
		except csplib.ParseException, error:
			self.SetError( "parseXML", error.getMessage() )
			error.clear()


class XmlNodeReal(layout_module.XmlNodeReal, XmlNodeRealData, XmlNodeSimple):
	tag = 'Real'
	variableType = 'Real'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeReal.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)
	
	def CheckErrors(self):
		XmlNodeRealData.CheckErrors(self)
		XmlNodeSimple.CheckErrors(self)


class XmlNodeECEF(layout_module.XmlNodeECEF, XmlNodeSimple):
	tag = 'ECEF'
	variableType = 'ECEF'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeECEF.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)
	
	def CheckErrors(self):
		super(XmlNodeECEF, self).CheckErrors()
		
		ecef = csplib.ECEF()
		try:
			ecef.parseXML( self.GetText() )
			self.SetError( "parseXML", None )
		except csplib.ParseException, error:
			self.SetError( "parseXML", error.getMessage() )
			error.clear()
	
	def GetStringValues(self):
		values = self.GetText().split(None, 2)
		for index in range(len(values), 3):
			values.append('')
		return values


class XmlNodeLLA(layout_module.XmlNodeLLA, XmlNodeSimple):
	tag = 'LLA'
	variableType = 'LLA'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeLLA.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)
	
	def CheckErrors(self):
		super(XmlNodeLLA, self).CheckErrors()
		
		lla = csplib.LLA()
		try:
			lla.parseXML( self.GetText() )
			self.SetError( "parseXML", None )
		except csplib.ParseException, error:
			self.SetError( "parseXML", error.getMessage() )
			error.clear()


class XmlNodeUTM(layout_module.XmlNodeUTM, XmlNodeSimple):
	tag = 'UTM'
	variableType = 'UTM'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeUTM.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)
	
	def CheckErrors(self):
		super(XmlNodeUTM, self).CheckErrors()
		
		utm = csplib.UTM()
		try:
			utm.parseXML( self.GetText() )
			self.SetError( "parseXML", None )
		except csplib.ParseException, error:
			self.SetError( "parseXML", error.getMessage() )
			error.clear()


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
			error.clear()
	
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
			error.clear()
	
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
	
	def CheckErrors(self):
		super(XmlNodeQuat, self).CheckErrors()
		
		quat = csplib.Quat()
		try:
			quat.parseXML( self.GetText() )
			self.SetError( "parseXML", None )
		except csplib.ParseException, error:
			self.SetError( "parseXML", error.getMessage() )
			error.clear()


class XmlNodeDate(layout_module.XmlNodeDate, XmlNodeSimple):
	tag = 'Date'
	variableType = 'SimDate'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeDate.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)
	
	def CheckErrors(self):
		super(XmlNodeDate, self).CheckErrors()
		
		simDate = csplib.SimDate()
		try:
			simDate.parseXML( self.GetText() )
			self.SetError( "parseXML", None )
		except csplib.ParseException, error:
			self.SetError( "parseXML", error.getMessage() )
			error.clear()


class XmlNodeEnum(layout_module.XmlNodeEnum, XmlNodeSimple):
	tag = 'Enum'
	variableType = 'Enum'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeEnum.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)
		
		# Contains the enumeration allowed values
		self.allowedValues = ()
	
	def PostLoad(self):
		self.allowedValues = ()
		
		prototype = self.GetSelfScalarPrototype()
		if prototype is None:
			return
		
		try:
			enumLink = prototype.getEnumLink()
			self.allowedValues = enumLink.getEnumeration().eachToken()
		except TypeError:
			pass
	
	def CheckErrors(self):
		super(XmlNodeEnum, self).CheckErrors()
		
		if not self.allowedValues or self.GetText() in self.allowedValues:
			self.SetError( "parseXML", None )
		else:
			self.SetError( "parseXML", "The enumeration value must be chosen amongst %s" % ', '.join(self.allowedValues) )


class XmlNodeExternal(layout_module.XmlNodeExternal, XmlNodeSimple):
	tag = 'External'
	variableType = 'External'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeExternal.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeKeyData(object):
	def CheckErrors(self):
		pass


class XmlNodeKey(layout_module.XmlNodeKey, XmlNodeKeyData, XmlNodeSimple):
	tag = 'Key'
	variableType = 'Key'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeKey.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)
	
	def CheckErrors(self):
		XmlNodeKeyData.CheckErrors(self)
		XmlNodeSimple.CheckErrors(self)


class XmlNodeLUT(layout_module.XmlNodeLUT, XmlNodeArchive):
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeLUT.__init__(self, *args, **kwargs)
		XmlNodeArchive.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeTable1(XmlNodeLUT):
	tag = 'Table1'
	variableType = 'LUT<1>'


class XmlNodeTable2(XmlNodeLUT):
	tag = 'Table2'
	variableType = 'LUT<2>'


class XmlNodeTable3(XmlNodeLUT):
	tag = 'Table3'
	variableType = 'LUT<3>'


class XmlNodeBreaks(XmlNodeArchive):
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeElement.__init__(self, *args, **kwargs)
		XmlNodeArchive.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodeBreaks0(XmlNodeBreaks):
	tag = 'Breaks0'
	variableType = ''


class XmlNodeBreaks1(XmlNodeBreaks):
	tag = 'Breaks1'
	variableType = ''


class XmlNodeBreaks2(XmlNodeBreaks):
	tag = 'Breaks2'
	variableType = ''


class XmlNodeValues(XmlNodeArchive):
	tag = 'Values'
	variableType = ''
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeElement.__init__(self, *args, **kwargs)
		XmlNodeArchive.__init__(self, parent, documentOwner, *args, **kwargs)


class XmlNodePath(layout_module.XmlNodePath, XmlNodeSimple):
	tag = 'Path'
	variableType = 'Path'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodePath.__init__(self, *args, **kwargs)
		XmlNodeSimple.__init__(self, parent, documentOwner, *args, **kwargs)
		
		# The XmlObjectDocument referenced by the Path
		self.subDocument = None
		
		# If the subDocument can't be opened, contains the error message
		self.loadError = None
		
		# A cache of the subDocument error counters
		self.subDocumentErrorCount = 0
		self.subDocumentChildrenErrorCount = 0
		
		# Protects on_SubDocumentChanged from being called recursively when in a dependency cycle
		self.subDocumentChangedInProgress = False
	
	def Dispose(self):
		super(XmlNodePath, self).Dispose()
		self.ReleaseSubDocument()
	
	def LoadDependencies(self):
		super(XmlNodePath, self).LoadDependencies()
		self.LoadLocalDependencies()
		self.CheckErrors()
	
	def LoadLocalDependencies(self):
		self.ReleaseSubDocument()
		self.loadError = None
		
		fullName = self.GetFullName()
		if fullName is None:
			return
		
		if not os.path.exists(fullName):
			return
		
		# Get the document from the DocumentRegistry
		documentRegistry = wx.GetApp().GetDocumentRegistry()
		try:
			self.subDocument = documentRegistry.GetOrCreateDocument( self.documentOwner.DocumentFactory(fullName), self.documentOwner )
		except Exception, error:
			self.loadError = str(error)
			return
		
		self.subDocument.GetChangedSignal().Connect(self.on_SubDocumentChanged)
		self.UpdateSubDocumentErrorCount()
	
	def ReleaseSubDocument(self):
		if self.subDocument is not None:
			if self.subDocument.GetChangedSignal() is not None:
				self.subDocument.GetChangedSignal().Disconnect(self.on_SubDocumentChanged)
			wx.GetApp().GetDocumentRegistry().ReleaseDocument(self.subDocument, self.documentOwner)
			self.subDocument = None
			
			self.ChangeChildrenErrorCount(-self.subDocumentErrorCount)
			self.subDocumentErrorCount = 0
			
			self.ChangeChildrenErrorCount(-self.subDocumentChildrenErrorCount)
			self.subDocumentChildrenErrorCount = 0
	
	def GetFullName(self):
		if self.documentOwner.dotPath is None:
			return None
		
		path = self.GetText()
		if not path:
			return None
		
		if path[0] == '.':
			# Absolute path
			path = path.split('.')[1:]
		else:
			# Relative path
			path = self.documentOwner.dotPath[:-1] + path.split('.')
		
		fullName = self.documentOwner.xmlPath
		for name in path:
			if not name:
				return None
			fullName = os.path.join(fullName, name)
		
		return fullName + '.xml'
	
	def GetChildren(self):
		for child in super(XmlNodePath, self).GetChildren():
			yield child
		
		if self.subDocument is not None:
			yield self.subDocument.xmlNodeDocument
	
	def CheckErrors(self):
		super(XmlNodePath, self).CheckErrors()
		
		fullName = self.GetFullName()
		if fullName is None:
			self.SetError( "parseXML", "Invalid path" )
		elif not os.path.exists(fullName):
			self.SetError( "parseXML", 'The file "%s" does not exist' % fullName )
		else:
			self.SetError( "parseXML", None )
		
		if self.loadError is None:
			self.SetError( "loadError", None )
		else:
			self.SetError( "loadError", 'Cannot open xml file "%s":\n%s' % (fullName, self.loadError) )
		
		if self.inDependencyCycle():
			self.SetError( "DependencyCycle", 'Dependency cycles are not allowed in <%s>' % self.tag )
		else:
			self.SetError( "DependencyCycle", None )
		
		if self.subDocument is None:
			self.SetError( "BadClassAttribute", None )
		else:
			selfVariableType = self.GetSelfVariableType()
			if not selfVariableType or selfVariableType[0] != 'type' or selfVariableType[1] != XmlNodeObject.variableType:
				self.SetError( "BadClassAttribute", None )
			else:
				rootElement = self.subDocument.xmlNodeDocument.rootElement
				if not isinstance(rootElement, XmlNodeObject):
					self.SetError( "BadClassAttribute", None )
				else:
					errorMessage = 'Attribute class of root element of "%s" must be %s or a descendant' % (fullName, selfVariableType[2])
					interface = rootElement.GetInterface()
					if interface is None:
						self.SetError( "BadClassAttribute", errorMessage )
					elif interface.isAbstract():
						self.SetError( "BadClassAttribute", 'Attribute class %s of root element of "%s" is abstract' % (rootElement.GetAttributeValue('class'), fullName) )
					elif interface.isSubclass(selfVariableType[2]):
						self.SetError( "BadClassAttribute", None )
					else:
						self.SetError( "BadClassAttribute", errorMessage )
	
	def on_SubDocumentChanged(self, subDocument):
		if not self.subDocumentChangedInProgress:
			self.subDocumentChangedInProgress = True
			self.UpdateSubDocumentErrorCount()
			self.CheckErrors()
			self.IncrementChildrenChangeCount()
			self.documentOwner.EmitChangedSignal()
			self.subDocumentChangedInProgress = False
	
	def UpdateSubDocumentErrorCount(self):
		if self.inDependencyCycle():
			self.ChangeChildrenErrorCount(-self.subDocumentErrorCount)
			self.subDocumentErrorCount = 0
			
			self.ChangeChildrenErrorCount(-self.subDocumentChildrenErrorCount)
			self.subDocumentChildrenErrorCount = 0
		else:
			self.ChangeChildrenErrorCount(len(self.subDocument.xmlNodeDocument.errors) - self.subDocumentErrorCount)
			self.subDocumentErrorCount = len(self.subDocument.xmlNodeDocument.errors)
			
			self.ChangeChildrenErrorCount(self.subDocument.xmlNodeDocument.childrenErrorCount - self.subDocumentChildrenErrorCount)
			self.subDocumentChildrenErrorCount = self.subDocument.xmlNodeDocument.childrenErrorCount
	
	def inDependencyCycle(self):
		if self.subDocument is None:
			return False
		else:
			return self.subDocument in self.documentOwner.GetAllReferrers()
	
	def SetText(self, newText):
		super(XmlNodePath, self).SetText(newText)
		self.LoadLocalDependencies()
		self.CheckErrors()


class XmlNodeListTextItem(XmlNodeChild):
	def __init__(self, parent, documentOwner, value):
		XmlNodeChild.__init__(self, parent, documentOwner)
		self.value = value
	
	def GetChildren(self):
		return []
	
	def GetText(self):
		return self.value
	
	def SetText(self, newText):
		newValue = newText.split()
		if newValue:
			self.value = newValue[0]
			self.parent.TextItemHasChanged()
			self.CheckErrors()
			self.IncrementChangeCount()


class XmlNodeListTextItemInt(XmlNodeIntData, XmlNodeListTextItem):
	tag = 'Int'
	type = 'int'
	group = 'builtin'
	variableTypes = ['int8', 'uint8', 'int16', 'uint16', 'int', 'uint']


class XmlNodeListTextItemFloat(XmlNodeFloatData, XmlNodeListTextItem):
	tag = 'Float'
	type = 'float'
	group = 'builtin'
	variableTypes = ['float', 'double']


class XmlNodeListTextItemReal(XmlNodeRealData, XmlNodeListTextItem):
	tag = 'Real'
	type = 'real'
	group = 'type'
	variableTypes = ['Real']


class XmlNodeListTextItemKey(XmlNodeKeyData, XmlNodeListTextItem):
	tag = 'Key'
	type = 'key'
	group = 'type'
	variableTypes = ['Key']


class XmlNodeList(layout_module.XmlNodeList, XmlNodeContainer):
	tag = 'List'
	variableType = 'vector'
	
	TextItemClasses = [
		XmlNodeListTextItemInt,
		XmlNodeListTextItemFloat,
		XmlNodeListTextItemReal,
		XmlNodeListTextItemKey,
		]
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeList.__init__(self, *args, **kwargs)
		XmlNodeContainer.__init__(self, parent, documentOwner, *args, **kwargs)
		
		# Indicates that the items are defined in text node and not in children elements
		self.hasTextItems = False
		
		# Contains the items of the list when it has a type attribute
		self.textItems = []
		
		# Class of items in self.textItems
		self.TextItemClass = None
	
	def Dispose(self):
		super(XmlNodeList, self).Dispose()
		self.textItems = []
	
	def PostLoad(self):
		self.hasTextItems = False
		self.TextItemClass = None
		typeAttribute = self.attributes.get('type')
		if typeAttribute:
			self.hasTextItems = True
			typeAttributeValue = typeAttribute.GetValue()
			for ItemClass in self.TextItemClasses:
				if typeAttributeValue == ItemClass.type:
					self.TextItemClass = ItemClass
					for value in self.GetText().split():
						item = ItemClass(self, self.documentOwner, value)
						self.textItems.append(item)
						item.CheckErrors()
					break
	
	def GetChildren(self):
		for child in super(XmlNodeList, self).GetChildren():
			yield child
		
		for textItem in self.textItems:
			yield textItem
	
	def CheckErrors(self):
		super(XmlNodeList, self).CheckErrors()
		
		childErrors = ["XmlNodeList.unknownElement", "XmlNodeList.badType"]
		
		selfVariableType = self.GetSelfVariableType()
		if not selfVariableType or selfVariableType[0] != self.variableType:
			if self.hasTextItems and not self.TextItemClass:
				self.SetError( "typeAttribute.unknownType", "Attribute type must be chosen amongst: %s" % ', '.join([ItemClass.type for ItemClass in self.TextItemClasses]) )
			else:
				self.SetError( "typeAttribute.unknownType", None )
			
			self.SetError( "typeAttribute.badType", None )
			self.CleanChildErrors(childErrors)
			return
		else:
			self.SetError( "typeAttribute.unknownType", None )
		
		if self.hasTextItems:
			if self.TextItemClass and selfVariableType[1] == self.TextItemClass.group and selfVariableType[2] in self.TextItemClass.variableTypes:
				self.SetError( "typeAttribute.badType", None )
			else:
				types = [ItemClass.type for ItemClass in self.TextItemClasses if selfVariableType[1] == ItemClass.group and selfVariableType[2] in ItemClass.variableTypes ]
				if len(types) == 1:
					self.SetError( "typeAttribute.badType", "Attribute type must be %s" % types[0] )
				else:
					self.SetError( "typeAttribute.badType", "Attribute type must not be defined" )
			
			for child in self.childNodes:
				if isinstance( child, (XmlNodeText, XmlNodeComment) ):
					child.SetError("XmlNodeList.unknownElement", None)
				else:
					child.SetError("XmlNodeList.unknownElement", "This node is not allowed in <%s> with type Attribute" % self.tag)
		else:
			self.SetError( "typeAttribute.badType", None )
			
			childVariableType = selfVariableType[1:]
			for child in self.childNodes:
				if isinstance( child, XmlNodeComment ):
					child.SetError("XmlNodeList.unknownElement", None)
					child.SetError("XmlNodeList.badType", None)
					continue
				
				if isinstance(child, XmlNodeArchive):
					child.SetError("XmlNodeList.unknownElement", None)
				else:
					child.SetError("XmlNodeList.unknownElement", "This node is not allowed in <%s> without type Attribute" % self.tag)
					child.SetError("XmlNodeList.badType", None)
					continue
				
				if not self.CheckVariableType(childVariableType, child):
					child.SetError( "XmlNodeList.badType", None )
					print "BUG: unknown archive type"
	
	def GetChildVariableType(self, child):
		selfVariableType = self.GetSelfVariableType()
		if selfVariableType and selfVariableType[0] == self.variableType:
			return selfVariableType[1:]
		else:
			return ['']
	
	def GetChildScalarPrototype(self, child):
		return self.GetSelfScalarPrototype()
	
	def TextItemHasChanged(self):
		self.SetText( '\n'.join([textItem.GetText() for textItem in self.textItems]) )


class XmlNodeObject(layout_module.XmlNodeObject, XmlNodeContainer):
	tag = 'Object'
	variableType = 'Link'
	
	def __init__(self, parent, documentOwner, *args, **kwargs):
		layout_module.XmlNodeObject.__init__(self, *args, **kwargs)
		XmlNodeContainer.__init__(self, parent, documentOwner, *args, **kwargs)
		
		# The XmlNode children with a name attribute.
		# If more than one child has the same name attribute,
		# only the first is referenced by namedChildNodes.
		self.namedChildNodes = {}
	
	def Dispose(self):
		super(XmlNodeObject, self).Dispose()
		self.namedChildNodes = {}
	
	def CheckErrors(self):
		super(XmlNodeObject, self).CheckErrors()
		
		childErrors = [
			"XmlNodeObject.parseXML",
			"XmlNodeObject.unknownElement",
			"XmlNodeObject.unknownName",
			"XmlNodeObject.badType",
			]
		
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
			self.SetError("UnknownClassAttribute", None)
			self.SetError("BadClassAttribute", None)
			self.CleanRequiredNameErrors()
			self.CleanChildErrors(childErrors)
			return
		else:
			self.SetError("MissingClassAttribute", None)
		
		classAttributeValue = classAttribute.GetValue()
		interfaceRegistry = csplib.InterfaceRegistry.getInterfaceRegistry()
		interface = interfaceRegistry.getInterface(classAttributeValue)
		
		if interface is None:
			self.SetError("UnknownClassAttribute", "<%s> has an unknown class attribute" % self.tag)
			self.SetError("BadClassAttribute", None)
			self.CleanRequiredNameErrors()
			self.CleanChildErrors(childErrors)
			return
		else:
			self.SetError("UnknownClassAttribute", None)
		
		selfVariableType = self.GetSelfVariableType()
		if selfVariableType and selfVariableType[0] == 'type' and selfVariableType[1] == self.variableType:
			if interface.isAbstract():
				self.SetError("BadClassAttribute", "Attribute class %s is abstract" % classAttributeValue)
			else:
				if interface.isSubclass(selfVariableType[2]):
					self.SetError("BadClassAttribute", None)
				else:
					self.SetError("BadClassAttribute", "Attribute class must be %s or a descendant" % selfVariableType[2])
		else:
			self.SetError("BadClassAttribute", None)
		
		parseXMLError = None
		if not interface.isAbstract():
			object = interface.createObject()
			try:
				object.parseXML( self.GetText() )
			except csplib.ParseException, error:
				parseXMLError = error.getMessage()
				error.clear()
		
		requiredNames = interface.getRequiredNames()
		
		requiredNameErrors = [ error for error in self.errors if error.startswith('requiredName_') ]
		for error in requiredNameErrors:
			requiredName = error[len('requiredName_'):]
			if requiredName not in requiredNames:
				self.SetError(error, None)
		
		for requiredName in requiredNames:
			if requiredName in self.namedChildNodes:
				self.SetError("requiredName_%s" % requiredName, None)
			else:
				self.SetError("requiredName_%s" % requiredName, "Required variable %s is missing" % requiredName)
		
		for child in self.childNodes:
			if isinstance( child, XmlNodeComment ):
				child.SetError("XmlNodeObject.parseXML", None)
				child.SetError("XmlNodeObject.unknownElement", None)
				child.SetError("XmlNodeObject.unknownName", None)
				child.SetError("XmlNodeObject.badType", None)
				continue
			
			if isinstance( child, XmlNodeText ):
				child.SetError("XmlNodeObject.parseXML", parseXMLError)
				child.SetError("XmlNodeObject.unknownElement", None)
				child.SetError("XmlNodeObject.unknownName", None)
				child.SetError("XmlNodeObject.badType", None)
				continue
			else:
				child.SetError("XmlNodeObject.parseXML", None)
			
			if isinstance(child, XmlNodeArchive):
				child.SetError("XmlNodeObject.unknownElement", None)
			else:
				child.SetError("XmlNodeObject.unknownElement", "This node is not allowed in <%s>" % self.tag)
				child.SetError("XmlNodeObject.unknownName", None)
				child.SetError("XmlNodeObject.badType", None)
				continue
			
			nameAttribute = child.GetAttributeValue('name')
			if not nameAttribute:
				child.SetError("XmlNodeObject.unknownName", "This element must have a name attribute")
				child.SetError("XmlNodeObject.badType", None)
				continue
			
			if interface.variableExists(nameAttribute):
				child.SetError("XmlNodeObject.unknownName", None)
			else:
				child.SetError( "XmlNodeObject.unknownName", "%s is not a variable of %s" % (nameAttribute, classAttributeValue) )
				child.SetError( "XmlNodeObject.badType", None )
				continue
			
			childVariableType = interface.variableType(nameAttribute).split('::')
			
			if childVariableType[0] == XmlNodeList.variableType:
				self.CheckChildType(child, XmlNodeList)
			elif not self.CheckVariableType(childVariableType, child):
				child.SetError( "XmlNodeObject.badType", None )
				print "BUG: unknown archive type"
	
	def CleanRequiredNameErrors(self):
		requiredNameErrors = [ error for error in self.errors if error.startswith('requiredName_') ]
		for error in requiredNameErrors:
			self.SetError(error, None)
	
	def GetInterface(self):
		interfaceRegistry = csplib.InterfaceRegistry.getInterfaceRegistry()
		return interfaceRegistry.getInterface( self.GetAttributeValue('class') )
	
	def GetChildVariableType(self, child):
		interface = self.GetInterface()
		if interface is not None:
			nameAttribute = child.GetAttributeValue('name')
			if nameAttribute:
				if interface.variableExists(nameAttribute):
					return interface.variableType(nameAttribute).split('::')
		return ['']
	
	def GetChildScalarPrototype(self, child):
		interface = self.GetInterface()
		if interface is not None:
			nameAttribute = child.GetAttributeValue('name')
			if nameAttribute:
				if interface.variableExists(nameAttribute):
					return interface.getScalarPrototype(nameAttribute)
		return None
