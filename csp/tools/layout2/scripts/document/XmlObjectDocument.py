#!/usr/bin/env python
import os
from XmlDocument import XmlDocument
from ..data.XmlNodeArchive import XmlNodeArchiveFactory
from ..data.XmlNodeArchive import XmlNodeObjectDocument

class XmlObjectDocument(XmlDocument):
	"""Document representing an object in a XML file."""
	
	def __init__(self, xmlPath, *args, **kwargs):
		XmlDocument.__init__(self, *args, **kwargs)
		
		# The root path of CSP Archive XML files
		self.xmlPath = xmlPath
		
		# The relative dot path below xmlPath
		self.dotPath = None
		
		if self.xmlPath is None:
			return
		
		self.xmlPath = os.path.abspath( self.xmlPath )
		filePath = os.path.abspath( self.GetFileName() )
		if filePath.startswith(self.xmlPath):
			filePath = filePath[len(self.xmlPath):]
			self.dotPath = []
			while True:
				(filePath, tail) = os.path.split(filePath)
				if tail != '':
					self.dotPath.insert(0, tail)
				else:
					break;
	
	def New(self):
		if self.xmlNodeDocument is not None:
			self.xmlNodeDocument.Dispose()
		self.xmlNodeDocument = XmlNodeObjectDocument(self)
		self.xmlNodeDocument.New()
	
	def Load(self):
		if self.xmlNodeDocument is not None:
			self.xmlNodeDocument.Dispose()
		self.xmlNodeDocument = XmlNodeObjectDocument(self)
		self.xmlNodeDocument.Load(self.GetFileName())
	
	def NodeFactory(self):
		if self.nodeFactory is None:
			self.nodeFactory = XmlNodeArchiveFactory(self)
		return self.nodeFactory
	
	def DocumentFactory(self, fileName):
		return XmlObjectDocumentFactory(self.xmlPath, fileName)


class XmlObjectDocumentFactory():
	def __init__(self, xmlPath, fileName):
		self.xmlPath = xmlPath
		self.fileName = fileName
	
	def GetUniqueId(self):
		"""Returns a unique Id identifying the document in the DocumentRegistry."""
		return XmlObjectDocument.MakeUniqueId(self.fileName)
	
	def CreateDocument(self):
		"""Returns a new document that will be added in the DocumentRegistry."""
		document = XmlObjectDocument(self.xmlPath, self.fileName)
		try:
			document.Load()
		except:
			document.Dispose()
			raise
		return document
