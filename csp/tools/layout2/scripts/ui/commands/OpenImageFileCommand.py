#!/usr/bin/env python
import os.path
import wx
from FileCommand import FileCommand
from csp.tools.layout2.scripts.document.ImageDocument import ImageDocument

class OpenImageFileCommand(FileCommand):
	"""Opens a single image file in its own window within
	the user interface. """

	def GetCaption(self):
		return "Open image file"

	def GetToolTipText(self):
		return "Opens an existing Open Scene Graph model file (.osg, .ive) file"

	def GetToolBarImageName(self):
		return "document-open.png"

	def Execute(self):

		# Get the application object. This object is used to retreive the
		# configuration object and the top window for this application.
		application = wx.GetApp()

		# Get the top window for this application. The top window shall be the 
		# parent for the open file dialog.
		topWindow = application.GetTopWindow()
		if topWindow == None:
			return

		# Retreive the filename. It is set by the parent class.
		fileName = self.GetFileName()

		# Create a document and add the root node to it. This will in
		# turn signal the document added signal that is caught in the gui.
		# This will create a 3D view.
		document = ImageDocument(os.path.basename(fileName))
		document.SetFileName(fileName)
		application.GetDocumentRegistry().Add(document)
		application.GetDocumentRegistry().SetCurrentDocument(document)
