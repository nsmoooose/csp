import wx


class OutputPane(wx.Panel):
    """This window holds a multi line text box that displays
    a log of events from any process that is sending message
    signals. For example when we are compiling a new sim.dar
    file."""

    Instance = None

    def __init__(self, parent, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)

        sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.SetSizer(sizer)

        self.textCtrl = wx.TextCtrl(self, style=wx.TE_MULTILINE | wx.TE_READONLY | wx.TE_DONTWRAP)
        sizer.Add(self.textCtrl, proportion=1, flag=wx.EXPAND)

    def Clear(self):
        self.textCtrl.Clear()

    def AppendText(self, text):
        self.textCtrl.AppendText(text)

    def AppendLine(self, text):
        self.textCtrl.AppendText(text + '\n')
