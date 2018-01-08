import wx
from .FileCommand import FileCommand
from ...document.ModelDocument import ModelDocumentFactory
from ..controls.ModelWindow import ModelWindow
from ..controls.DocumentNotebook import DocumentNotebook

class OpenOsgModelFileCommand(FileCommand):
    """Opens a single Open Scene Graph model file in its own window within
    the user interface. """

    caption = "Open model file"
    tooltip = "Opens an existing Open Scene Graph model file (.osg, .ive) file"
    toolbarimage = "document-open"

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
