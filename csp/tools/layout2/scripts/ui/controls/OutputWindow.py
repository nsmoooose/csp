import wx

from .DocumentNotebookPage import DocumentNotebookPage


class OutputWindow(wx.Panel, DocumentNotebookPage):
    """This window holds a multi line text box that displays
    a log of events from any process that is sending message
    signals. For example when we are compiling a new sim.dar
    file."""

    def __init__(self, *args, **kwargs):
        wx.Panel.__init__(self, *args, **kwargs)

        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(self.sizer)

        self.textCtrl = wx.TextCtrl(self, style=wx.BORDER_NONE | wx.TE_MULTILINE | wx.TE_READONLY | wx.TE_DONTWRAP)
        self.sizer.Add(self.textCtrl, proportion=1, flag=wx.EXPAND)

        self.outputDocument = None
        self.nbLines = 0

    def Dispose(self):
        self.outputDocument.GetChangedSignal().Disconnect(self.on_DocumentChanged)
        documentRegistry = wx.GetApp().GetDocumentRegistry()
        documentRegistry.ReleaseDocument(self.outputDocument)
        self.outputDocument = None

    def SetDocument(self, outputDocument):
        # Store a reference to the outputDocument for later use.
        self.outputDocument = outputDocument
        self.outputDocument.GetChangedSignal().Connect(self.on_DocumentChanged)

        # Call the changed method in order to display the content
        # from the outputDocument.
        self.textCtrl.Clear()
        self.nbLines = 0
        self.on_DocumentChanged(outputDocument)

    def GetDocument(self):
        return self.outputDocument

    def on_DocumentChanged(self, outputDocument):
        lines = outputDocument.GetLines()
        newNbLines = len(lines)

        if self.nbLines > newNbLines:
            # the outputDocument has been cleared
            self.textCtrl.Clear()
            self.nbLines = 0

        for line in lines[self.nbLines:]:
            self.textCtrl.AppendText(line + '\n')

        self.nbLines = newNbLines
