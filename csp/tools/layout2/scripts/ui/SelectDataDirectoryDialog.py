import wx
import os

class SelectDataDirectoryDialog(wx.Dialog):
    """A dialog to be used to display a splash window for
    this layout tool. You must also select a valid data
    directory for this application to work with. After
    you press OK the main frame is loaded and you can begin
    to edit your layout of CSP."""

    def __init__(self, *args, **kwargs):
        wx.Dialog.__init__(self, *args, **kwargs)

        horizontalSizer = wx.BoxSizer(wx.HORIZONTAL)
        self.SetSizer(horizontalSizer)

        bitmap = wx.ArtProvider.GetBitmap('splash')
        bitmapControl = wx.StaticBitmap(self, wx.ID_ANY, bitmap)
        horizontalSizer.Add(bitmapControl, flag=wx.RIGHT|wx.ALIGN_CENTER, border=10)

        rightPanel = wx.Panel(self, wx.ID_ANY)
        horizontalSizer.Add(rightPanel, flag=wx.TOP|wx.BOTTOM|wx.RIGHT|wx.EXPAND, border=10, proportion=1)

        verticalSizer = wx.BoxSizer(wx.VERTICAL)
        rightPanel.SetSizer(verticalSizer)

        cspLabel = wx.StaticText(rightPanel, wx.ID_ANY, 'Combat Simulator Project')
        verticalSizer.Add(cspLabel)

        infoLabel = wx.StaticText(rightPanel, wx.ID_ANY, 'Select the data directory of your CSP installation.')
        verticalSizer.Add(infoLabel)


        directoryLabel = wx.StaticText(rightPanel, wx.ID_ANY, 'Directory:')
        verticalSizer.Add(directoryLabel, border=5, flag=wx.TOP)

        directoryPanel = wx.Panel(rightPanel, wx.ID_ANY)
        verticalSizer.Add(directoryPanel, flag=wx.EXPAND)
        directorySizer = wx.BoxSizer(wx.HORIZONTAL)
        directoryPanel.SetSizer(directorySizer)
        self.directoryText = wx.TextCtrl(directoryPanel, wx.ID_ANY, '')
        self.directoryText.SetMinSize(wx.Size(300, self.directoryText.GetSize().GetHeight()))
        directorySizer.Add(self.directoryText, flag=wx.RIGHT, border=5)
        browseDirectoryButton = wx.Button(directoryPanel, label='...', size=wx.Size(20,20))
        browseDirectoryButton.Bind(wx.EVT_BUTTON, self.browseDirectoryButton_Click)
        directorySizer.Add(browseDirectoryButton)

        verticalSizer.AddStretchSpacer()

        buttonPanel = wx.Panel(rightPanel, wx.ID_ANY)
        verticalSizer.Add(buttonPanel, border=5, flag=wx.TOP)
        buttonSizer = wx.BoxSizer(wx.HORIZONTAL)
        buttonPanel.SetSizer(buttonSizer)
        okButton = wx.Button(buttonPanel, wx.ID_OK, 'OK')
        okButton.SetFocus()
        buttonSizer.Add(okButton, border=5, flag=wx.RIGHT)
        cancelButton = wx.Button(buttonPanel, wx.ID_CANCEL, 'Cancel')
        buttonSizer.Add(cancelButton)

        # Force the window to resize itself according to its content.
        horizontalSizer.SetSizeHints(self)

        # Store a reference to the text control containing
        # the selected directory.
        application = wx.GetApp()
        self.directoryText.SetValue(application.Configuration.get('LayoutApplication.DataDirectory', '.'))
        
        # Bind events to buttons
        wx.EVT_BUTTON(self, wx.ID_OK, self.okButton_Click)

    def browseDirectoryButton_Click(self, event):
        # Use the current value in the text box. This will
        # be the default directory in the directory browser.
        defaultDataDirectory = self.directoryText.GetValue()
        
        # Show the directory browser.
        directoryBrowser = wx.DirDialog(None, 'Select the data directory for your csp application', defaultDataDirectory)
        if directoryBrowser.ShowModal() != wx.ID_OK:
            return False

        # Set the choosen directory into the text box.
        self.directoryText.SetValue(directoryBrowser.GetPath())

    def okButton_Click(self, event):
        dataDirectory = self.directoryText.GetValue().encode('utf8')
        application = wx.GetApp()
        application.Configuration['LayoutApplication.DataDirectory'] = dataDirectory
        application.Configuration['LayoutApplication.XmlPath'] = os.path.join(dataDirectory, 'xml')
        self.EndModal(wx.ID_OK)
