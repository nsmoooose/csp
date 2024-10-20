import wx
import wx.aui

from csp.base.signals import Signal

class DocumentNotebook(wx.aui.AuiNotebook):
    """This class handles the UI of all opened documents in tab views.
    
    Opened documents is displayed in a notebook (tab control)
    where each page is a view on a single document.
    
    There can be multiple views on the same document.
    """

    Instance = None

    def __init__(self, *args, **kwargs):
        wx.aui.AuiNotebook.__init__(self, style = wx.NO_BORDER | wx.aui.AUI_NB_DEFAULT_STYLE | wx.aui.AUI_NB_WINDOWLIST_BUTTON, *args, **kwargs)
        
        self.pageUnselectedSignal = Signal()
        self.pageSelectedSignal = Signal()
        self.inhibitCurrentDocumentChange = False
        
        self.Bind(wx.aui.EVT_AUINOTEBOOK_PAGE_CHANGED, self.on_PageChanged)
        self.Bind(wx.aui.EVT_AUINOTEBOOK_PAGE_CLOSE, self.on_PageClose)
    
    def on_PageChanged(self, event):
        if not self.inhibitCurrentDocumentChange:
            oldSelection = event.GetOldSelection()
            if oldSelection != -1:
                oldPage = self.GetPage(oldSelection)
                oldPage.on_PageUnselected()
                self.pageUnselectedSignal.Emit(oldPage)
            newSelection = event.GetSelection()
            newPage = self.GetPage(newSelection)
            newPage.on_PageSelected()
            self.pageSelectedSignal.Emit(newPage)
    
    def on_PageClose(self, event):
        documentRegistry = wx.GetApp().GetDocumentRegistry()
        documentRegistry.SetCurrentDocument(None)
        documentRegistry.SetActiveDocument(None)
        
        currentSelection = event.GetSelection()
        currentPage = self.GetPage(currentSelection)
        currentPage.Dispose()

    def GetPageUnselectedSignal(self):
        """Whenever the current page is replaced by another
        this signal is emitted to all listeners. The page
        is attached to the signal as the first argument."""
        return self.pageUnselectedSignal

    def GetPageSelectedSignal(self):
        """Whenever the current page is replaced by another
        this signal is emitted to all listeners. The page
        is attached to the signal as the first argument."""
        return self.pageSelectedSignal
    
    def AddDocumentPage(self, PageClass, document, caption = None, select = True):
        page = PageClass(self, style = wx.BORDER_NONE)
        page.SetDocument(document)
        if caption is None:
            caption = document.GetName()
        self.AddPage(page, caption, select)
        return page
    
    def CloseCurrentPage(self):
        currentSelection = self.GetSelection()
        if currentSelection == -1:
            return None
        
        documentRegistry = wx.GetApp().GetDocumentRegistry()
        documentRegistry.SetCurrentDocument(None)
        documentRegistry.SetActiveDocument(None)
        
        currentPage = self.GetPage(currentSelection)
        currentPage.Dispose()
        self.DeletePage(currentSelection)
    
    def CloseAllPages(self):
        # Prevent on_PageChanged to set a new CurrentDocument
        # while we delete all pages one after the other.
        self.inhibitCurrentDocumentChange = True
        
        documentRegistry = wx.GetApp().GetDocumentRegistry()
        documentRegistry.SetCurrentDocument(None)
        documentRegistry.SetActiveDocument(None)
        
        while self.GetPageCount() != 0:
            currentPage = self.GetPage(0)
            currentPage.Dispose()
            self.DeletePage(0)
        
        self.inhibitCurrentDocumentChange = False
    
    def GetCurrentPage(self):
        currentSelection = self.GetSelection()
        if currentSelection == -1:
            return None
        return self.GetPage(currentSelection)
    
    def SetCurrentPage(self, page):
        pageIndex = self.GetPageIndex(page)
        if pageIndex == wx.NOT_FOUND:
            return
        self.SetSelection(pageIndex)
    
    def GetAllDocumentPages(self, document):
        allDocumentPages = []
        pageCount = self.GetPageCount()
        for pageIndex in range(pageCount):
            page = self.GetPage(pageIndex)
            if page.GetDocument() == document:
                allDocumentPages.append( page )
        return allDocumentPages
