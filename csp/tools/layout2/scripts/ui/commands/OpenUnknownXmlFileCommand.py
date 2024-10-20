import wx
from .FileCommand import FileCommand
from ...document.XmlDocument import XmlDocumentFactory
from ..controls.XmlWindow import XmlWindow
from ..controls.DocumentNotebook import DocumentNotebook

class OpenUnknownXmlFileCommand(FileCommand):
    caption = "Open unknown XML file"
    tooltip = "Opens an existing unknown XML file"
    toolbarimage = "document-open"

    def Execute(self):
        # Get the document from the DocumentRegistry
        application = wx.GetApp()
        documentRegistry = application.GetDocumentRegistry()
        document = documentRegistry.GetOrCreateDocument( XmlDocumentFactory( self.GetFileName() ) )

        # Create a XmlWindow for the document and add it to the DocumentNotebook
        DocumentNotebook.Instance.AddDocumentPage(XmlWindow, document)
