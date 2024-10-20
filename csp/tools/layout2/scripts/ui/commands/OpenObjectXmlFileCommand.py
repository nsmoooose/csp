import os
import wx
from .FileCommand import FileCommand
from ...document.XmlObjectDocument import XmlObjectDocumentFactory
from ..controls.DocumentNotebook import DocumentNotebook
from ..controls.TerrainWindow import TerrainWindow
from ..controls.XmlWindow import XmlWindow

class OpenObjectXmlFileCommand(FileCommand):
    caption = "Open CSP Archive XML file"
    tooltip = "Opens an existing CSP Archive XML file"
    toolbarimage = "document-open"

    def GetPageType(self):
        return XmlWindow

    def Execute(self):
        # Get the current xml path settings.
        application = wx.GetApp()
        xmlPath = application.Configuration.get('LayoutApplication.XmlPath', None)

        # Do a safety check that the file exists under the current project directory.
        if xmlPath is not None and not os.path.abspath( self.GetFileName() ).startswith(os.path.abspath(xmlPath)):
            messageDialog = wx.MessageDialog(wx.GetApp().GetTopWindow(), 
                'Cannot open CSP Archive XML files that are outside the XML path.\n\n'
                'The current XML path is "%s".\n'
                'The file to open is "%s".' % ( xmlPath, self.GetFileName() ),
                'Error loading CSP Archive XML file', style = wx.OK | wx.ICON_ERROR)
            messageDialog.ShowModal()
            return None

        # Get the document from the DocumentRegistry
        documentRegistry = application.GetDocumentRegistry()
        document = documentRegistry.GetOrCreateDocument( XmlObjectDocumentFactory( xmlPath, self.GetFileName() ) )

        # Create a Page for the document and add it to the DocumentNotebook
        DocumentNotebook.Instance.AddDocumentPage(self.GetPageType(), document)

class OpenTerrainObjectXmlFileCommand(OpenObjectXmlFileCommand):
    def GetPageType(self):
        return TerrainWindow
