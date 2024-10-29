import wx

class AutoFitTextCtrl(wx.TextCtrl):
    CharWidth = None
    CharHeight = None
    MarginLineHeight = None
    
    @staticmethod
    def initCharSize(parent):
        if AutoFitTextCtrl.CharWidth is None:
            text = wx.TextCtrl(parent)
            AutoFitTextCtrl.CharWidth = text.GetCharWidth()
            AutoFitTextCtrl.CharHeight = text.GetCharHeight()
            width, height = text.GetSize()
            AutoFitTextCtrl.MarginLineHeight = height - AutoFitTextCtrl.CharHeight
            text.Destroy()
    
    def __init__(self, parent, value = wx.EmptyString, style = 0, *args, **kwargs):
        AutoFitTextCtrl.initCharSize(parent)
        
        lines = value.splitlines()
        if lines:
            width = max( len(line) for line in lines )
        else:
            width = 0
        height = max( len(lines), 1 )
        if height > 1:
            style |= wx.TE_MULTILINE
            MarginLineWidth = 5
        else:
            MarginLineWidth = 2
        width = (width + MarginLineWidth) * AutoFitTextCtrl.CharWidth
        height = height * AutoFitTextCtrl.CharHeight + AutoFitTextCtrl.MarginLineHeight
        
        wx.TextCtrl.__init__(self, parent, value = value, style = style, size = (width, height), *args, **kwargs)
