#!/usr/bin/env python
import wx
from FileCommand import FileCommand
from ...document.ModelDocument import ModelDocument
from ..controls.ModelWindow import ModelWindow
from ..controls.DocumentNotebook import DocumentNotebook

class OpenOsgModelFileCommand(FileCommand):
    """Opens a single Open Scene Graph model file in its own window within
    the user interface. """

    def GetCaption(self):
        return "Open model file"

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
        document = documentRegistry.GetOrCreateDocument( ModelDocumentFactory(fileName) )

        # Create a ModelWindow for the document and add it to the DocumentNotebook
        DocumentNotebook.Instance.AddDocumentPage(ModelWindow, document)


class ModelDocumentFactory():
    def __init__(self, fileName):
        self.fileName = fileName
    
    def GetUniqueId(self):
        """Returns a unique Id identifying the document in the DocumentRegistry."""
        return ModelDocument.MakeUniqueId(self.fileName)
    
    def CreateDocument(self):
        """Returns a new document that will be added in the DocumentRegistry."""
        return ModelDocument(self.fileName)
