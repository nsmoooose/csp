#!/usr/bin/env python
from FileDocument import FileDocument
from ..data.XmlNodeArchive import XmlNodeDocument

class XmlDocument(FileDocument):
	"""Document representing an object in a XML file."""
	
	def __init__(self, *args, **kwargs):
		FileDocument.__init__(self, *args, **kwargs)
		
		self.xmlNodeDocument = None
	
	def Dispose(self):
		FileDocument.Dispose(self)
		self.xmlNodeDocument.Dispose()
		self.xmlNodeDocument = None
	
	def Load(self):
		self.xmlNodeDocument = XmlNodeDocument(self, self.GetFileName())
	
	def Save(self):
		self.xmlNodeDocument.SaveTo(self.GetFileName() + ".new.xml")
	
	def IsReadOnly(self):
		return False
