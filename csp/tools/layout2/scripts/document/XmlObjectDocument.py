#!/usr/bin/env python
from XmlDocument import XmlDocument
from ..data.XmlNodeArchive import XmlNodeObjectDocument

class XmlObjectDocument(XmlDocument):
	"""Document representing an object in a XML file."""
	
	def __init__(self, *args, **kwargs):
		XmlDocument.__init__(self, *args, **kwargs)
	
	def Load(self):
		self.xmlNodeDocument = XmlNodeObjectDocument(self, self.GetFileName())
