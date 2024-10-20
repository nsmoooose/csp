import wx

from .DocumentNotebookPage import DocumentNotebookPage

class ImageWindow(wx.ScrolledWindow, DocumentNotebookPage):
    """This window holds a single bitmap control. It is used to display
    an image loaded."""

    def __init__(self, *args, **kwargs):
        wx.ScrolledWindow.__init__(self, *args, **kwargs)
        self.document = None
        
        self.SetScrollRate(1, 1)
        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(self.sizer)

    def Dispose(self):
        documentRegistry = wx.GetApp().GetDocumentRegistry()
        documentRegistry.ReleaseDocument(self.document)
        self.document = None
    
    def SetDocument(self, document):
        # Store a reference to the document for later use.
        self.document = document

        bitmap = wx.Bitmap(document.GetFileName())
        bitmapControl = wx.StaticBitmap(self, bitmap = bitmap)
        self.sizer.Add(bitmapControl)
    
    def GetDocument(self):
        return self.document
