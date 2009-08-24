#!/usr/bin/env python
from XmlDocument import XmlDocument
from ..data.XmlNodeArchive import XmlNodeArchiveFactory
from ..data.XmlNodeArchive import XmlNodeObjectDocument

class XmlObjectDocument(XmlDocument):
	"""Document representing an object in a XML file."""
	
	def __init__(self, *args, **kwargs):
		XmlDocument.__init__(self, *args, **kwargs)
	
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
