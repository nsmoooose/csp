#!/usr/bin/env python
from FileDocument import FileDocument
from ..data.XmlNode import XmlNodeDocument
from ..data.XmlNode import XmlNodeFactory

class XmlDocument(FileDocument):
	"""Document representing an object in a XML file."""
	
	def __init__(self, *args, **kwargs):
		FileDocument.__init__(self, *args, **kwargs)
		
		self.xmlNodeDocument = None
		self.nodeFactory = None
	
	def Dispose(self):
		FileDocument.Dispose(self)
		self.xmlNodeDocument.Dispose()
		self.xmlNodeDocument = None
		self.nodeFactory = None
	
	def New(self):
		if self.xmlNodeDocument is not None:
			self.xmlNodeDocument.Dispose()
		self.xmlNodeDocument = XmlNodeDocument(self)
		self.xmlNodeDocument.New()
	
	def Load(self):
		if self.xmlNodeDocument is not None:
			self.xmlNodeDocument.Dispose()
		self.xmlNodeDocument = XmlNodeDocument(self)
		self.xmlNodeDocument.Load(self.GetFileName())
	
	def Save(self):
		self.xmlNodeDocument.Save(self.GetFileName() + ".new.xml")
	
	def IsReadOnly(self):
		return False
	
	def NodeFactory(self):
		if self.nodeFactory is None:
			self.nodeFactory = XmlNodeFactory(self)
		return self.nodeFactory
	
	def GetXmlNodeDocument(self):
		return self.xmlNodeDocument
