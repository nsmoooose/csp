#!/usr/bin/env python
import wx
from FileCommand import FileCommand
from ...document.ImageDocument import ImageDocumentFactory
from ..controls.ImageWindow import ImageWindow
from ..controls.DocumentNotebook import DocumentNotebook

class OpenImageFileCommand(FileCommand):
	"""Opens a single image file in its own window within
	the user interface. """

	def GetCaption(self):
		return "Open image file"

	def GetToolTipText(self):
		return "Opens an existing Open Scene Graph model file (.osg, .ive) file"

	def GetToolBarImageName(self):
		return "document-open"

	def Execute(self):

		# Get the application object. This object is used to retreive the
		# configuration object and the top window for this application.
		application = wx.GetApp()

		# Retreive the filename. It is set by the parent class.
		fileName = self.GetFileName()
		
		# Get the document from the DocumentRegistry
		documentRegistry = application.GetDocumentRegistry()
		document = documentRegistry.GetOrCreateDocument( ImageDocumentFactory(fileName) )

		# Create an ImageWindow for the document and add it to the DocumentNotebook
		DocumentNotebook.Instance.AddDocumentPage(ImageWindow, document)
