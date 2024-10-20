import wx

class NoPropertiesPane(wx.ScrolledWindow):
    def __init__(self, parent, label = "No current document", *args, **kwargs):
        wx.ScrolledWindow.__init__(self, parent, *args, **kwargs)
        
        self.SetScrollRate(10, 10)
        
        sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(sizer)
        
        sizer.AddStretchSpacer()
        
        staticText = wx.StaticText(self, label = label, style = wx.ALIGN_CENTRE)
        sizer.Add(staticText, flag = wx.EXPAND)
        
        sizer.AddStretchSpacer()
    
    def Dispose(self):
        pass
